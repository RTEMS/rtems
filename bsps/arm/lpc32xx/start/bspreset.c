/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC32XX
 *
 * @brief Reset code.
 */

/*
 * Copyright (C) 2009, 2010 embedded brains GmbH & Co. KG
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
