/* @file
 *
 * @ingroup RTEMSBSPsARMLPC32XX
 *
 * @brief Implementations of interrupt mechanisms for Time Test 27
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

#include <rtems.h>

#include <bsp/lpc32xx.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#define MUST_WAIT_FOR_INTERRUPT 1

#define LPC32XX_TM27_TIMER (&lpc32xx.timer_2)

#define LPC32XX_TM27_IRQ LPC32XX_IRQ_TIMER_2

static inline void Install_tm27_vector(void (*handler)(rtems_vector_number))
{
  static rtems_interrupt_entry entry;
  volatile lpc_timer *timer = LPC32XX_TM27_TIMER;

  LPC32XX_TIMCLK_CTRL1 |= 1U << 4;

  timer->tcr = LPC_TIMER_TCR_RST;
  timer->ctcr = 0x0;
  timer->pr = 0x0;
  timer->ir = 0xff;
  timer->mcr = LPC_TIMER_MCR_MR0_INTR | LPC_TIMER_MCR_MR0_STOP |
    LPC_TIMER_MCR_MR0_RST;
  timer->ccr = 0x0;
  timer->emr = 0x0;
  timer->mr0 = 0x1;

  rtems_interrupt_entry_initialize(
    &entry,
    (rtems_interrupt_handler) handler,
    NULL,
    "tm27"
  );
  (void) rtems_interrupt_entry_install(
    LPC32XX_TM27_IRQ,
    RTEMS_INTERRUPT_SHARED,
    &entry
  );
}

static inline void Cause_tm27_intr(void)
{
  volatile lpc_timer *timer = LPC32XX_TM27_TIMER;

  timer->tcr = LPC_TIMER_TCR_EN;
}

static inline void Clear_tm27_intr(void)
{
  volatile lpc_timer *timer = LPC32XX_TM27_TIMER;

  timer->ir = LPC_TIMER_IR_MR0;
  lpc32xx_irq_set_priority(LPC32XX_TM27_IRQ, LPC32XX_IRQ_PRIORITY_LOWEST);
}

static inline void Lower_tm27_intr(void)
{
  bsp_interrupt_vector_enable(LPC32XX_TM27_IRQ);
  lpc32xx_irq_set_priority(LPC32XX_TM27_IRQ, LPC32XX_IRQ_PRIORITY_HIGHEST);
}

#endif /* __tm27_h */
