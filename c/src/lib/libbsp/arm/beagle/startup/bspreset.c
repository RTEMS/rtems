/**
 * @file
 *
 * @ingroup beagle
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
#include <bsp/beagle.h>

static void watchdog_reset(void)
{
  #ifdef BEAGLE_ENABLE_WATCHDOG_RESET
    //BEAGLE_TIMCLK_CTRL |= TIMCLK_CTRL_WDT;
    //beagle.wdt.mctrl |= WDTTIM_MCTRL_M_RES1 | WDTTIM_MCTRL_M_RES2;
    //beagle.wdt.emr = WDTTIM_EMR_MATCH_CTRL_SET(beagle.wdt.emr, 0x2);
    //beagle.wdt.ctrl |= WDTTIM_CTRL_COUNT_ENAB;
    //beagle.wdt.match0 = 1;
    //beagle.wdt.counter = 0;
  #endif
}

void bsp_reset( void)
{
  watchdog_reset();

  while (true) {
    /* Do nothing */
  }
}
