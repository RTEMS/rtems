/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsX8664AMD64
 *
 * @brief BSP SMP implementation
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

#include <rtems/score/smpimpl.h>

#include <apic.h>
#include <assert.h>
#include <bsp.h>
#include <smp.h>
#include <libcpu/page.h>
#include <rtems/score/idt.h>

#include <string.h>

extern void _Trampoline_start(void);
extern const uint64_t _Trampoline_size;

static bool has_ap_started = false;

/**
 * @brief Copies the trampoline code to the address where APs will boot from.
 *
 * TODO: We should ideally parse through the UEFI Memory Map to find a
 *       free page under 1MB
 */
static void copy_trampoline(void)
{
  /* Copy the trampoline code to its destiny address */
  void* trampoline_dest = (void*) TRAMPOLINE_ADDR;
  memcpy(trampoline_dest, (void*) _Trampoline_start, _Trampoline_size);
}

/**
 * @brief Waits for the Application Processor to set the flag.
 *
 * @param timeout_ms Timeout in miliseconds.
 *
 * @return true if successful.
 */
static bool wait_for_ap(uint32_t timeout_ms)
{
  uint8_t chan2_value;
  uint32_t pit_ticks = PIT_FREQUENCY/1000; /* a milisecond */

  PIT_CHAN2_ENABLE(chan2_value);
  PIT_CHAN2_WRITE_TICKS(pit_ticks);

  for (int i = 0; has_ap_started == false && i < timeout_ms; i++) {
    PIT_CHAN2_START_DELAY(chan2_value);
    PIT_CHAN2_WAIT_DELAY(pit_ticks);
    amd64_spinwait();
  }

  return has_ap_started;
}

static void bsp_inter_processor_interrupt(void* arg)
{
  (void) arg;

  _SMP_Inter_processor_interrupt_handler(_Per_CPU_Get());
  lapic_eoi();
}

void _CPU_SMP_Prepare_start_multitasking(void)
{
  /* Do nothing */
}

bool _CPU_SMP_Start_processor(uint32_t cpu_index)
{
  has_ap_started = false;
  lapic_start_ap(cpu_index, TRAMPOLINE_PAGE_VECTOR);
  return wait_for_ap(WAIT_FOR_AP_TIMEOUT_MS);
}

uint32_t _CPU_SMP_Get_current_processor(void)
{
  uint8_t lapic_id = lapic_get_id();
  return amd64_lapic_to_cpu_map[lapic_id];
}

uint32_t _CPU_SMP_Initialize(void)
{
  copy_trampoline();
  return lapic_get_num_of_procesors();
}

void _CPU_SMP_Finalize_initialization(uint32_t cpu_count)
{
  (void) cpu_count;

  rtems_status_code sc = rtems_interrupt_handler_install(
    BSP_VECTOR_IPI,
    "IPI",
    RTEMS_INTERRUPT_UNIQUE,
    bsp_inter_processor_interrupt,
    NULL
  );
  assert(sc == RTEMS_SUCCESSFUL);
}

void _CPU_SMP_Send_interrupt(uint32_t target_processor_index)
{
  lapic_send_ipi(target_processor_index, BSP_VECTOR_IPI);
}

void smp_init_ap(void)
{
  has_ap_started = true;

  Context_Control_fp* null_fp_context_p = &_CPU_Null_fp_context;
  _CPU_Context_restore_fp(&null_fp_context_p);

  amd64_lapic_base[LAPIC_REGISTER_SPURIOUS] =
    LAPIC_SPURIOUS_ENABLE | BSP_VECTOR_SPURIOUS;

  lidt(&amd64_idtr);

  _SMP_Start_multitasking_on_secondary_processor(_Per_CPU_Get());
}
