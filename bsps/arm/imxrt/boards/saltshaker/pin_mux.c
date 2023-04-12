/*
 * Copyright 2018 NXP.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include <bsp.h>
#include <bsp/start.h>

BSP_START_TEXT_SECTION
void BOARD_InitDEBUG_UARTPins(void) {
  CLOCK_EnableClock(kCLOCK_Iomuxc);

  IOMUXC_SetPinMux(IOMUXC_GPIO_AD_02_LPUART8_TXD, 0U);
  IOMUXC_SetPinMux(IOMUXC_GPIO_AD_03_LPUART8_RXD, 0U);
  IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_02_LPUART8_TXD, 0x10B0u);
  IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_03_LPUART8_RXD, 0x10B0u);
}
