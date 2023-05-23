/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMShared
 *
 * @brief This source file contains the implementation of
 *   arm_cp15_set_translation_table_entries().
 */

/*
 * Copyright (C) 2010, 2019 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems.h>
#include <libcpu/arm-cp15.h>
#include <bspopts.h>

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
  uint32_t *ttb;
#ifdef ARM_MMU_USE_SMALL_PAGES
  uint32_t *pt;
  uint32_t flags;
#endif
  uint32_t istart;
  uint32_t iend;
  uint32_t index_mask;
  uint32_t section_flags_of_first_entry;
  uint32_t i;
  uint32_t *modified_begin;
  size_t modified_size;

  ttb = arm_cp15_get_translation_table_base();
#ifdef ARM_MMU_USE_SMALL_PAGES
  pt = &ttb[ARM_MMU_TRANSLATION_TABLE_ENTRY_COUNT];
  istart = ARM_MMU_SMALL_PAGE_GET_INDEX(begin);
  iend = ARM_MMU_SMALL_PAGE_GET_INDEX(ARM_MMU_SMALL_PAGE_MVA_ALIGN_UP(end));
  index_mask = (1U << (32 - ARM_MMU_SMALL_PAGE_BASE_SHIFT)) - 1U;
  section_flags_of_first_entry = ARM_MMU_SMALL_PAGE_FLAGS_TO_SECT(pt[istart])
    | ARM_MMU_PAGE_TABLE_FLAGS_TO_SECT(ttb[ARM_MMU_SECT_GET_INDEX(begin)]);
  modified_begin = &pt[istart];
  flags = ARM_MMU_SECT_FLAGS_TO_SMALL_PAGE(section_flags);
#else
  istart = ARM_MMU_SECT_GET_INDEX(begin);
  iend = ARM_MMU_SECT_GET_INDEX(ARM_MMU_SECT_MVA_ALIGN_UP(end));
  index_mask = (1U << (32 - ARM_MMU_SECT_BASE_SHIFT)) - 1U;
  section_flags_of_first_entry = ttb[istart];
  modified_begin = &ttb[istart];
#endif
  modified_size = 0;

  for ( i = istart; i != iend; i = (i + 1U) & index_mask ) {
    uint32_t pa;

#ifdef ARM_MMU_USE_SMALL_PAGES
    pa = i << ARM_MMU_SMALL_PAGE_BASE_SHIFT;
    pt[i] = pa | flags;
    modified_size += ARM_MMU_SMALL_PAGE_TABLE_ENTRY_SIZE;
#else
    pa = i << ARM_MMU_SECT_BASE_SHIFT;
    ttb[i] = pa | section_flags;
    modified_size += ARM_MMU_TRANSLATION_TABLE_ENTRY_SIZE;
#endif
  }

  if ((arm_cp15_get_control() & (ARM_CP15_CTRL_C | ARM_CP15_CTRL_M)) != 0) {
    rtems_cache_flush_multiple_data_lines(modified_begin, modified_size);
  }

  _ARM_Data_synchronization_barrier();

  for ( i = istart; i != iend; i = (i + 1U) & index_mask ) {
    void *mva;

#ifdef ARM_MMU_USE_SMALL_PAGES
    mva = (void *) (i << ARM_MMU_SMALL_PAGE_BASE_SHIFT);
#else
    mva = (void *) (i << ARM_MMU_SECT_BASE_SHIFT);
#endif

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
      arm_cp15_tlb_invalidate_entry_all_asids_inner_shareable(mva);
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
