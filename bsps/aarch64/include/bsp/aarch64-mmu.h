/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup aarch64_start
 *
 * @brief AArch64 MMU configuration.
 */

/*
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

#ifndef LIBBSP_AARCH64_SHARED_AARCH64_MMU_H
#define LIBBSP_AARCH64_SHARED_AARCH64_MMU_H

#include <bsp/fatal.h>
#include <bsp/linker-symbols.h>
#include <bsp/start.h>
#include <bsp/utility.h>
#include <bspopts.h>
#include <libcpu/mmu-vmsav8-64.h>
#include <rtems/score/aarch64-system-registers.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  uintptr_t begin;
  uintptr_t end;
  uint64_t flags;
} aarch64_mmu_config_entry;

#define AARCH64_MMU_DEFAULT_SECTIONS \
  { \
    .begin = (uintptr_t) bsp_section_fast_text_begin, \
    .end = (uintptr_t) bsp_section_fast_text_end, \
    .flags = AARCH64_MMU_CODE_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_fast_data_begin, \
    .end = (uintptr_t) bsp_section_fast_data_end, \
    .flags = AARCH64_MMU_DATA_RW_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_start_begin, \
    .end = (uintptr_t) bsp_section_start_end, \
    .flags = AARCH64_MMU_CODE_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_vector_begin, \
    .end = (uintptr_t) bsp_section_vector_end, \
    .flags = AARCH64_MMU_DATA_RW_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_text_begin, \
    .end = (uintptr_t) bsp_section_text_end, \
    .flags = AARCH64_MMU_CODE_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_rodata_begin, \
    .end = (uintptr_t) bsp_section_rodata_end, \
    .flags = AARCH64_MMU_DATA_RO_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_data_begin, \
    .end = (uintptr_t) bsp_section_data_end, \
    .flags = AARCH64_MMU_DATA_RW_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_bss_begin, \
    .end = (uintptr_t) bsp_section_bss_end, \
    .flags = AARCH64_MMU_DATA_RW_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_rtemsstack_begin, \
    .end = (uintptr_t) bsp_section_rtemsstack_end, \
    .flags = AARCH64_MMU_DATA_RW_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_noinit_begin, \
    .end = (uintptr_t) bsp_section_noinit_end, \
    .flags = AARCH64_MMU_DATA_RW_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_work_begin, \
    .end = (uintptr_t) bsp_section_work_end, \
    .flags = AARCH64_MMU_DATA_RW_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_stack_begin, \
    .end = (uintptr_t) bsp_section_stack_end, \
    .flags = AARCH64_MMU_DATA_RW_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_nocache_begin, \
    .end = (uintptr_t) bsp_section_nocache_end, \
    .flags = AARCH64_MMU_DEVICE \
  }, { \
    .begin = (uintptr_t) bsp_section_nocachenoload_begin, \
    .end = (uintptr_t) bsp_section_nocachenoload_end, \
    .flags = AARCH64_MMU_DEVICE \
  }, { \
    .begin = (uintptr_t) bsp_translation_table_base, \
    .end = (uintptr_t) bsp_translation_table_end, \
    .flags = AARCH64_MMU_DATA_RW_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_start_vector_table_begin, \
    .end = (uintptr_t) bsp_start_vector_table_end, \
    .flags = AARCH64_MMU_CODE_CACHED \
  }

/**
 * @brief This is the AArch64 MMU configuration table.
 *
 * The default table is provided by the BSP.  Applications may provide their
 * own.
 */
extern const aarch64_mmu_config_entry aarch64_mmu_config_table[];

/**
 * @brief This is the count of entries in the AArch64 MMU configuration table.
 *
 * The default table is provided by the BSP.  Applications may provide their
 * own.
 */
extern const size_t aarch64_mmu_config_table_size;

/**
 * @brief This structure represents the state to maintain the MMU translation
 *   tables.
 */
typedef struct {
  /**
   * @brief This member references the translation table base.
   */
  uint64_t *ttb;

  /**
   * @brief This member contains the count of used page tables.
   *
   * A maximum of ::AARCH64_MMU_TRANSLATION_TABLE_PAGES can be used.
   */
  size_t used_page_tables;
} aarch64_mmu_control;

