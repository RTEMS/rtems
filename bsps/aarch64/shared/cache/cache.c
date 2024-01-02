/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64Shared
 *
 * @brief AArch64 cache defines and implementation.
 */

/*
 * Copyright (C) 2020 On-Line Applications Research Corporation (OAR)
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

#include <rtems.h>
#include <bsp.h>
#include <rtems/score/aarch64-system-registers.h>

#define CPU_DATA_CACHE_ALIGNMENT 64

#define CPU_INSTRUCTION_CACHE_ALIGNMENT 64

#define CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS

#define CPU_CACHE_SUPPORT_PROVIDES_CACHE_SIZE_FUNCTIONS

#define AARCH64_CACHE_L1_CPU_DATA_ALIGNMENT ( (size_t) 64 )
#define AARCH64_CACHE_PREPARE_MVA(mva) (const void *) \
  RTEMS_ALIGN_DOWN ( (size_t) mva, AARCH64_CACHE_L1_CPU_DATA_ALIGNMENT )

static inline
void AArch64_data_cache_clean_and_invalidate_line(const void *d_addr)
{
  d_addr = AARCH64_CACHE_PREPARE_MVA(d_addr);

  __asm__ volatile (
    "dc civac, %[d_addr]"
    :
    : [d_addr] "r" (d_addr)
    : "memory"
  );
}

static inline void
_CPU_cache_flush_data_range(
  const void *d_addr,
  size_t      n_bytes
)
{
  _AARCH64_Data_synchronization_barrier();
  if ( n_bytes != 0 ) {
    size_t adx = (size_t) AARCH64_CACHE_PREPARE_MVA ( d_addr );
    const size_t ADDR_LAST = (size_t) d_addr + n_bytes - 1;

    for (; adx <= ADDR_LAST; adx += AARCH64_CACHE_L1_CPU_DATA_ALIGNMENT ) {
      /* Store and invalidate the Data cache line */
      AArch64_data_cache_clean_and_invalidate_line( (void*)adx );
    }
    /* Wait for L1 store to complete */
    _AARCH64_Data_synchronization_barrier();
  }
  _AARCH64_Data_synchronization_barrier();
}

static inline void AArch64_data_cache_invalidate_line(const void *d_addr)
{
  d_addr = AARCH64_CACHE_PREPARE_MVA(d_addr);

  __asm__ volatile (
    "dc ivac, %[d_addr]"
    :
    : [d_addr] "r" (d_addr)
    : "memory"
  );
}

static inline void
_CPU_cache_invalidate_data_range(
  const void *d_addr,
  size_t     n_bytes
)
{
  if ( n_bytes != 0 ) {
    size_t adx = (size_t) AARCH64_CACHE_PREPARE_MVA ( d_addr );
    const size_t end = (size_t)d_addr + n_bytes -1;

    /* Back starting address up to start of a line and invalidate until end */
    for (;
         adx <= end;
         adx += AARCH64_CACHE_L1_CPU_DATA_ALIGNMENT ) {
        /* Invalidate the Instruction cache line */
        AArch64_data_cache_invalidate_line( (void*)adx );
    }
    /* Wait for L1 invalidate to complete */
    _AARCH64_Data_synchronization_barrier();
  }
}

static inline void _CPU_cache_freeze_data(void)
{
  /* TODO */
}

static inline void _CPU_cache_unfreeze_data(void)
{
  /* TODO */
}

static inline void AArch64_instruction_cache_invalidate_line(const void *i_addr)
{
  /* __builtin___clear_cache is explicitly only for instruction cacche */
  __builtin___clear_cache((void *)i_addr, ((char *)i_addr) + sizeof(void*) - 1);
}

static inline void
_CPU_cache_invalidate_instruction_range( const void *i_addr, size_t n_bytes)
{
  if ( n_bytes != 0 ) {
    __builtin___clear_cache((void *)i_addr, ((char *)i_addr) + n_bytes - 1);
  }
  _AARCH64_Instruction_synchronization_barrier();
}

static inline void _CPU_cache_freeze_instruction(void)
{
  /* TODO */
}

static inline void _CPU_cache_unfreeze_instruction(void)
{
  /* TODO */
}

static inline uint64_t AArch64_get_ccsidr_for_level(
  uint64_t level, bool instruction
)
{
  uint64_t csselr = AARCH64_CSSELR_EL1_LEVEL(level - 1);

  csselr |= instruction ? AARCH64_CSSELR_EL1_IND : 0;

  _AArch64_Write_csselr_el1(csselr);
  return _AArch64_Read_ccsidr_el1();
}

