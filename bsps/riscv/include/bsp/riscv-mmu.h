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

#ifndef LIBBSP_RISCV_SHARED_RISCV_MMU_H
#define LIBBSP_RISCV_SHARED_RISCV_MMU_H

#include <bsp/fatal.h>
#include <bsp/utility.h>
#include <bspopts.h>

#include <rtems/score/riscv-utility.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* TODO: put somewhere more general */
#define BSP_START_TEXT_SECTION __attribute__((section(".bsp_start_text")))
#define BSP_START_DATA_SECTION __attribute__((section(".bsp_start_data")))

/**
 * @brief This enumeration maps the virtual address bit lengths to the
 * mnemonic names.
 */
typedef enum {
  SV32 = 32,
  SV39 = 39,
  SV48 = 48,
  SV57 = 57
} riscv_virtual_address_size;

/**
 * @brief This structure holds the state to maintain the MMU page tables.
 */
typedef struct {
  /**
   * @brief This member points to the root page table.
   */
  uintptr_t *root;
  /**
   * @brief This member stores the virtual address size.
   */
  riscv_virtual_address_size va_bits;
} riscv_mmu_control;

/**
 * @brief This object is used to maintain the MMU page tables.
 */
extern riscv_mmu_control riscv_mmu_instance;

/**
 * @brief Sets up the MMU translation table.
 *
 * The memory regions of the configuration table are mapped by the MMU. If a
 * mapping is infeasible, then the BSP fatal error
 * ::RISCV_FATAL_MMU_CANNOT_MAP_BLOCK will be issued.
 *
 * @param[in, out] control is a reference to the MMU control state.
 */
void riscv_mmu_setup_translation_table(
  riscv_mmu_control *control
);

void riscv_mmu_setup( void );

BSP_START_TEXT_SECTION static inline void
riscv_mmu_enable( const riscv_mmu_control *control )
{
  /* Enable MMU */
  unsigned long value;

#define SATP_SET_VA_SIZE_CASE( _va_bits )                          \
    case _va_bits:                                                 \
      value = (uintptr_t) SATP_MODE_##_va_bits << SATP_MODE_SHIFT; \
      break

  switch ( control->va_bits ) {
#if __riscv_xlen == 64
    SATP_SET_VA_SIZE_CASE( SV39 );
#else
    SATP_SET_VA_SIZE_CASE( SV32 );
#endif
    default:
      bsp_fatal( RISCV_FATAL_MMU_CANNOT_MAP_BLOCK );
  }

  value |= (uintptr_t) control->root >> RISCV_PGSHIFT;
  write_csr( satp, value );

  _RISCV_TLB_flush_all_entries( );
}

BSP_START_TEXT_SECTION static inline void
riscv_mmu_disable( void )
{
  /* Disable MMU */
  write_csr( satp, 0 );
  _RISCV_TLB_flush_all_entries( );
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_RISCV_SHARED_RISCV_MMU_H */
