/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsX8664AMD64
 *
 * @brief APIC implementation
 */

/*
 * Copyright (C) 2024 Matheus Pecoraro
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <acpi/acpi.h>
#include <apic.h>
#include <assert.h>
#include <bsp.h>
#include <pic.h>
#include <rtems/score/idt.h>

extern void apic_spurious_handler(void);

volatile uint32_t* amd64_lapic_base;

#ifdef RTEMS_SMP
/* Maps the processor index to the Local APIC ID */
uint8_t amd64_lapic_to_cpu_map[xAPIC_MAX_APIC_ID + 1];
static uint8_t cpu_to_lapic_map[xAPIC_MAX_APIC_ID + 1];
static uint32_t lapic_count = 0;
#endif

/**
 * @brief Returns wheather the system contains a local APIC or not.
 *
 * When the CPUID instruction is executed with a source operand of 1 in the EAX
 * register, bit 9 of the CPUID feature flags returned in the EDX register
 * indicates the presence (set) or absence (clear) of a local APIC.
 *
 * @return true if CPUID reports the presence of a local APIC.
 */
static bool has_lapic_support(void)
{
  uint32_t eax, ebx, ecx, edx;
  cpuid(1, &eax, &ebx, &ecx, &edx);
  return (edx >> 9) & 1;
}

/**
 * @brief Passed to acpi_walk_subtables to parse information from MADT entries
 *
 * @param entry An entry of the table passed to acpi_walk_subtables
 */
static void madt_subtables_handler(ACPI_SUBTABLE_HEADER* entry)
{
  switch (entry->Type) {
#ifdef RTEMS_SMP
    case ACPI_MADT_TYPE_LOCAL_APIC:
      ACPI_MADT_LOCAL_APIC* lapic_entry = (ACPI_MADT_LOCAL_APIC*) entry;

      if (lapic_count >= xAPIC_MAX_APIC_ID + 1 ||
          lapic_get_id() == lapic_entry->Id) {
        break;
      }

      amd64_lapic_to_cpu_map[lapic_entry->Id] = (uint8_t) lapic_count;
      cpu_to_lapic_map[lapic_count++] = lapic_entry->Id;
      break;
#endif
    case ACPI_MADT_TYPE_LOCAL_APIC_OVERRIDE:
      ACPI_MADT_LOCAL_APIC_OVERRIDE* lapic_override =
                                        (ACPI_MADT_LOCAL_APIC_OVERRIDE*) entry;
      amd64_lapic_base = (uint32_t*) lapic_override->Address;
      break;
    default:
      break;
  }
}

/**
 * @brief Retrieve the MADT and parse the values we need.
 *
 * @return true if successful.
 */
static bool parse_madt(void)
{
  ACPI_STATUS status;
  ACPI_TABLE_MADT* madt;

  status = AcpiGetTable(ACPI_SIG_MADT, 1, (ACPI_TABLE_HEADER**) &madt);
  if (status != (AE_OK)) {
    return false;
  }

#ifdef RTEMS_SMP
  /* Ensure the boot processor is cpu index 0 */
  uint8_t lapic_id = lapic_get_id();
  amd64_lapic_to_cpu_map[lapic_id] = (uint8_t) lapic_count;
  cpu_to_lapic_map[lapic_count++] = lapic_id;
#endif

  amd64_lapic_base = (uint32_t*) ((uintptr_t) madt->Address);
  acpi_walk_subtables(
    (ACPI_TABLE_HEADER*) madt,
    sizeof(ACPI_TABLE_MADT),
    madt_subtables_handler
  );

  return true;
}

/**
 * @brief Calculates the amount of LAPIC timer ticks per second using the PIT.
 *
 * @return The amount of ticks per second calculated.
 */
