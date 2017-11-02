/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/bootcard.h>
#include <bsp.h>
#include <arm/freescale/imx/imx_wdogreg.h>

void bsp_reset(void)
{
  volatile uint16_t *pcr;

  imx_uart_console_drain();

  pcr = (volatile uint16_t *) 0x30280000;
  *pcr = WDOG_CR_WDE;
  *pcr = WDOG_CR_WDE;
  *pcr = WDOG_CR_WDE;

  while (true) {
    /* Wait */
  }
}
