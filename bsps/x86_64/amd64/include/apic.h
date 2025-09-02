/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsX8664AMD64
 *
 * @brief APIC definitions
 */

/*
 * Copyright (C) 2024 Matheus Pecoraro
 * Copyright (c) 2018 Amaan Cheval <amaan.cheval@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _AMD64_APIC_H
#define _AMD64_APIC_H

#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The address of the MSR pointing to the APIC base physical address */
#define APIC_BASE_MSR             0x1B
/* Value to hardware-enable the APIC through the APIC_BASE_MSR */
#define APIC_BASE_MSR_ENABLE      0x800

#define xAPIC_MAX_APIC_ID         0xFE

/*
 * Since the LAPIC registers are contained in an array of 32-bit elements
 * these byte-offsets need to be divided by 4 to index the array.
 */
#define LAPIC_OFFSET(val) (val >> 2)

#define LAPIC_REGISTER_ID            LAPIC_OFFSET(0x20)
#define LAPIC_REGISTER_EOI           LAPIC_OFFSET(0x0B0)
#define LAPIC_REGISTER_SPURIOUS      LAPIC_OFFSET(0x0F0)
#define LAPIC_REGISTER_ICR_LOW       LAPIC_OFFSET(0x300)
#define LAPIC_REGISTER_ICR_HIGH      LAPIC_OFFSET(0x310)
#define LAPIC_REGISTER_LVT_TIMER     LAPIC_OFFSET(0x320)
#define LAPIC_REGISTER_TIMER_INITCNT LAPIC_OFFSET(0x380)
#define LAPIC_REGISTER_TIMER_CURRCNT LAPIC_OFFSET(0x390)
#define LAPIC_REGISTER_TIMER_DIV     LAPIC_OFFSET(0x3E0)

#define LAPIC_LVT_MASK               0x10000

#define LAPIC_ICR_HIGH_MASK          0x00FFFFFF
#define LAPIC_ICR_LOW_MASK           0xFFF32000
#define LAPIC_ICR_DELIV_INIT         0x500
#define LAPIC_ICR_DELIV_START        0x600
#define LAPIC_ICR_DELIV_STAT_PEND    0x1000
#define LAPIC_ICR_ASSERT             0x4000
#define LAPIC_ICR_TRIG_LEVEL         0x8000

#define LAPIC_EOI_ACK                0
#define LAPIC_SELECT_TMR_PERIODIC    0x20000
#define LAPIC_SPURIOUS_ENABLE        0x100

/* Number of times to calibrate the LAPIC timer to average it out */
#define LAPIC_TIMER_NUM_CALIBRATIONS 5
/* Default divide value used by LAPIC timer */
#define LAPIC_TIMER_DIVIDE_VALUE     16
/* Value to set in register to pick the divide value above */
#define LAPIC_TIMER_SELECT_DIVIDER   3

/* PIT defines and macros used when calibrating the LAPIC timer and starting APs */
#define PIT_FREQUENCY               1193180
/*
 * The PIT_FREQUENCY determines how many times the PIT counter is decremented
 * per second - therefore, we can calculate how many ticks we set based on what
 * fraction of a second we're okay with spending on calibration
 */
#define PIT_CALIBRATE_DIVIDER       20
#define PIT_CALIBRATE_TICKS       (PIT_FREQUENCY/PIT_CALIBRATE_DIVIDER)
/*
 * Since the PIT only has 2 one-byte registers, the maximum tick value is
 * limited to 16-bits. We can set the PIT to use a frequency divider if
 * needed.
 */
RTEMS_STATIC_ASSERT(
  PIT_CALIBRATE_TICKS <= 0xffff,
  PIT_CALIBRATE_DIVIDER
);

/* I/O ports for the PIT */
#define PIT_PORT_CHAN0              0x40
#define PIT_PORT_CHAN1              0x41
#define PIT_PORT_CHAN2              0x42
/*
 * The input to channel 2 can be gated through software, using bit 0 of port
 * 0x61.
 */
#define PIT_PORT_CHAN2_GATE         0x61
#define PIT_CHAN2_TIMER_BIT         1
#define PIT_CHAN2_SPEAKER_BIT       2
/* The PIT mode/command register */
#define PIT_PORT_MCR                0x43

/* PIT values to select channels, access, and operating modes */
#define PIT_SELECT_CHAN0            0b00000000
#define PIT_SELECT_CHAN1            0b01000000
#define PIT_SELECT_CHAN2            0b10000000
/*
 * In the lo/hi mode, the low-byte is sent to the data port, followed by the
 * high-byte; this makes it important that this be an atomic operation.
 */
#define PIT_SELECT_ACCESS_LOHI      0b00110000
#define PIT_SELECT_ONE_SHOT_MODE    0b00000010
#define PIT_SELECT_BINARY_MODE      0

