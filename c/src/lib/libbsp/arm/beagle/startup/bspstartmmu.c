/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * Copyright (c) 2014 Chris Johns.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/arm-cp15-start.h>

#define ARM_SECTIONS       4096          /* all sections needed to describe the
                                            virtual address space */
#define ARM_SECTION_SIZE   (1024 * 1024) /* how much virtual memory is described
                                            by one section */

//static uint32_t pagetable[ARM_SECTIONS] __attribute__((aligned (1024*16)));

BSP_START_DATA_SECTION static const arm_cp15_start_section_config
beagle_mmu_config_table[] = {
  ARMV7_CP15_START_DEFAULT_SECTIONS,
  {
    .begin = 0x40000000U,
    .end = 0x4FFFFFFFU,
    .flags = ARMV7_MMU_DEVICE
  }
};

/*
 * Make weak and let the user override.
 */
BSP_START_TEXT_SECTION void beagle_setup_mmu_and_cache(void) __attribute__ ((weak));

BSP_START_TEXT_SECTION void beagle_setup_mmu_and_cache(void)
{
  /* turn mmu off first in case it's on */
  uint32_t ctrl = arm_cp15_start_setup_mmu_and_cache(
    ARM_CP15_CTRL_M | ARM_CP15_CTRL_A,	/* clear - mmu off */
    ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_Z
  );

  arm_cp15_start_setup_translation_table_and_enable_mmu_and_cache(
    ctrl,
    (uint32_t *) bsp_translation_table_base,
    ARM_MMU_DEFAULT_CLIENT_DOMAIN,
    &beagle_mmu_config_table[0],
    RTEMS_ARRAY_SIZE(beagle_mmu_config_table)
  );
}
