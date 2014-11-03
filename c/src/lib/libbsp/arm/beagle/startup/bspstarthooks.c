/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/arm-cp15-start.h>
#include <bsp/arm-a8core-start.h>

#include <bsp/uart-output-char.h>

BSP_START_TEXT_SECTION void bsp_start_hook_0(void)
{
}

BSP_START_TEXT_SECTION void bsp_start_hook_1(void)
{
  arm_a8core_start_hook_1();
  bsp_start_copy_sections();
  beagle_setup_mmu_and_cache();
  bsp_start_clear_bss();
}