#define PIT_CHAN2_ENABLE(chan2_value)                                    \
  /* Enable the channel 2 timer gate and disable the speaker output */   \
  chan2_value = (inport_byte(PIT_PORT_CHAN2_GATE) | PIT_CHAN2_TIMER_BIT) \
    & ~PIT_CHAN2_SPEAKER_BIT;                                            \
  outport_byte(PIT_PORT_CHAN2_GATE, chan2_value);                        \
  /* Initialize PIT in one-shot mode on Channel 2 */                     \
  outport_byte(                                                          \
    PIT_PORT_MCR,                                                        \
    PIT_SELECT_CHAN2 | PIT_SELECT_ACCESS_LOHI |                          \
      PIT_SELECT_ONE_SHOT_MODE | PIT_SELECT_BINARY_MODE                  \
  );                                                                     \

#define PIT_CHAN2_WRITE_TICKS(pit_ticks)                                 \
  /* Set PIT reload value */                                             \
  outport_byte(PIT_PORT_CHAN2, pit_ticks & 0xff);                        \
  stub_io_wait();                                                        \
  outport_byte(PIT_PORT_CHAN2, (pit_ticks >> 8) & 0xff);                 \

#define PIT_CHAN2_START_DELAY(chan2_value)                               \
  /* Restart PIT by disabling the gated input and then re-enabling it */ \
  chan2_value &= ~PIT_CHAN2_TIMER_BIT;                                   \
  outport_byte(PIT_PORT_CHAN2_GATE, chan2_value);                        \
  chan2_value |= PIT_CHAN2_TIMER_BIT;                                    \
  outport_byte(PIT_PORT_CHAN2_GATE, chan2_value);                        \

#define PIT_CHAN2_WAIT_DELAY(pit_ticks)                                  \
  do {                                                                   \
    uint32_t curr_ticks = pit_ticks;                                     \
    while ( curr_ticks <= pit_ticks ) {                                  \
      /* Send latch command to read multi-byte value atomically */       \
      outport_byte(PIT_PORT_MCR, PIT_SELECT_CHAN2);                      \
      curr_ticks = inport_byte(PIT_PORT_CHAN2);                          \
      curr_ticks |= inport_byte(PIT_PORT_CHAN2) << 8;                    \
    }                                                                    \
  } while(0);                                                            \

extern volatile uint32_t* amd64_lapic_base;
extern uint8_t amd64_lapic_to_cpu_map[xAPIC_MAX_APIC_ID + 1];

/**
 * @brief Initializes the Local APIC by hardware and software enabling it.
 *
 * Initializes the Local APIC by hardware and software enabling it, and sets
 * up the amd64_lapic_base pointer that can be used as a 32-bit addressable array to
 * access Local APIC registers.
 *
 * @return true if successful.
 */
bool lapic_initialize(void);

/**
 * @brief Calculates the number of Local APIC timer ticks which can be used
 *        with lapic_timer_enable to set up a timer of given frequency.
 *
 * @param desired_freq_hz The frequency in Hz.
 *
 * @return The number of Local APIC timer ticks.
 */
uint32_t lapic_timer_calc_ticks(uint64_t desired_freq_hz);

/**
 * @brief Enables the Local APIC timer.
 *
 * @param reload_value Number of ticks per interrupt.
 */
void lapic_timer_enable(uint32_t reload_value);

#ifdef RTEMS_SMP
/**
 * @brief Retrieves the number of available processors in the system
 *
 * @return Number of available processors
 */
uint32_t lapic_get_num_of_procesors(void);

/**
 * @brief Sends an interprocessor interrupt to a specified processor.
 *
 * @param target_cpu_index The processor index of the target processor.
 * @param isr_vector The vector of the interrupt being sent.
 */
void lapic_send_ipi(uint32_t target_cpu_index, uint8_t isr_vector);

/**
 * @brief Starts the Application Processor that corresponds to cpu_index.
 *
 * @param cpu_index The processor to be started.
 * @param page_vector The under 1MB 4KB page where the trampoline code is located.
 */
void lapic_start_ap(uint32_t cpu_index, uint8_t page_vector);
#endif

/**
 * @brief Retrieves the Local APIC ID
 * @return Local APIC ID
 */
inline uint8_t lapic_get_id(void)
{
  /* ID stored in highest 8 bits */
  return amd64_lapic_base[LAPIC_REGISTER_ID]>>24;
}

/**
 * @brief Signals an end of interrupt to the Local APIC
 */
inline void lapic_eoi(void)
{
  amd64_lapic_base[LAPIC_REGISTER_EOI] = LAPIC_EOI_ACK;
}

#ifdef __cplusplus
}
#endif

#endif /* _AMD64_APIC_H */
