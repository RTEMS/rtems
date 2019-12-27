/**
 * @file
 *
 * @ingroup arm_start
 *
 * @brief Arm CP15 start.
 */


/*
 * Copyright (c) 2013 Hesham AL-Matary.
 * Copyright (c) 2009-2019 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_SHARED_ARM_CP15_START_H
#define LIBBSP_ARM_SHARED_ARM_CP15_START_H

#include <libcpu/arm-cp15.h>
#include <bsp/start.h>
#include <bsp/linker-symbols.h>
#include <bspopts.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  uint32_t begin;
  uint32_t end;
  uint32_t flags;
} arm_cp15_start_section_config;

#define ARMV7_CP15_START_DEFAULT_SECTIONS \
  { \
    .begin = (uint32_t) bsp_section_fast_text_begin, \
    .end = (uint32_t) bsp_section_fast_text_end, \
    .flags = ARMV7_MMU_CODE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_fast_data_begin, \
    .end = (uint32_t) bsp_section_fast_data_end, \
    .flags = ARMV7_MMU_DATA_READ_WRITE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_start_begin, \
    .end = (uint32_t) bsp_section_start_end, \
    .flags = ARMV7_MMU_CODE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_vector_begin, \
    .end = (uint32_t) bsp_section_vector_end, \
    .flags = ARMV7_MMU_DATA_READ_WRITE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_text_begin, \
    .end = (uint32_t) bsp_section_text_end, \
    .flags = ARMV7_MMU_CODE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_rodata_begin, \
    .end = (uint32_t) bsp_section_rodata_end, \
    .flags = ARMV7_MMU_DATA_READ_ONLY_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_data_begin, \
    .end = (uint32_t) bsp_section_data_end, \
    .flags = ARMV7_MMU_DATA_READ_WRITE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_bss_begin, \
    .end = (uint32_t) bsp_section_bss_end, \
    .flags = ARMV7_MMU_DATA_READ_WRITE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_work_begin, \
    .end = (uint32_t) bsp_section_work_end, \
    .flags = ARMV7_MMU_DATA_READ_WRITE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_stack_begin, \
    .end = (uint32_t) bsp_section_stack_end, \
    .flags = ARMV7_MMU_DATA_READ_WRITE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_nocache_begin, \
    .end = (uint32_t) bsp_section_nocache_end, \
    .flags = ARMV7_MMU_DEVICE \
  }, { \
    .begin = (uint32_t) bsp_section_nocachenoload_begin, \
    .end = (uint32_t) bsp_section_nocachenoload_end, \
    .flags = ARMV7_MMU_DEVICE \
  }, { \
    .begin = (uint32_t) bsp_translation_table_base, \
    .end = (uint32_t) bsp_translation_table_end, \
    .flags = ARMV7_MMU_DATA_READ_WRITE_CACHED \
  }

#define ARMV7_CP15_START_WORKSPACE_ENTRY_INDEX 8

BSP_START_DATA_SECTION extern const arm_cp15_start_section_config
  arm_cp15_start_mmu_config_table[];

BSP_START_DATA_SECTION extern const size_t
  arm_cp15_start_mmu_config_table_size;

BSP_START_TEXT_SECTION static inline void
arm_cp15_start_set_translation_table_entries(
  uint32_t *ttb,
  const arm_cp15_start_section_config *config
)
{
  if (config->begin != config->end) {
    uint32_t i;
    uint32_t iend;
    uint32_t index_mask;
    uint32_t flags;
#ifdef ARM_MMU_USE_SMALL_PAGES
    uint32_t *pt;

    pt = &ttb[ARM_MMU_TRANSLATION_TABLE_ENTRY_COUNT];
    i = ARM_MMU_SMALL_PAGE_GET_INDEX(config->begin);
    iend = ARM_MMU_SMALL_PAGE_GET_INDEX(ARM_MMU_SECT_MVA_ALIGN_UP(config->end));
    index_mask = (1U << (32 - ARM_MMU_SMALL_PAGE_BASE_SHIFT)) - 1U;
    flags = ARM_MMU_SECT_FLAGS_TO_SMALL_PAGE(config->flags);

    while (i != iend) {
      pt[i] = (i << ARM_MMU_SMALL_PAGE_BASE_SHIFT) | flags;
      i = (i + 1U) & index_mask;
    }
#else
    i = ARM_MMU_SECT_GET_INDEX(config->begin);
    iend = ARM_MMU_SECT_GET_INDEX(ARM_MMU_SECT_MVA_ALIGN_UP(config->end));
    index_mask = (1U << (32 - ARM_MMU_SECT_BASE_SHIFT)) - 1U;
    flags = config->flags;

    while (i != iend) {
      ttb[i] = (i << ARM_MMU_SECT_BASE_SHIFT) | flags;
      i = (i + 1U) & index_mask;
    }
#endif
  }
}

BSP_START_TEXT_SECTION static inline void
arm_cp15_start_setup_translation_table(
  uint32_t *ttb,
  uint32_t client_domain,
  const arm_cp15_start_section_config *config_table,
  size_t config_count
)
{
#ifdef ARM_MMU_USE_SMALL_PAGES
  uint32_t *pt;
#endif
  uint32_t dac;
  size_t i;

  dac = ARM_CP15_DAC_DOMAIN(client_domain, ARM_CP15_DAC_CLIENT);
  arm_cp15_set_domain_access_control(dac);
  arm_cp15_set_translation_table_base(ttb);

  /* Initialize translation table with invalid entries */
#ifdef ARM_MMU_USE_SMALL_PAGES
  pt = &ttb[ARM_MMU_TRANSLATION_TABLE_ENTRY_COUNT];
  for (i = 0; i < ARM_MMU_TRANSLATION_TABLE_ENTRY_COUNT; ++i) {
    size_t j;

    for (j = 0; j < ARM_MMU_SMALL_PAGE_TABLE_ENTRY_COUNT; ++j) {
      pt[j] = 0;
    }

    ttb[i] = (uint32_t) pt | (client_domain << ARM_MMU_SECT_DOMAIN_SHIFT)
      | ARM_MMU_PAGE_TABLE_DEFAULT;
    pt += ARM_MMU_SMALL_PAGE_TABLE_ENTRY_COUNT;
  }
#else
  for (i = 0; i < ARM_MMU_TRANSLATION_TABLE_ENTRY_COUNT; ++i) {
    ttb[i] = 0;
  }
#endif

  for (i = 0; i < config_count; ++i) {
    arm_cp15_start_set_translation_table_entries(ttb, &config_table [i]);
  }
}

BSP_START_TEXT_SECTION static inline void
arm_cp15_start_setup_translation_table_and_enable_mmu_and_cache(
  uint32_t ctrl,
  uint32_t *ttb,
  uint32_t client_domain,
  const arm_cp15_start_section_config *config_table,
  size_t config_count
)
{
  arm_cp15_start_setup_translation_table(
    ttb,
    client_domain,
    config_table,
    config_count
  );

  /* Enable MMU and cache */
  ctrl |= ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_M;

  arm_cp15_set_control(ctrl);
}

BSP_START_TEXT_SECTION static inline uint32_t
arm_cp15_start_setup_mmu_and_cache(uint32_t ctrl_clear, uint32_t ctrl_set)
{
  uint32_t ctrl = arm_cp15_get_control();

  ctrl &= ~ctrl_clear;
  ctrl |= ctrl_set;

  arm_cp15_set_control(ctrl);

  arm_cp15_tlb_invalidate();

  return ctrl;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_ARM_CP15_START_H */
