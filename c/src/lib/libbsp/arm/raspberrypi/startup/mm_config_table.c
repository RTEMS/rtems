/**
 * @file
 *
 * @ingroup arm_start
 *
 * @brief Raspberry Pi low level start
 */

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
#include <bsp/arm-cp15-start.h>

/*
 * Pagetable initialization data
 *
 * Keep all read-only sections before read-write ones.
 * This ensures that write is allowed if one page/region
 * is partially filled by read-only section contentent
 * and rest is used for writeable section
 */

const arm_cp15_start_section_config arm_cp15_start_mmu_config_table[] = {
  {
    .begin = (uint32_t) bsp_section_fast_text_begin,
    .end = (uint32_t) bsp_section_fast_text_end,
    .flags = ARMV7_MMU_CODE_CACHED
  }, {
    .begin = (uint32_t) bsp_section_start_begin,
    .end = (uint32_t) bsp_section_start_end,
    .flags = ARMV7_MMU_CODE_CACHED
  }, {
    .begin = (uint32_t) bsp_section_text_begin,
    .end = (uint32_t) bsp_section_text_end,
    .flags = ARMV7_MMU_CODE_CACHED
  }, {
    .begin = (uint32_t) bsp_section_rodata_begin,
    .end = (uint32_t) bsp_section_rodata_end,
    .flags = ARMV7_MMU_DATA_READ_ONLY_CACHED
  }, {
    .begin = (uint32_t) bsp_translation_table_base,
    .end = (uint32_t) bsp_translation_table_base + 0x4000,
    .flags = ARMV7_MMU_DATA_READ_WRITE_CACHED
  }, {
    .begin = (uint32_t) bsp_section_fast_data_begin,
    .end = (uint32_t) bsp_section_fast_data_end,
    .flags = ARMV7_MMU_DATA_READ_WRITE_CACHED
  }, {
    .begin = (uint32_t) bsp_section_vector_begin,
    .end = (uint32_t) bsp_section_vector_end,
    .flags = ARMV7_MMU_DATA_READ_WRITE_CACHED
  }, {
    .begin = (uint32_t) bsp_section_data_begin,
    .end = (uint32_t) bsp_section_data_end,
    .flags = ARMV7_MMU_DATA_READ_WRITE_CACHED
  }, {
    .begin = (uint32_t) bsp_section_bss_begin,
    .end = (uint32_t) bsp_section_bss_end,
    .flags = ARMV7_MMU_DATA_READ_WRITE_CACHED
  }, {
    .begin = (uint32_t) bsp_section_work_begin,
    .end = (uint32_t) bsp_section_work_end,
    .flags = ARMV7_MMU_DATA_READ_WRITE_CACHED
  }, {
    .begin = (uint32_t) bsp_section_stack_begin,
    .end = (uint32_t) bsp_section_stack_end,
    .flags = ARMV7_MMU_DATA_READ_WRITE_CACHED
  }, {
    .begin = RPI_PERIPHERAL_BASE,
    .end =   RPI_PERIPHERAL_BASE + RPI_PERIPHERAL_SIZE,
    .flags = ARMV7_MMU_DEVICE
  }
#if (BSP_IS_RPI2 == 1)
  /* Core local peripherals area - timer, mailboxes */
  , {
    .begin = BCM2836_CORE_LOCAL_PERIPH_BASE,
    .end =   BCM2836_CORE_LOCAL_PERIPH_BASE + BCM2836_CORE_LOCAL_PERIPH_SIZE,
    .flags = ARMV7_MMU_DEVICE
  }
#endif
};

const size_t arm_cp15_start_mmu_config_table_size =
  RTEMS_ARRAY_SIZE(arm_cp15_start_mmu_config_table);
