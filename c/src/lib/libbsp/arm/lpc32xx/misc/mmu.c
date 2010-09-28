/**
 * @file
 *
 * @ingroup lpc32xx_mmu
 *
 * @brief MMU support implementation.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
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

void lpc32xx_set_translation_table_entries(
  void *begin,
  void *end,
  uint32_t section_flags
)
{
  uint32_t *ttb = arm_cp15_get_translation_table_base();
  uint32_t i = ARM_MMU_SECT_GET_INDEX(begin);
  uint32_t iend = ARM_MMU_SECT_GET_INDEX(ARM_MMU_SECT_MVA_ALIGN_UP(end));

  while (i < iend) {
    ttb [i] = (i << ARM_MMU_SECT_BASE_SHIFT) | section_flags;
    ++i;
  }
}