static uint32_t lapic_timer_calc_ticks_per_sec(void)
{
  /* Configure LAPIC timer in one-shot mode to prepare for calibration */
  amd64_lapic_base[LAPIC_REGISTER_LVT_TIMER] = BSP_VECTOR_APIC_TIMER;
  amd64_lapic_base[LAPIC_REGISTER_TIMER_DIV] = LAPIC_TIMER_SELECT_DIVIDER;

  uint8_t chan2_value;
  PIT_CHAN2_ENABLE(chan2_value);

  /*
   * Make sure interrupts are disabled while we calibrate for 2 reasons:
   *   - Writing values to the PIT should be atomic (for now, this is okay
   *     because we're the only ones ever touching the PIT ports, but an
   *     interrupt resetting the PIT could mess calibration up).
   *   - We need to make sure that no interrupts throw our synchronization of
   *     the LAPIC timer off.
   */
  rtems_interrupt_level level;
  rtems_interrupt_local_disable(level);

  uint32_t pit_ticks = PIT_CALIBRATE_TICKS;
  PIT_CHAN2_WRITE_TICKS(pit_ticks);

  /* Start LAPIC timer's countdown */
  const uint32_t lapic_calibrate_init_count = 0xffffffff;

  PIT_CHAN2_START_DELAY(chan2_value);
  amd64_lapic_base[LAPIC_REGISTER_TIMER_INITCNT] = lapic_calibrate_init_count;

  PIT_CHAN2_WAIT_DELAY(pit_ticks);
  uint32_t lapic_currcnt = amd64_lapic_base[LAPIC_REGISTER_TIMER_CURRCNT];

  DBG_PRINTF("PIT stopped at 0x%" PRIx32 "\n", pit_ticks);

  /* Stop APIC timer to calculate ticks to time ratio */
  amd64_lapic_base[LAPIC_REGISTER_LVT_TIMER] = LAPIC_LVT_MASK;

  /* Get counts passed since we started counting */
  uint32_t lapic_ticks_per_sec = lapic_calibrate_init_count - lapic_currcnt;

  DBG_PRINTF(
    "APIC ticks passed in 1/%d of a second: 0x%" PRIx32 "\n",
    PIT_CALIBRATE_DIVIDER,
    lapic_ticks_per_sec
  );

  /* We ran the PIT for a fraction of a second */
  lapic_ticks_per_sec = lapic_ticks_per_sec * PIT_CALIBRATE_DIVIDER;

  /* Restore interrupts now that calibration is complete */
  rtems_interrupt_local_enable(level);

  /* Confirm that the APIC timer never hit 0 and IRQ'd during calibration */
  assert(lapic_ticks_per_sec != 0 &&
         lapic_ticks_per_sec != lapic_calibrate_init_count);

  DBG_PRINTF(
    "CPU frequency: 0x%" PRIu64 "\nAPIC ticks/sec: 0x%" PRIu32 "\n",
    /* Multiply to undo effects of divider */
    (uint64_t) lapic_ticks_per_sec * LAPIC_TIMER_DIVIDE_VALUE,
    lapic_ticks_per_sec
  );

  return lapic_ticks_per_sec;
}

#ifdef RTEMS_SMP
/**
 * @brief Sends an interprocessor interrupt
 *
 * @param dest_id Local APIC ID of the destination
 * @param icr_low The flags to be written to the low value of the ICR
 */
static void send_ipi(uint8_t dest_id, uint32_t icr_low)
{
  amd64_lapic_base[LAPIC_REGISTER_ICR_HIGH] =
    (amd64_lapic_base[LAPIC_REGISTER_ICR_HIGH] & LAPIC_ICR_HIGH_MASK) | (dest_id << 24);

  amd64_lapic_base[LAPIC_REGISTER_ICR_LOW] =
    (amd64_lapic_base[LAPIC_REGISTER_ICR_LOW] & LAPIC_ICR_LOW_MASK) | icr_low;
}

static void wait_ipi(void)
{
  while (amd64_lapic_base[LAPIC_REGISTER_ICR_LOW] & LAPIC_ICR_DELIV_STAT_PEND) {
    amd64_spinwait();
  }
}
#endif

bool lapic_initialize(void)
{
  if (has_lapic_support() == false || parse_madt() == false) {
    return false;
  }

  /* Hardware enable the APIC just to be sure */
  uint64_t apic_base_msr = rdmsr(APIC_BASE_MSR);
  wrmsr(
    APIC_BASE_MSR,
    apic_base_msr | APIC_BASE_MSR_ENABLE,
    apic_base_msr >> 32
  );

  DBG_PRINTF("APIC is at 0x%" PRIxPTR "\n", (uintptr_t) amd64_lapic_base);
  DBG_PRINTF(
    "APIC ID at *0x%" PRIxPTR "=0x%" PRIx32 "\n",
    (uintptr_t) &amd64_lapic_base[LAPIC_REGISTER_ID],
    amd64_lapic_base[LAPIC_REGISTER_ID]
  );

  DBG_PRINTF(
    "APIC spurious vector register *0x%" PRIxPTR "=0x%" PRIx32 "\n",
    (uintptr_t) &amd64_lapic_base[LAPIC_REGISTER_SPURIOUS],
    amd64_lapic_base[LAPIC_REGISTER_SPURIOUS]
  );

  /*
   * Software enable the APIC by mapping spurious vector and setting enable bit.
   */
  uintptr_t old;
  amd64_install_raw_interrupt(
    BSP_VECTOR_SPURIOUS,
    (uintptr_t) apic_spurious_handler,
    &old
  );
  amd64_lapic_base[LAPIC_REGISTER_SPURIOUS] =
    LAPIC_SPURIOUS_ENABLE | BSP_VECTOR_SPURIOUS;

  DBG_PRINTF(
    "APIC spurious vector register *0x%" PRIxPTR "=0x%" PRIx32 "\n",
    (uintptr_t) &amd64_lapic_base[LAPIC_REGISTER_SPURIOUS],
    amd64_lapic_base[LAPIC_REGISTER_SPURIOUS]
  );

  /*
   * The PIC may send spurious IRQs even when disabled, and without remapping
   * IRQ7 would look like an exception.
   */
  pic_remap(PIC1_REMAP_DEST, PIC2_REMAP_DEST);
  pic_disable();

  return true;
}

