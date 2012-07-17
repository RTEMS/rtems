/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Watchdog initialization code.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
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

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/mpc55xx-config.h>

BSP_START_TEXT_SECTION void mpc55xx_start_watchdog(void)
{
  #ifdef MPC55XX_NEEDS_LOW_LEVEL_INIT
    #ifdef MPC55XX_HAS_SWT
      /* Write keys to clear soft lock bit */
      SWT.SR.R = 0x0000c520;
      SWT.SR.R = 0x0000d928;

      /* Clear watchdog enable (WEN) */
      SWT.CR.R = 0x8000010A;
    #endif
  #endif
}
