/**
 * @file
 *
 * @ingroup lpc32xx
 *
 * @brief NAND controller selection.
 */

/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
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
#include <bsp/lpc32xx.h>
#include <bsp/nand-mlc.h>

void lpc32xx_select_nand_controller(lpc32xx_nand_controller nand_controller)
{
  uint32_t flashclk_ctrl = LPC32XX_FLASHCLK_CTRL & ~(FLASHCLK_IRQ_MLC
    | FLASHCLK_SELECT_SLC | FLASHCLK_MLC_CLK_ENABLE | FLASHCLK_SLC_CLK_ENABLE);

  switch (nand_controller) {
    case LPC32XX_NAND_CONTROLLER_MLC:
      flashclk_ctrl |= FLASHCLK_IRQ_MLC | FLASHCLK_MLC_CLK_ENABLE;
      break;
    case LPC32XX_NAND_CONTROLLER_SLC:
      flashclk_ctrl |= FLASHCLK_SELECT_SLC | FLASHCLK_SLC_CLK_ENABLE;
      break;
    default:
      break;
  }

  LPC32XX_FLASHCLK_CTRL = flashclk_ctrl;
}