static inline uint64_t
AArch64_ccsidr_get_line_power(uint64_t ccsidr)
{
  return AARCH64_CCSIDR_EL1_LINESIZE_GET(ccsidr) + 4;
}

static inline uint64_t
AArch64_ccsidr_get_associativity(uint64_t ccsidr)
{
  return AARCH64_CCSIDR_EL1_ASSOCIATIVITY_GET_0(ccsidr) + 1;
}

static inline uint64_t
AArch64_ccsidr_get_num_sets(uint64_t ccsidr)
{
  return AARCH64_CCSIDR_EL1_NUMSETS_GET_0(ccsidr) + 1;
}


static inline void AArch64_data_cache_clean_level(uint64_t level)
{
  uint64_t ccsidr;
  uint64_t line_power;
  uint64_t associativity;
  uint64_t way;
  uint64_t way_shift;

  ccsidr = AArch64_get_ccsidr_for_level(level, false);

  line_power = AArch64_ccsidr_get_line_power(ccsidr);
  associativity = AArch64_ccsidr_get_associativity(ccsidr);
  way_shift = __builtin_clz(associativity - 1);

  for (way = 0; way < associativity; ++way) {
    uint64_t num_sets = AArch64_ccsidr_get_num_sets(ccsidr);
    uint64_t set;

    for (set = 0; set < num_sets; ++set) {
      uint64_t set_and_way = (way << way_shift)
        | (set << line_power)
        | ((level - 1) << 1);

      __asm__ volatile (
        "dc csw, %[set_and_way]"
        :
        : [set_and_way] "r" (set_and_way)
        : "memory"
      );
    }
  }
}

static inline
uint64_t AArch64_clidr_get_cache_type(uint64_t clidr, uint64_t level)
{
  switch (level)
  {
  case 1:
    return AARCH64_CLIDR_EL1_CTYPE1_GET(clidr);
  case 2:
    return AARCH64_CLIDR_EL1_CTYPE2_GET(clidr);
  case 3:
    return AARCH64_CLIDR_EL1_CTYPE3_GET(clidr);
  case 4:
    return AARCH64_CLIDR_EL1_CTYPE4_GET(clidr);
  case 5:
    return AARCH64_CLIDR_EL1_CTYPE5_GET(clidr);
  case 6:
    return AARCH64_CLIDR_EL1_CTYPE6_GET(clidr);
  case 7:
    return AARCH64_CLIDR_EL1_CTYPE7_GET(clidr);
  default:
    return 0;
  }
}

static inline uint64_t AArch64_clidr_get_level_of_coherency(uint64_t clidr)
{
  return AARCH64_CLIDR_EL1_LOC_GET(clidr);
}

static inline void AArch64_data_cache_clean_all_levels(void)
{
  uint64_t clidr = _AArch64_Read_clidr_el1();
  uint64_t loc = AArch64_clidr_get_level_of_coherency(clidr);
  uint64_t level = 0;

  for (level = 1; level <= loc; ++level) {
    uint64_t ctype = AArch64_clidr_get_cache_type(clidr, level);

    /* Check if this level has a data cache or unified cache */
    if (((ctype & (0x6)) == 2) || (ctype == 4)) {
      AArch64_data_cache_clean_level(level);
    }
  }
}

static inline void _CPU_cache_flush_entire_data(void)
{
  _AARCH64_Data_synchronization_barrier();
  AArch64_data_cache_clean_all_levels();
  _AARCH64_Data_synchronization_barrier();
}

static inline void AArch64_cache_invalidate_level(uint64_t level)
{
  uint64_t ccsidr;
  uint64_t line_power;
  uint64_t associativity;
  uint64_t way;
  uint64_t way_shift;

  ccsidr = AArch64_get_ccsidr_for_level(level, false);

  line_power = AArch64_ccsidr_get_line_power(ccsidr);
  associativity = AArch64_ccsidr_get_associativity(ccsidr);
  way_shift = __builtin_clz(associativity - 1);

  for (way = 0; way < associativity; ++way) {
    uint64_t num_sets = AArch64_ccsidr_get_num_sets(ccsidr);
    uint64_t set;

    for (set = 0; set < num_sets; ++set) {
      uint64_t set_and_way = (way << way_shift)
        | (set << line_power)
        | ((level - 1) << 1);

      __asm__ volatile (
        "dc isw, %[set_and_way]"
        :
        : [set_and_way] "r" (set_and_way)
        : "memory"
      );
    }
  }
}

