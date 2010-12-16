/**
 * @file
 *
 * @ingroup lpc32xx
 *
 * @brief Reset code.
 */

/*
 * Copyright (c) 2009-2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <stdbool.h>

#include <bspopts.h>
#include <bsp/bootcard.h>
#include <bsp/lpc32xx.h>

static void watchdog_reset(void)
{
  #ifdef LPC32XX_ENABLE_WATCHDOG_RESET
    LPC32XX_TIMCLK_CTRL |= TIMCLK_CTRL_WDT;
    lpc32xx.wdt.mctrl |= WDTTIM_MCTRL_M_RES1 | WDTTIM_MCTRL_M_RES2;
    lpc32xx.wdt.emr = WDTTIM_EMR_MATCH_CTRL_SET(lpc32xx.wdt.emr, 0x2);
    lpc32xx.wdt.ctrl |= WDTTIM_CTRL_COUNT_ENAB;
    lpc32xx.wdt.match0 = 1;
    lpc32xx.wdt.counter = 0;
  #endif
}

void bsp_reset( void)
{
  watchdog_reset();

  while (true) {
    /* Do nothing */
  }
}