uint32_t lapic_timer_calc_ticks(uint64_t desired_freq_hz)
{
  uint32_t lapic_ticks_per_sec = 0;
  uint64_t lapic_tick_total = 0;
  for (uint32_t i = 0; i < LAPIC_TIMER_NUM_CALIBRATIONS; i++) {
    lapic_tick_total += lapic_timer_calc_ticks_per_sec();
  }
  lapic_ticks_per_sec = lapic_tick_total / LAPIC_TIMER_NUM_CALIBRATIONS;

  /*
   * The APIC timer counter is decremented at the speed of the CPU bus
   * frequency (and we use a frequency divider).
   *
   * This means:
   *   lapic_ticks_per_sec = (cpu_bus_frequency / timer_divide_value)
   *
   * Therefore:
   *   reload_value = lapic_ticks_per_sec / desired_freq_hz
   */
  return lapic_ticks_per_sec / desired_freq_hz;
}

void lapic_timer_enable(uint32_t reload_value)
{
  amd64_lapic_base[LAPIC_REGISTER_LVT_TIMER] = BSP_VECTOR_APIC_TIMER | LAPIC_SELECT_TMR_PERIODIC;
  amd64_lapic_base[LAPIC_REGISTER_TIMER_DIV] = LAPIC_TIMER_SELECT_DIVIDER;
  amd64_lapic_base[LAPIC_REGISTER_TIMER_INITCNT] = reload_value;
}

#ifdef RTEMS_SMP
uint32_t lapic_get_num_of_procesors(void)
{
  return lapic_count;
}

void lapic_send_ipi(uint32_t target_cpu_index, uint8_t isr_vector)
{
  uint8_t target_lapic_id = cpu_to_lapic_map[target_cpu_index];
  send_ipi(target_lapic_id, isr_vector);
  wait_ipi();
}

/**
 * This routine attempts to follow the algorithm described in the
 * Intel Multiprocessor Specification v1.4 in section B.4.
 */
void lapic_start_ap(uint32_t cpu_index, uint8_t page_vector)
{
  if (cpu_index >= lapic_count) {
    return;
  }

  uint8_t lapic_id = cpu_to_lapic_map[cpu_index];

  uint8_t chan2_value;
  PIT_CHAN2_ENABLE(chan2_value);
  uint32_t pit_ticks = PIT_FREQUENCY/100; /* 10 miliseconds */
  PIT_CHAN2_WRITE_TICKS(pit_ticks);

  /* INIT IPI */
  send_ipi(lapic_id, LAPIC_ICR_DELIV_INIT | LAPIC_ICR_ASSERT | LAPIC_ICR_TRIG_LEVEL);
  wait_ipi();
  /* Deassert INIT IPI */
  send_ipi(lapic_id, LAPIC_ICR_DELIV_INIT  | LAPIC_ICR_TRIG_LEVEL);
  /* Wait 10ms */
  PIT_CHAN2_START_DELAY(chan2_value);
  PIT_CHAN2_WAIT_DELAY(pit_ticks);

  pit_ticks = PIT_FREQUENCY/5000; /* 200 microseconds */
  PIT_CHAN2_WRITE_TICKS(pit_ticks);

  /* STARTUP IPI */
  send_ipi(lapic_id, LAPIC_ICR_DELIV_START | page_vector);
  wait_ipi();
  /* Wait 200us */
  PIT_CHAN2_START_DELAY(chan2_value);
  PIT_CHAN2_WAIT_DELAY(pit_ticks);
  /**
   * It is possible that the first STARTUP IPI sent is ignored
   * so we send it twice.
   */
  send_ipi(lapic_id, LAPIC_ICR_DELIV_START | page_vector);
}
#endif
