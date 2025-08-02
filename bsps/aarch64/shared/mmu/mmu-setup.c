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

#include <bsp/aarch64-mmu.h>

#include <bsp/fatal.h>
#include <bsp/linker-symbols.h>
#include <bsp/start.h>
#include <rtems/score/aarch64-system-registers.h>
#include <rtems/score/assert.h>

aarch64_mmu_control aarch64_mmu_instance = {
  .ttb = (uint64_t *) bsp_translation_table_base,

  /* One page table is used for the initial setup at the base */
  .used_page_tables = 1
};

/* AArch64 uses levels 0, 1, 2, and 3 */
#define MMU_MAX_SUBTABLE_PAGE_BITS ( 3 * MMU_BITS_PER_LEVEL + MMU_PAGE_BITS )

/* setup straight mapped block entries */
BSP_START_TEXT_SECTION static inline void aarch64_mmu_page_table_set_blocks(
  uint64_t *page_table,
  uint64_t base,
  uint32_t bits_offset,
  uint64_t default_attr
)
{
  uint64_t page_flag = 0;

  if ( bits_offset == MMU_PAGE_BITS ) {
    page_flag = MMU_DESC_TYPE_PAGE;
  }

  for ( uint64_t i = 0; i < ( 1 << MMU_BITS_PER_LEVEL ); i++ ) {
    page_table[i] = base | ( i << bits_offset );
    page_table[i] |= default_attr | page_flag;
  }
}

BSP_START_TEXT_SECTION static inline uint64_t *
aarch64_mmu_page_table_alloc( aarch64_mmu_control *control )
{
  size_t used_page_tables = control->used_page_tables;

  if ( used_page_tables >= AARCH64_MMU_TRANSLATION_TABLE_PAGES ) {
    return NULL;
  }

  control->used_page_tables = used_page_tables + 1;
  return (uint64_t *)
    ( (uintptr_t) control->ttb + ( used_page_tables << MMU_PAGE_BITS ) );
}

BSP_START_TEXT_SECTION static inline uintptr_t aarch64_mmu_get_index(
  uintptr_t root_address,
  uintptr_t vaddr,
  uint32_t shift
)
{
  uintptr_t mask = ( 1 << ( MMU_BITS_PER_LEVEL + 1 ) ) - 1;

  return ( ( vaddr - root_address ) >> shift ) & mask;
}

BSP_START_TEXT_SECTION static uint64_t *
aarch64_mmu_get_sub_table(
  aarch64_mmu_control *control,
  uint64_t *page_table_entry,
  uintptr_t physical_root_address,
  uint32_t shift
)
{
  /* check if the index already has a page table */
  if ( ( *page_table_entry & MMU_DESC_TYPE_TABLE ) == MMU_DESC_TYPE_TABLE ) {
    /* extract page table address */
    uint64_t table_pointer = *page_table_entry & MMU_DESC_PAGE_TABLE_MASK;
    /* This cast should be safe since the address was inserted in this mode */
    return (uint64_t *) (uintptr_t) table_pointer;
  }

  /* allocate new page table and set block */
  uint64_t *sub_table = aarch64_mmu_page_table_alloc( control );

  if ( sub_table == NULL ) {
    return NULL;
  }

  aarch64_mmu_page_table_set_blocks(
    sub_table,
    physical_root_address,
    shift - MMU_BITS_PER_LEVEL,
    *page_table_entry & ~MMU_DESC_PAGE_TABLE_MASK
  );
  *page_table_entry = (uintptr_t) sub_table;
  *page_table_entry |= MMU_DESC_TYPE_TABLE | MMU_DESC_VALID;

  return sub_table;
}

