/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMSMDK2410 
 *
 * @brief S3C2400 clock specific using the System Timer
 */

/*
 * Copyright (C) 2004 Jay Monkman <jtm@lopingdog.com>
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


#include <rtems.h>
#include <bsp/irq.h>
#include <bsp.h>
#include <s3c24xx.h>

void Clock_isr(rtems_irq_hdl_param arg);
static void clock_isr_on(const rtems_irq_connect_data *unused);
static void clock_isr_off(const rtems_irq_connect_data *unused);
static int clock_isr_is_on(const rtems_irq_connect_data *irq);

rtems_irq_connect_data clock_isr_data = {
  .name   = BSP_INT_TIMER4,
  .hdl    = Clock_isr,
  .handle = NULL,
  .on     = clock_isr_on,
  .off    = clock_isr_off,
  .isOn   = clock_isr_is_on,
};

/**
 * When we get the clock interrupt
 *    - clear the interrupt bit?
 *    - restart the timer?
 */
#define Clock_driver_support_at_tick(arg)             \
  do {                                                \
        ClearPending(BIT_TIMER4);                     \
  } while(0)


/**
 * Installs the clock ISR. You shouldn't need to change this.
 */
#define Clock_driver_support_install_isr( _new ) \
  BSP_install_rtems_irq_handler(&clock_isr_data)


/**
 * Initialize the hardware for the clock
 *   - Set the frequency
 *   - enable it
 *   - clear any pending interrupts
 *
 * Since you may want the clock always running, you can
 * enable interrupts here. If you do so, the clock_isr_on(),
 * clock_isr_off(), and clock_isr_is_on() functions can be
 * NOPs.
 */
#define Clock_driver_support_initialize_hardware() \
  do { \
        uint32_t cr; \
        uint32_t freq; \
        /* set MUX for Timer4 to 1/16 */ \
        cr=rTCFG1 & 0xFFF0FFFF; \
        rTCFG1=(cr | (3<<16)); \
        freq = get_PCLK(); \
        /* set TIMER4 counter, input freq=PLCK/16/16Mhz*/ \
        freq = (freq /16)/16; \
        rTCNTB4 = ((freq / 1000) * rtems_configuration_get_microseconds_per_tick()) / 1000; \
        /*unmask TIMER4 irq*/ \
        rINTMSK&=~BIT_TIMER4; \
        /* start TIMER4 with autoreload */ \
        cr=rTCON & 0xFF8FFFFF; \
        rTCON=(cr|(0x6<<20)); \
        rTCON=(cr|(0x5<<20)); \
    } while (0)

/**
 * Enables clock interrupt.
 *
 * If the interrupt is always on, this can be a NOP.
 */
static void clock_isr_on(const rtems_irq_connect_data *unused)
{
  (void) unused;
}

/**
 * Disables clock interrupts
 *
 * If the interrupt is always on, this can be a NOP.
 */
static void clock_isr_off(const rtems_irq_connect_data *unused)
{
  (void) unused;
}

/**
 * Tests to see if clock interrupt is enabled, and returns 1 if so.
 * If interrupt is not enabled, returns 0.
 *
 * If the interrupt is always on, this always returns 1.
 */
static int clock_isr_is_on(const rtems_irq_connect_data *irq)
{
  (void) irq;

  return 1;
}

#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

/* Make sure to include this, and only at the end of the file */
#include "../../../shared/dev/clock/clockimpl.h"
