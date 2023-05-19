/* SPDX-License-Identifier: BSD-2-Clause */

/* @file
 *
 * @ingroup RTEMSBSPsARMLPC32XX
 *
 * @brief Implementations of interrupt mechanisms for Time Test 27
 */

/*
 * Copyright (c) 2010 embedded brains GmbH & Co. KG
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

static inline void Install_tm27_vector( rtems_interrupt_handler handler )
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
    handler,
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