static inline void AArch64_data_cache_invalidate_all_levels(void)
{
  uint64_t clidr = _AArch64_Read_clidr_el1();
  uint64_t loc = AArch64_clidr_get_level_of_coherency(clidr);
  uint64_t level = 0;

  for (level = 1; level <= loc; ++level) {
    uint64_t ctype = AArch64_clidr_get_cache_type(clidr, level);

    /* Check if this level has a data cache or unified cache */
    if ((ctype & 0x2) || (ctype == 4)) {
      AArch64_cache_invalidate_level(level);
    }
  }
}

static inline void _CPU_cache_invalidate_entire_data(void)
{
  AArch64_data_cache_invalidate_all_levels();
}

static inline void _CPU_cache_enable_data(void)
{
  rtems_interrupt_level level;
  uint64_t sctlr;

  rtems_interrupt_local_disable(level);
  sctlr = _AArch64_Read_sctlr_el1();
  sctlr |= AARCH64_SCTLR_EL1_C;
  _AArch64_Write_sctlr_el1(sctlr);
  rtems_interrupt_local_enable(level);
}

static inline void _CPU_cache_disable_data(void)
{
  rtems_interrupt_level level;
  uint64_t sctlr;

  rtems_interrupt_local_disable(level);
  AArch64_data_cache_clean_all_levels();
  AArch64_data_cache_invalidate_all_levels();
  sctlr = _AArch64_Read_sctlr_el1();
  sctlr &= ~AARCH64_SCTLR_EL1_C;
  _AArch64_Write_sctlr_el1(sctlr);
  rtems_interrupt_local_enable(level);
}

#ifdef RTEMS_SMP
static inline
void AArch64_instruction_cache_inner_shareable_invalidate_all(void)
{
  __asm__ volatile (
    "ic ialluis\n"
    :
    :
    : "memory"
  );
}
#endif /* RTEMS_SMP */

static inline void AArch64_instruction_cache_invalidate(void)
{
  __asm__ volatile (
    "ic iallu\n"
    :
    :
    : "memory"
  );
}

static inline void _CPU_cache_invalidate_entire_instruction(void)
{
  /* There is no way to manage branch prediction in AArch64.
   * See D4.4.12 in the ARMv8 technical manual. */

  #ifdef RTEMS_SMP
  /* invalidate I-cache inner shareable */
  AArch64_instruction_cache_inner_shareable_invalidate_all();
  #endif /* RTEMS_SMP */

  /* I+BTB cache invalidate */
  AArch64_instruction_cache_invalidate();

  _AARCH64_Instruction_synchronization_barrier();
}

static inline void _CPU_cache_enable_instruction(void)
{
  rtems_interrupt_level level;
  uint64_t sctlr;

  rtems_interrupt_local_disable(level);
  sctlr = _AArch64_Read_sctlr_el1();
  sctlr |= AARCH64_SCTLR_EL1_I;
  _AArch64_Write_sctlr_el1(sctlr);
  rtems_interrupt_local_enable(level);
}

static inline void _CPU_cache_disable_instruction(void)
{
  rtems_interrupt_level level;
  uint64_t sctlr;

  rtems_interrupt_local_disable(level);
  sctlr = _AArch64_Read_sctlr_el1();
  sctlr &= ~AARCH64_SCTLR_EL1_I;
  _AArch64_Write_sctlr_el1(sctlr);
  rtems_interrupt_local_enable(level);
}

static inline size_t AArch64_get_cache_size(
  uint64_t level,
  bool instruction
)
{
  uint64_t clidr;
  uint64_t loc;
  uint64_t ccsidr;

  clidr = _AArch64_Read_clidr_el1();
  loc = AArch64_clidr_get_level_of_coherency(clidr);

  if (level > loc) {
    return 0;
  }

  ccsidr = AArch64_get_ccsidr_for_level(level, instruction);

  return (1U << (AArch64_ccsidr_get_line_power(ccsidr)+4))
    * AArch64_ccsidr_get_associativity(ccsidr)
    * AArch64_ccsidr_get_num_sets(ccsidr);
}

static inline size_t _CPU_cache_get_data_cache_size(uint64_t level)
{
  return AArch64_get_cache_size(level, false);
}

static inline size_t _CPU_cache_get_instruction_cache_size(uint64_t level)
{
  return AArch64_get_cache_size(level, true);
}

#include "../../shared/cache/cacheimpl.h"
