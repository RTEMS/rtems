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
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <libcpu/arm-cp15.h>

/*
 * Translation table modification requires to propagate
 * information to memory and other cores.
 *
 * Algorithm follows example found in the section
 *
 * B3.10.1 General TLB maintenance requirements
 * TLB maintenance operations and the memory order model
 *
 * of ARM Architecture Reference Manual
 * ARMv7-A and ARMv7-R edition
 * ARM DDI 0406C.b (ID072512)
 */

static uint32_t set_translation_table_entries(
  const void *begin,
  const void *end,
  uint32_t section_flags
)
{
  uint32_t *ttb = arm_cp15_get_translation_table_base();
  uint32_t istart = ARM_MMU_SECT_GET_INDEX(begin);
  uint32_t iend = ARM_MMU_SECT_GET_INDEX(ARM_MMU_SECT_MVA_ALIGN_UP(end));
  uint32_t index_mask = (1U << (32 - ARM_MMU_SECT_BASE_SHIFT)) - 1U;
  uint32_t ctrl;
  uint32_t section_flags_of_first_entry;
  uint32_t i;
  void *first_ttb_addr;
  void *last_ttb_end;

  ctrl = arm_cp15_get_control();
  section_flags_of_first_entry = ttb [istart];
  last_ttb_end = first_ttb_addr = ttb + istart;

  for ( i = istart; i != iend; i = (i + 1U) & index_mask ) {
    uint32_t addr = i << ARM_MMU_SECT_BASE_SHIFT;

    ttb [i] = addr | section_flags;
    last_ttb_end = ttb + i + 1;
  }

  if ( ctrl & (ARM_CP15_CTRL_C | ARM_CP15_CTRL_M ) ) {
    rtems_cache_flush_multiple_data_lines(first_ttb_addr,
                last_ttb_end - first_ttb_addr);
  }

  _ARM_Data_synchronization_barrier();

  for ( i = istart; i != iend; i = (i + 1U) & index_mask ) {
    void *mva = (void *) (i << ARM_MMU_SECT_BASE_SHIFT);
#if defined(__ARM_ARCH_7A__)
    /*
     * Bit 31 needs to be 1 to indicate the register implements the
     * Multiprocessing Extensions register format and the U (bit 30)
     * is 0.
     */
    #define MPIDR_MX_FMT (1 << 31)
    #define MPIDR_UP     (1 << 30)
    const uint32_t mpidr = arm_cp15_get_multiprocessor_affinity();
    if ((mpidr & (MPIDR_MX_FMT | MPIDR_UP)) == MPIDR_MX_FMT) {
      arm_cp15_tlb_invalidate_entry_all_asids(mva);
    }
    else
#endif
    {
      arm_cp15_tlb_instruction_invalidate_entry(mva);
      arm_cp15_tlb_data_invalidate_entry(mva);
    }
  }

  _ARM_Data_synchronization_barrier();
  _ARM_Instruction_synchronization_barrier();

  return section_flags_of_first_entry;
}

uint32_t arm_cp15_set_translation_table_entries(
  const void *begin,
  const void *end,
  uint32_t section_flags
)
{
  return set_translation_table_entries(begin, end, section_flags);
}
