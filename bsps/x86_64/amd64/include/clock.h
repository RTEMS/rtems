/*
 * Copyright (c) 2018.
 * Amaan Cheval <amaan.cheval@gmail.com>
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

#ifndef _AMD64_CLOCK_H
#define _AMD64_CLOCK_H

#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ASM
  extern volatile uint32_t *amd64_apic_base;
  bool has_apic_support(void);
  void apic_initialize(void);
  void apic_timer_install_handler(void);
  uint32_t apic_timer_calibrate(void);
  void apic_timer_initialize(uint64_t desired_freq_hz);
  void amd64_clock_driver_initialize(void);
#endif

/* Number of times to calibrate the APIC timer to average it out */
#define APIC_TIMER_NUM_CALIBRATIONS 5
/* Default divide value used by APIC timer */
#define APIC_TIMER_DIVIDE_VALUE     16
/* Value to set in register to pick the divide value above */
#define APIC_TIMER_SELECT_DIVIDER   3

#define PIT_FREQUENCY               1193180
/*
 * The PIT_FREQUENCY determines how many times the PIT counter is decremented
 * per second - therefore, we can calculate how many ticks we set based on what
 * fraction of a second we're okay with spending on calibration
 */
#define PIT_CALIBRATE_DIVIDER       20
#define PIT_CALIBRATE_TICKS       (PIT_FREQUENCY/PIT_CALIBRATE_DIVIDER)
/* Since the PIT only has 2 one-byte registers, the maximum tick value is
 * limited to 16-bits. We can set the PIT to use a frequency divider if
 * needed. */
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

#ifdef __cplusplus
}
#endif

#endif /* _AMD64_CLOCK_H */
