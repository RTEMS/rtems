/**
 * @file
 *
 * @ingroup lpc32xx_mmu
 *
 * @brief MMU support implementation.
 */

/*
 * Copyright (c) 2010-2011 embedded brains GmbH.  All rights reserved.
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

#include <bsp/mmu.h>

static uint32_t disable_mmu(void)
{
  uint32_t ctrl = 0;

  arm_cp15_data_cache_test_and_clean_and_invalidate();

  ctrl = arm_cp15_get_control();
  arm_cp15_set_control(ctrl & ~ARM_CP15_CTRL_M);

  arm_cp15_tlb_invalidate();

  return ctrl;
}

static void restore_mmu_control(uint32_t ctrl)
{
  arm_cp15_set_control(ctrl);
}

uint32_t set_translation_table_entries(
  const void *begin,
  const void *end,
  uint32_t section_flags
)
{
  uint32_t *ttb = arm_cp15_get_translation_table_base();
  uint32_t i = ARM_MMU_SECT_GET_INDEX(begin);
  uint32_t iend = ARM_MMU_SECT_GET_INDEX(ARM_MMU_SECT_MVA_ALIGN_UP(end));
  uint32_t ctrl = disable_mmu();
  uint32_t section_flags_of_first_entry = ttb [i];

  while (i < iend) {
    ttb [i] = (i << ARM_MMU_SECT_BASE_SHIFT) | section_flags;
    ++i;
  }

  restore_mmu_control(ctrl);

  return section_flags_of_first_entry;
}

uint32_t lpc32xx_set_translation_table_entries(
  const void *begin,
  const void *end,
  uint32_t section_flags
)
{
  rtems_interrupt_level level;
  uint32_t section_flags_of_first_entry = 0;

  rtems_interrupt_disable(level);
  section_flags_of_first_entry =
    set_translation_table_entries(begin, end, section_flags);
  rtems_interrupt_enable(level);

  return section_flags_of_first_entry;
}
