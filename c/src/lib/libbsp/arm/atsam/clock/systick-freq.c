/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>

#include <chip.h>

uint32_t atsam_systick_frequency(void)
{
  uint32_t mdiv = (PMC->PMC_MCKR & PMC_MCKR_MDIV_Msk) >> PMC_MCKR_MDIV_Pos;
  uint32_t fclk;

  if (mdiv == 3) {
    fclk = BOARD_MCK * mdiv;
  } else {
    fclk = BOARD_MCK * (1 << mdiv);
  }

  return fclk;
}
