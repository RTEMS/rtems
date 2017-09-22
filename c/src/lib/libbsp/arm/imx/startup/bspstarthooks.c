/*
 * Copyright (c) 2013, 2017 embedded brains GmbH.  All rights reserved.
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
imx_mmu_config_table[] = {
  ARMV7_CP15_START_DEFAULT_SECTIONS,
  {
    .begin = 0x07000000U,
    .end = 0x70000000U,
    .flags = ARMV7_MMU_DEVICE
  }
};

BSP_START_TEXT_SECTION static void setup_mmu_and_cache(void)
{
  uint32_t ctrl = arm_cp15_start_setup_mmu_and_cache(
    ARM_CP15_CTRL_A,
    ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_Z
  );

  arm_cp15_start_setup_translation_table_and_enable_mmu_and_cache(
    ctrl,
    (uint32_t *) bsp_translation_table_base,
    ARM_MMU_DEFAULT_CLIENT_DOMAIN,
    &imx_mmu_config_table[0],
    RTEMS_ARRAY_SIZE(imx_mmu_config_table)
  );
}

BSP_START_TEXT_SECTION void bsp_start_hook_0(void)
{
#ifdef RTEMS_SMP
  uint32_t cpu_id = arm_cortex_a9_get_multiprocessor_cpu_id();

  arm_a9mpcore_start_enable_smp_in_auxiliary_control();

  if (cpu_id != 0) {
    arm_a9mpcore_start_on_secondary_processor();
  }
#endif
}

BSP_START_TEXT_SECTION void bsp_start_hook_1(void)
{
  arm_a9mpcore_start_set_vector_base();
  bsp_start_copy_sections();
  setup_mmu_and_cache();
  bsp_start_clear_bss();
}
