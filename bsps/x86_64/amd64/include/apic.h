/**
 * @file
 *
 * @ingroup RTEMSBSPsX8664AMD64
 *
 * @brief APIC definitions
 */

/*
 * Copyright (c) 2018.
 * Amaan Cheval <amaan.cheval@gmail.com>
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

/*
 * Since the LAPIC registers are contained in an array of 32-bit elements
 * these byte-offsets need to be divided by 4 to index the array.
 */
#define LAPIC_OFFSET(val) (val >> 2)

#define LAPIC_REGISTER_APICID        LAPIC_OFFSET(0x20)
#define LAPIC_REGISTER_EOI           LAPIC_OFFSET(0x0B0)
#define LAPIC_REGISTER_SPURIOUS      LAPIC_OFFSET(0x0F0)
#define LAPIC_REGISTER_LVT_TIMER     LAPIC_OFFSET(0x320)
#define LAPIC_REGISTER_TIMER_INITCNT LAPIC_OFFSET(0x380)
#define LAPIC_REGISTER_TIMER_CURRCNT LAPIC_OFFSET(0x390)
#define LAPIC_REGISTER_TIMER_DIV     LAPIC_OFFSET(0x3E0)

#define LAPIC_LVT_MASK               0x10000

#define LAPIC_EOI_ACK                0
#define LAPIC_SELECT_TMR_PERIODIC    0x20000
#define LAPIC_SPURIOUS_ENABLE        0x100

/* Number of times to calibrate the LAPIC timer to average it out */
#define LAPIC_TIMER_NUM_CALIBRATIONS 5
/* Default divide value used by LAPIC timer */
#define LAPIC_TIMER_DIVIDE_VALUE     16
/* Value to set in register to pick the divide value above */
#define LAPIC_TIMER_SELECT_DIVIDER   3

/* PIT defines used during LAPIC timer calibration */
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

extern volatile uint32_t* amd64_lapic_base;

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
 * @brief Initializes the Local APIC timer
 *
 * Calibrates and initializes the Local APIC timer configuring it to
 * periodically generate interrupts on vector BSP_VECTOR_APIC_TIMER
 *
 * @param desired_freq_hz The desired frequency of the Local APIC timer
 */
void lapic_timer_initialize(uint64_t desired_freq_hz);

/**
 * @brief Signals an end of interrupt to the Local APIC
 */
void inline lapic_eoi(void)
{
  amd64_lapic_base[LAPIC_REGISTER_EOI] = LAPIC_EOI_ACK;
}

#ifdef __cplusplus
}
#endif

#endif /* _AMD64_APIC_H */
