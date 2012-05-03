/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief BSP reset.
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

#include <stdbool.h>

#include <bsp/bootcard.h>

#include <mpc55xx/regs.h>

void bsp_reset(void)
{
  while (true) {
    #if MPC55XX_CHIP_TYPE / 10 == 564
      /* TODO */
    #else
      SIU.SRCR.R = 1U << (31 - 0);
    #endif
  }
}
