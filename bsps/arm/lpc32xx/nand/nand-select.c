/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC32XX
 *
 * @brief NAND controller selection.
 */

/*
 * Copyright (c) 2012 embedded brains GmbH & Co. KG
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
