/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

#include <assert.h>

#include <rtems.h>

#include <bsp/lpc32xx.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#define MUST_WAIT_FOR_INTERRUPT 1

static void Install_tm27_vector(void (*handler)(rtems_vector_number))
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  LPC32XX_SW_INT = 0;

  sc = rtems_interrupt_handler_install(
    LPC32XX_IRQ_SW,
    "SW",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) handler,
    NULL
  );
  assert(sc == RTEMS_SUCCESSFUL);
}

static void Cause_tm27_intr(void)
{
  LPC32XX_SW_INT = 0x1;
}

static void Clear_tm27_intr(void)
{
  LPC32XX_SW_INT = 0;
  lpc32xx_irq_set_priority(LPC32XX_IRQ_SW, LPC32XX_IRQ_PRIORITY_LOWEST);
}

static void Lower_tm27_intr(void)
{
  bsp_interrupt_vector_enable(LPC32XX_IRQ_SW);
  lpc32xx_irq_set_priority(LPC32XX_IRQ_SW, LPC32XX_IRQ_PRIORITY_HIGHEST);
}

#endif /* __tm27_h */
