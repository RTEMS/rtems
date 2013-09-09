/*
 * Copyright (c) 2010-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <libcpu/arm-cp15.h>

static uint32_t set_translation_table_entries(
  const void *begin,
  const void *end,
  uint32_t section_flags
)
{
  uint32_t cl_size = arm_cp15_get_min_cache_line_size();
  uint32_t *ttb = arm_cp15_get_translation_table_base();
  uint32_t i = ARM_MMU_SECT_GET_INDEX(begin);
  uint32_t iend = ARM_MMU_SECT_GET_INDEX(ARM_MMU_SECT_MVA_ALIGN_UP(end));
  uint32_t index_mask = (1U << (32 - ARM_MMU_SECT_BASE_SHIFT)) - 1U;
  uint32_t ctrl;
  uint32_t section_flags_of_first_entry;

  ctrl = arm_cp15_mmu_disable(cl_size);
  arm_cp15_tlb_invalidate();
  section_flags_of_first_entry = ttb [i];

  while (i != iend) {
    uint32_t addr = i << ARM_MMU_SECT_BASE_SHIFT;

    ttb [i] = addr | section_flags;

    i = (i + 1U) & index_mask;
  }

  arm_cp15_set_control(ctrl);

  return section_flags_of_first_entry;
}

uint32_t arm_cp15_set_translation_table_entries(
  const void *begin,
  const void *end,
  uint32_t section_flags
)
{
  rtems_interrupt_level level;
  uint32_t section_flags_of_first_entry;

  rtems_interrupt_disable(level);
  section_flags_of_first_entry =
    set_translation_table_entries(begin, end, section_flags);
  rtems_interrupt_enable(level);

  return section_flags_of_first_entry;
}
