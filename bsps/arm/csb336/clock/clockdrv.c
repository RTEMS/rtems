/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief MC9328MXL clock specific using the System Timer
 */

/*
 * Copyright (C) 2004 Cogent Computer Systems
 * Written by Jay Monkman <jtm@lopingdog.com>
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
#include <bsp.h>
#include <bsp/irq.h>
#include <mc9328mxl.h>
#include <rtems/bspIo.h>  /* for printk */

/* this is defined in ../../../shared/dev/clock/clockimpl.h */
void Clock_isr(rtems_irq_hdl_param arg);
static void clock_isr_on(const rtems_irq_connect_data *unused);
static void clock_isr_off(const rtems_irq_connect_data *unused);
static int clock_isr_is_on(const rtems_irq_connect_data *irq);

/* Replace the first value with the clock's interrupt name. */
rtems_irq_connect_data clock_isr_data = {
  .name   = BSP_INT_TIMER1,
  .hdl    = Clock_isr,
  .handle = (void *)BSP_INT_TIMER1,
  .on     = clock_isr_on,
  .off    = clock_isr_off,
  .isOn   = clock_isr_is_on,
};

/**
 * When we get the clock interrupt
 *    - clear the interrupt bit?
 *    - restart the timer?
 */
#define Clock_driver_support_at_tick(arg)            \
  do {                                               \
    uint32_t reg;                                    \
                                                     \
    reg = MC9328MXL_TMR1_TSTAT;                      \
    (void) reg; /* avoid set but not used warning */ \
    MC9328MXL_TMR1_TSTAT = 0;                        \
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
        int freq; \
        int cnt; \
        freq = get_perclk1_freq(); \
        printk("perclk1 freq is %d\n", freq); \
        cnt = ((long long)freq * rtems_configuration_get_microseconds_per_tick() + 500000) / 1000000;\
        printk("cnt freq is %d\n", cnt); \
        MC9328MXL_TMR1_TCMP = cnt; \
        /* use PERCLK1 as input, enable timer */ \
        MC9328MXL_TMR1_TCTL = (MC9328MXL_TMR_TCTL_CLKSRC_PCLK1 | \
                               MC9328MXL_TMR_TCTL_TEN | \
                               MC9328MXL_TMR_TCTL_IRQEN); \
        /* set prescaler to 1 (register value + 1) */ \
        MC9328MXL_TMR1_TPRER = 0; \
     } while (0)

/**
 * Enables clock interrupt.
 *
 * If the interrupt is always on, this can be a NOP.
 */
static void clock_isr_on(const rtems_irq_connect_data *unused)
{
  (void) unused;

  MC9328MXL_TMR1_TCTL |= MC9328MXL_TMR_TCTL_IRQEN;
  MC9328MXL_AITC_INTENNUM = MC9328MXL_INT_TIMER1;
}

/**
 * Disables clock interrupts
 *
 * If the interrupt is always on, this can be a NOP.
 */
static void clock_isr_off(const rtems_irq_connect_data *unused)
{
  (void) unused;

  MC9328MXL_TMR1_TCTL &= ~MC9328MXL_TMR_TCTL_IRQEN;
  MC9328MXL_AITC_INTDISNUM = MC9328MXL_INT_TIMER1;
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

  return MC9328MXL_TMR1_TCTL & MC9328MXL_TMR_TCTL_IRQEN;
}

#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

/* Make sure to include this, and only at the end of the file */

#include "../../../shared/dev/clock/clockimpl.h"
