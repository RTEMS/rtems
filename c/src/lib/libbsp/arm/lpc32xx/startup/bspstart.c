/**
 * @file
 *
 * @ingroup lpc32xx
 *
 * @brief Startup code.
 */

/*
 * Copyright (c) 2009, 2010
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <bsp/irq.h>
#include <bsp/linker-symbols.h>
#include <bsp/stackalloc.h>
#include <bsp/lpc32xx.h>

static void lpc32xx_timer_initialize(void)
{
  volatile lpc_timer *timer = LPC32XX_STANDARD_TIMER;

  LPC32XX_TIMCLK_CTRL1 = (1U << 2) | (1U << 3);

  timer->tcr = LPC_TIMER_TCR_RST;
  timer->ctcr = 0x0;
  timer->pr = 0x0;
  timer->ir = 0xff;
  timer->mcr = 0x0;
  timer->ccr = 0x0;
  timer->tcr = LPC_TIMER_TCR_EN;
}

void bsp_start(void)
{
  if (bsp_interrupt_initialize() != RTEMS_SUCCESSFUL) {
    _CPU_Fatal_halt(0xe);
  }

  bsp_stack_initialize(
    bsp_section_stack_begin,
    (uintptr_t) bsp_section_stack_size
  );

  lpc32xx_timer_initialize();
}
