/*
 * Copyright (c) 2013, 2018 embedded brains GmbH.  All rights reserved.
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

#include <rtems/score/armv7m.h>

#include <bsp.h>
#include <imxrt/mpu-config.h>

#include <chip.h>
#include <fsl_pin_mux.h>
#include <fsl_clock_config.h>

BSP_START_TEXT_SECTION void bsp_start_hook_0(void)
{
  /* FIXME: Initializing SDRAM is currently done by DCD. It would be more user
   * friendly if that would be done here with a readable structure. */
  if ((SCB->CCR & SCB_CCR_IC_Msk) == 0) {
    SCB_EnableICache();
  }

  if ((SCB->CCR & SCB_CCR_DC_Msk) == 0) {
    SCB_EnableDCache();
  }

  _ARMV7M_MPU_Setup(imxrt_config_mpu_region, imxrt_config_mpu_region_count);
}

BSP_START_TEXT_SECTION void bsp_start_hook_1(void)
{
  bsp_start_copy_sections_compact();
  SCB_CleanDCache();
  SCB_InvalidateICache();
  bsp_start_clear_bss();

  BOARD_BootClockRUN();
  BOARD_InitDEBUG_UARTPins();

  /* Reduce frequency for I2C */
  CLOCK_SetDiv(kCLOCK_Lpi2cDiv, 5);

  /* Enable EDMA clock. We initialize the EDMA so we need the clock. */
  CLOCK_EnableClock(kCLOCK_Dma);
}
