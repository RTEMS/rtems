/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsRISCV
 *
 * @brief RISC-V MMU configuration.
 */

/*
 * Copyright (C) 2026 Gedare Bloom
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

#include <bsp/riscv-mmu.h>

#include <bsp/fatal.h>

#include <rtems/score/riscv-utility.h>
#include <rtems/score/assert.h>

BSP_START_DATA_SECTION RTEMS_ALIGNED( RISCV_PGSIZE ) static uintptr_t
    riscv_root_page_table[1 << RISCV_PGLEVEL_BITS];

#if __riscv_xlen == 64
BSP_START_DATA_SECTION RTEMS_ALIGNED( RISCV_PGSIZE ) static uintptr_t
    riscv_l1_ram_page_table[1 << RISCV_PGLEVEL_BITS];

BSP_START_DATA_SECTION RTEMS_ALIGNED( RISCV_PGSIZE ) static uintptr_t
    riscv_l1_mmio_page_table[1 << RISCV_PGLEVEL_BITS];
#endif

BSP_START_DATA_SECTION riscv_mmu_control riscv_mmu_instance = {
  .root = (uintptr_t *) riscv_root_page_table,
#if __riscv_xlen == 64
  .va_bits = SV39
#else
  .va_bits = SV32
#endif
};

BSP_START_TEXT_SECTION static inline uintptr_t riscv_create_pte(
  riscv_mmu_control *control,
  uintptr_t address,
  uint32_t flags
)
{
  uintptr_t ppn0, ppn1;
#if __riscv_xlen == 64
  uintptr_t ppn2;
#endif
  uintptr_t pte = 0;

  uintptr_t page = address >> RISCV_PGSHIFT;
  const uint32_t mask = (1 << RISCV_PGLEVEL_BITS) - 1;

  ppn0 = page & mask;
  ppn0 <<= PTE_PPN_SHIFT;

  ppn1 = (page >> RISCV_PGLEVEL_BITS);
#if __riscv_xlen == 64
  ppn1 &= mask;
#else
  if ( control->va_bits != SV32 ) {
      bsp_fatal( RISCV_FATAL_MMU_CANNOT_MAP_BLOCK );
  }
#endif
  ppn1 <<= PTE_PPN_SHIFT + RISCV_PGLEVEL_BITS;

#if __riscv_xlen == 64
  ppn2 = (page >> 2 * RISCV_PGLEVEL_BITS);
  ppn2 <<= PTE_PPN_SHIFT + 2 * RISCV_PGLEVEL_BITS;
  if ( control->va_bits != SV39 ) {
      bsp_fatal( RISCV_FATAL_MMU_CANNOT_MAP_BLOCK );
  }
  pte |= ppn2;
#endif

  pte |= ppn1 | ppn0 | flags;

  return pte;
}

BSP_START_TEXT_SECTION static inline uint32_t riscv_mmu_get_index(
  uintptr_t address,
  int level
)
{
  uint32_t shift_bits;
  const uint32_t mask = (1 << RISCV_PGLEVEL_BITS) - 1;

  if ( level == 0 || level > 2 ) {
      bsp_fatal( RISCV_FATAL_MMU_CANNOT_MAP_BLOCK );
  }

  shift_bits = RISCV_PGSHIFT + level * RISCV_PGLEVEL_BITS;

  return (address >> shift_bits) & mask;
}

BSP_START_TEXT_SECTION static inline void riscv_map_megapages(
  riscv_mmu_control *control,
  uintptr_t *page_table,
  uintptr_t address,
  size_t size,
  uint32_t flags,
  int level,
  size_t megapage_size
) {
  size_t i;

  uintptr_t index;

  for ( i = 0; i < size / megapage_size; i++ )
  {
    index = riscv_mmu_get_index( address, level );
    page_table[index] = riscv_create_pte( control, address, flags );
    address += (uintptr_t) megapage_size;
  }
}

BSP_START_TEXT_SECTION static inline void riscv_mmu_add_subtable(
  riscv_mmu_control *control,
  uintptr_t *subtable,
  uintptr_t address,
  size_t size,
  uint32_t flags
)
{
  uint32_t nonleaf_flags = PTE_V | PTE_G;
  size_t two_MiB = 2 * 1024 * 1024;
  uintptr_t index = riscv_mmu_get_index( address, 2 );

  control->root[index] = riscv_create_pte(
      control,
      (uintptr_t) subtable,
      nonleaf_flags
  );

  /* TODO: check that size fits in the root index PTE */
  riscv_map_megapages( control, subtable, address, size, flags, 1, two_MiB );
}

BSP_START_TEXT_SECTION void riscv_mmu_setup_translation_table(
  riscv_mmu_control *control
)
{
  uint32_t ram_flags = PTE_V | PTE_R | PTE_W | PTE_X | PTE_G | PTE_A | PTE_D;
  uint32_t mmio_flags = PTE_V | PTE_R | PTE_W | PTE_G | PTE_A | PTE_D;
  const size_t two_MiB = 2 * 1024 * 1024;

#if __riscv_xlen == 64
  riscv_mmu_add_subtable(
    control,
    riscv_l1_ram_page_table,
    (uintptr_t) RISCV_RAM_REGION_BEGIN,
    (size_t) RISCV_RAM_REGION_SIZE,
    ram_flags
  );

  riscv_mmu_add_subtable(
    control,
    riscv_l1_mmio_page_table,
    0x10000000, /* TODO: get this from FDT? */
    two_MiB,
    mmio_flags
  );
#else
  riscv_map_megapages(
    control,
    control->root,
    (uintptr_t) RISCV_RAM_REGION_BEGIN,
    (size_t) RISCV_RAM_REGION_SIZE,
    ram_flags,
    1,
    2 * two_MiB
  );

  riscv_map_megapages(
    control,
    control->root,
    0x10000000, /* TODO: get this from FDT? */
    2 * two_MiB,
    mmio_flags,
    1,
    2 * two_MiB
  );
#endif
}

BSP_START_TEXT_SECTION void riscv_mmu_setup( void )
{
  riscv_mmu_setup_translation_table( &riscv_mmu_instance );
  _RISCV_data_barrier();
  riscv_mmu_enable( &riscv_mmu_instance );
}