/**
 * @brief This object is used to maintain the MMU translation tables.
 */
extern aarch64_mmu_control aarch64_mmu_instance;

/**
 * @brief Sets the MMU translation table entries associated with the memory
 *   region.
 *
 * @param[in, out] control is a reference to the MMU control state.
 *
 * @param[in] config is the configuration entry with the memory region and
 *   region attributes.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The begin address of the memory region
 *   cannot be mapped by the MMU.
 *
 * @retval ::RTEMS_INVALID_SIZE The end address of the memory region cannot be
 *   mapped by the MMU.
 *
 * @retval ::RTEMS_TOO_MANY There was no page table entry available to perform
 *   the mapping.
 */
rtems_status_code aarch64_mmu_set_translation_table_entries(
  aarch64_mmu_control *control,
  const aarch64_mmu_config_entry *config
);

/**
 * @brief Sets up the MMU translation table.
 *
 * The memory regions of the configuration table are mapped by the MMU.  If a
 * mapping is infeasible, then the BSP fatal error
 * ::AARCH64_FATAL_MMU_CANNOT_MAP_BLOCK will be issued.
 *
 * @param[in, out] control is a reference to the MMU control state.
 *
 * @param[in] config_table is the configuration table with memory regions and
 *   region attributes.
 *
 * @param config_count is the count of configuration table entries.
 */
void aarch64_mmu_setup_translation_table(
  aarch64_mmu_control *control,
  const aarch64_mmu_config_entry *config_table,
  size_t config_count
);

BSP_START_TEXT_SECTION static inline void
aarch64_mmu_enable( const aarch64_mmu_control *control )
{
  uint64_t sctlr;

  /* CPUECTLR_EL1.SMPEN is already set on ZynqMP and is not writable */

  /* Flush and invalidate cache */
  rtems_cache_flush_entire_data();

  _AArch64_Write_ttbr0_el1( (uintptr_t) control->ttb );
  _AARCH64_Instruction_synchronization_barrier();

  /* Enable MMU and cache */
  sctlr = _AArch64_Read_sctlr_el1();
  sctlr |= AARCH64_SCTLR_EL1_I | AARCH64_SCTLR_EL1_C | AARCH64_SCTLR_EL1_M;
  _AArch64_Write_sctlr_el1( sctlr );
}

BSP_START_TEXT_SECTION static inline void
aarch64_mmu_disable( void )
{
  uint64_t sctlr;

  /*
   * Flush data cache before disabling the MMU. While the MMU is disabled, all
   * accesses are treated as uncached device memory.
   */
  rtems_cache_flush_entire_data();

  /* Disable MMU */
  sctlr = _AArch64_Read_sctlr_el1();
  sctlr &= ~(AARCH64_SCTLR_EL1_M);
  _AArch64_Write_sctlr_el1( sctlr );
}

BSP_START_TEXT_SECTION static inline void aarch64_mmu_setup( void )
{
  /* Set TCR */
  /* 256TB/48 bits mappable (64-0x10) */
  _AArch64_Write_tcr_el1(
    AARCH64_TCR_EL1_T0SZ( 0x10 ) | AARCH64_TCR_EL1_IRGN0( 0x1 ) |
    AARCH64_TCR_EL1_ORGN0( 0x1 ) | AARCH64_TCR_EL1_SH0( 0x3 ) |
    AARCH64_TCR_EL1_TG0( 0x0 ) | AARCH64_TCR_EL1_IPS( 0x5ULL ) |
    AARCH64_TCR_EL1_EPD1
  );

  /* Set MAIR */
  _AArch64_Write_mair_el1(
    AARCH64_MAIR_EL1_ATTR0( 0x0 ) | AARCH64_MAIR_EL1_ATTR1( 0x4 ) |
    AARCH64_MAIR_EL1_ATTR2( 0x44 ) | AARCH64_MAIR_EL1_ATTR3( 0xFF )
  );
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_AARCH64_SHARED_AARCH64_MMU_H */
