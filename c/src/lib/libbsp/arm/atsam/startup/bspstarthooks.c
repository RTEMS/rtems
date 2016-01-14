/*
 * Copyright (c) 2016 embedded brains GmbH Huber.  All rights reserved.
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
#include <bsp/start.h>
#include <bsp/pin-config.h>

#include <chip.h>
#include <include/board_lowlevel.h>
#include <include/board_memories.h>

void BSP_START_TEXT_SECTION bsp_start_hook_0(void)
{
  system_init_flash(BOARD_MCK);
  SystemInit();

  PIO_Configure(&atsam_pin_config[0], atsam_pin_config_count);
  MATRIX->CCFG_SYSIO = atsam_matrix_ccfg_sysio;

  if (!PMC_IsPeriphEnabled(ID_SDRAMC)) {
    BOARD_ConfigureSdram();
  }

  if ((SCB->CCR & SCB_CCR_IC_Msk) == 0) {
    SCB_EnableICache();
  }

  if ((SCB->CCR & SCB_CCR_DC_Msk) == 0) {
    SCB_EnableDCache();
  }

  _SetupMemoryRegion();
}

void BSP_START_TEXT_SECTION bsp_start_hook_1(void)
{
  bsp_start_copy_sections_compact();
  SCB_CleanDCache();
  SCB_InvalidateICache();
  bsp_start_clear_bss();
  WDT_Disable(WDT);
}
