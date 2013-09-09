/**
 * @file
 *
 * @ingroup raspberrypi
 *
 * @brief Startup code.
 */

/*
 * Copyright (c) 2013 by Alan Cudmore
 * based on work by:
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE
 */

#include <bspopts.h>
#include <bsp/start.h>
#include <bsp/raspberrypi.h>
#include <bsp/mmu.h>

static void BSP_START_TEXT_SECTION raspberrypi_cache_setup(void)
{
  uint32_t ctrl = 0;

  /* Disable MMU and cache, basic settings */
  ctrl = arm_cp15_get_control();
  ctrl &= ~(ARM_CP15_CTRL_I | ARM_CP15_CTRL_R | ARM_CP15_CTRL_C
    | ARM_CP15_CTRL_V | ARM_CP15_CTRL_M);
  ctrl |= ARM_CP15_CTRL_S;
  arm_cp15_set_control(ctrl);

  arm_cp15_cache_invalidate();
  arm_cp15_tlb_invalidate();

}


void BSP_START_TEXT_SECTION bsp_start_hook_0(void)
{
  raspberrypi_cache_setup();
}


void BSP_START_TEXT_SECTION bsp_start_hook_1(void)
{
  bsp_start_copy_sections();
  bsp_start_clear_bss();
}