BSP_START_TEXT_SECTION static inline rtems_status_code aarch64_mmu_map_block(
  aarch64_mmu_control *control,
  uint64_t *page_table,
  uint64_t root_address,
  uint64_t addr,
  uint64_t size,
  int8_t level,
  uint64_t flags
)
{
  uint32_t shift = ( 2 - level ) * MMU_BITS_PER_LEVEL + MMU_PAGE_BITS;
  uint64_t granularity = 1LLU << shift;

  do {
    uintptr_t index = aarch64_mmu_get_index( root_address, addr, shift );
    uint64_t block_bottom = RTEMS_ALIGN_DOWN( addr, granularity );
    uint64_t chunk_size = granularity;

    /* check for perfect block match */
    if ( block_bottom == addr ) {
      if ( size >= chunk_size ) {
        /* level -1 can't contain block descriptors, fall through to subtable */
        if ( level != -1 ) {
          uint64_t page_flag = 0;

          if ( level == 2 ) {
            page_flag = MMU_DESC_TYPE_PAGE;
          }

          /* when page_flag is set the last level must be a page descriptor */
          if ( page_flag || ( page_table[index] & MMU_DESC_TYPE_TABLE ) != MMU_DESC_TYPE_TABLE ) {
            /* no sub-table, apply block properties */
            page_table[index] = addr | flags | page_flag;
            size -= chunk_size;
            addr += chunk_size;
            continue;
          }
        }
      } else {
        /*
         * Block starts on a boundary, but is short.
         *
         * The size is >= MMU_PAGE_SIZE since
         * aarch64_mmu_set_translation_table_entries() aligns the memory region
         * to page boundaries.  The minimum chunk_size is MMU_PAGE_SIZE.
         */
        _Assert( level < 2 );
        chunk_size = size;
      }
    } else {
      uintptr_t block_top = RTEMS_ALIGN_UP( addr, granularity );
      chunk_size = block_top - addr;

      if ( chunk_size > size ) {
        chunk_size = size;
      }
    }

    /* Deal with any subtable modification  */
    uint64_t new_root_address = root_address + index * granularity;
    rtems_status_code sc;

    uint64_t *sub_table = aarch64_mmu_get_sub_table(
      control,
      &page_table[index],
      new_root_address,
      shift
    );

    if ( sub_table == NULL ) {
      return RTEMS_TOO_MANY;
    }

    sc = aarch64_mmu_map_block(
      control,
      sub_table,
      new_root_address,
      addr,
      chunk_size,
      level + 1,
      flags
    );

    if ( sc != RTEMS_SUCCESSFUL ) {
      return sc;
    }

    size -= chunk_size;
    addr += chunk_size;
  } while ( size > 0 );

  return RTEMS_SUCCESSFUL;
}
/* Get the maximum number of bits supported by this hardware */
BSP_START_TEXT_SECTION static inline uint64_t
aarch64_mmu_get_cpu_pa_bits( void )
{
#ifdef AARCH64_MMU_PHYSICAL_ADDRESS_RANGE_BITS
  return AARCH64_MMU_PHYSICAL_ADDRESS_RANGE_BITS;
#else
  uint64_t id_reg = _AArch64_Read_id_aa64mmfr0_el1();

  switch ( AARCH64_ID_AA64MMFR0_EL1_PARANGE_GET( id_reg ) ) {
  case 0:
	  return 32;
  case 1:
	  return 36;
  case 2:
	  return 40;
  case 3:
	  return 42;
  case 4:
	  return 44;
  case 5:
	  return 48;
  case 6:
	  return 52;
  default:
	  return 48;
  }
  return 48;
#endif
}

BSP_START_TEXT_SECTION rtems_status_code
aarch64_mmu_set_translation_table_entries(
  aarch64_mmu_control *control,
  const aarch64_mmu_config_entry *config
)
{
  uint64_t max_mappable = 1LLU << aarch64_mmu_get_cpu_pa_bits();
  /* Align to page boundaries */
  uint64_t begin = RTEMS_ALIGN_DOWN( config->begin, MMU_PAGE_SIZE );
  uint64_t end = RTEMS_ALIGN_UP( (uint64_t) config->end, MMU_PAGE_SIZE );
  uint64_t size = end - begin;

  if ( config->begin == config->end ) {
    return RTEMS_SUCCESSFUL;
  }

  if ( begin >= max_mappable ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( size > max_mappable - begin ) {
    return RTEMS_INVALID_SIZE;
  }

  return aarch64_mmu_map_block(
    control,
    control->ttb,
    0x0,
    begin,
    size,
    -1,
    config->flags
  );
}

BSP_START_TEXT_SECTION void aarch64_mmu_setup_translation_table(
  aarch64_mmu_control *control,
  const aarch64_mmu_config_entry *config_table,
  size_t config_count
)
{
  size_t i;

  aarch64_mmu_page_table_set_blocks(
    control->ttb,
    (uintptr_t) NULL,
    MMU_MAX_SUBTABLE_PAGE_BITS,
    0
  );

  /* Configure entries required for each memory section */
  for ( i = 0; i < config_count; ++i ) {
    rtems_status_code sc;

    sc = aarch64_mmu_set_translation_table_entries( control, &config_table[i] );

    if ( sc != RTEMS_SUCCESSFUL ) {
      bsp_fatal( AARCH64_FATAL_MMU_CANNOT_MAP_BLOCK );
    }
  }
}
