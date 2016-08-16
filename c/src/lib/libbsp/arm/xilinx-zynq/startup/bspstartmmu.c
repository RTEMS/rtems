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

#define ARM_CP15_TEXT_SECTION BSP_START_TEXT_SECTION

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/arm-cp15-start.h>
#include <bsp/arm-a9mpcore-start.h>

BSP_START_DATA_SECTION static const arm_cp15_start_section_config
zynq_mmu_config_table[] = {
  ARMV7_CP15_START_DEFAULT_SECTIONS,
#if defined(RTEMS_SMP)
  {
    .begin = 0xffff0000U,
    .end = 0xffffffffU,
    .flags = ARMV7_MMU_DEVICE
  },
#endif
  {
    .begin = 0xe0000000U,
    .end = 0xe0200000U,
    .flags = ARMV7_MMU_DEVICE
  }, {
    .begin = 0xf8000000U,
    .end = 0xf9000000U,
    .flags = ARMV7_MMU_DEVICE
  }
};

/*
 * Make weak and let the user override.
 */
BSP_START_TEXT_SECTION void zynq_setup_mmu_and_cache(void) __attribute__ ((weak));

BSP_START_TEXT_SECTION void zynq_setup_mmu_and_cache(void)
{
  uint32_t ctrl = arm_cp15_start_setup_mmu_and_cache(
    ARM_CP15_CTRL_A,
    ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_Z
  );

  arm_cp15_start_setup_translation_table_and_enable_mmu_and_cache(
    ctrl,
    (uint32_t *) bsp_translation_table_base,
    ARM_MMU_DEFAULT_CLIENT_DOMAIN,
    &zynq_mmu_config_table[0],
    RTEMS_ARRAY_SIZE(zynq_mmu_config_table)
  );
}
