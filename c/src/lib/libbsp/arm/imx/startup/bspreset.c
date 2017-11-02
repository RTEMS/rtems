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

#include <arm/freescale/imx/imx_wdogreg.h>

void bsp_reset(void)
{
  volatile uint16_t *pcr;

  pcr = (volatile uint16_t *) 0x30280000;
  *pcr &= ~WDOG_CR_SRS;
  *pcr &= ~WDOG_CR_SRS;
  *pcr &= ~WDOG_CR_SRS;

  while (true) {
    /* Wait */
  }
}
