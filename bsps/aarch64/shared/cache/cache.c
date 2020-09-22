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
#include <bsp/utility.h>

#define AARCH64_CACHE_L1_CPU_DATA_ALIGNMENT ((size_t)64)
#define AARCH64_CACHE_L1_DATA_LINE_MASK \
  ( AARCH64_CACHE_L1_CPU_DATA_ALIGNMENT - 1 )
#define AARCH64_CACHE_PREPARE_MVA(mva) \
  ((const void *) (((size_t) (mva)) & AARCH64_CACHE_L1_DATA_LINE_MASK))

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

static inline void _CPU_cache_flush_1_data_line(const void *d_addr)
{
  /* Flush the Data cache */
  AArch64_data_cache_clean_and_invalidate_line( d_addr );

  /* Wait for L1 flush to complete */
  _AARCH64_Data_synchronization_barrier();
}

static inline void
_CPU_cache_flush_data_range(
  const void *d_addr,
  size_t      n_bytes
)
{
  _AARCH64_Data_synchronization_barrier();
  if ( n_bytes != 0 ) {
    size_t adx = (size_t) d_addr & ~AARCH64_CACHE_L1_DATA_LINE_MASK;
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

static inline void _CPU_cache_invalidate_1_data_line(const void *d_addr)
{
  /* Invalidate the data cache line */
  AArch64_data_cache_invalidate_line( d_addr );

  /* Wait for L1 invalidate to complete */
  _AARCH64_Data_synchronization_barrier();
}

static inline void
_CPU_cache_invalidate_data_range(
  const void *d_addr,
  size_t     n_bytes
)
{
  if ( n_bytes != 0 ) {
    size_t adx = (size_t) d_addr
                         & ~AARCH64_CACHE_L1_DATA_LINE_MASK;
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

static inline void _CPU_cache_invalidate_1_instruction_line(const void *d_addr)
{
  /* Invalidate the Instruction cache line */
  AArch64_instruction_cache_invalidate_line( d_addr );

  /* Wait for L1 invalidate to complete */
  _AARCH64_Data_synchronization_barrier();
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

static inline uint64_t
AArch64_get_ccsidr(void)
{
  uint64_t val;

  __asm__ volatile (
    "mrs %[val], CCSIDR_EL1\n"
    : [val] "=&r" (val)
  );

  return val;
}

#define CCSIDR_NUMSETS(val) BSP_FLD64(val, 13, 27)
#define CCSIDR_NUMSETS_GET(reg) BSP_FLD64GET(reg, 13, 27)
#define CCSIDR_NUMSETS_SET(reg, val) BSP_FLD64SET(reg, val, 13, 27)
#define CCSIDR_ASSOCIATIVITY(val) BSP_FLD64(val, 3, 12)
#define CCSIDR_ASSOCIATIVITY_GET(reg) BSP_FLD64GET(reg, 3, 12)
#define CCSIDR_ASSOCIATIVITY_SET(reg, val) BSP_FLD64SET(reg, val, 3, 12)
/* line size == 1 << (GET(reg)+4): 0 -> (1 << 4) == 16 */
#define CCSIDR_LINE_SIZE(val) BSP_FLD64(val, 0, 2)
#define CCSIDR_LINE_SIZE_GET(reg) BSP_FLD64GET(reg, 0, 2)
#define CCSIDR_LINE_SIZE_SET(reg, val) BSP_FLD64SET(reg, val, 0, 2)

static inline uint64_t
AArch64_ccsidr_get_line_power(uint64_t ccsidr)
{
  return CCSIDR_LINE_SIZE_GET(ccsidr) + 4;
}

static inline uint64_t
AArch64_ccsidr_get_associativity(uint64_t ccsidr)
{
  return CCSIDR_ASSOCIATIVITY_GET(ccsidr) + 1;
}

static inline uint64_t
AArch64_ccsidr_get_num_sets(uint64_t ccsidr)
{
  return CCSIDR_NUMSETS_GET(ccsidr) + 1;
}

static inline void
AArch64_set_csselr(uint64_t val)
{
  __asm__ volatile (
    "msr CSSELR_EL1, %[val]\n"
    :
    : [val] "r" (val)
  );
}
#define CSSELR_TND BSP_BIT64(4)
/* This field is level-1: L1 cache is 0, L2 cache is 1, etc */
#define CSSELR_LEVEL(val) BSP_FLD64(val, 1, 3)
#define CSSELR_LEVEL_GET(reg) BSP_FLD64GET(reg, 1, 3)
#define CSSELR_LEVEL_SET(reg, val) BSP_FLD64SET(reg, val, 1, 3)
#define CSSELR_IND BSP_BIT64(0)

static inline uint64_t AArch64_get_ccsidr_for_level(uint64_t val)
{
  AArch64_set_csselr(val);
  return AArch64_get_ccsidr();
}

static inline void AArch64_data_cache_clean_level(uint64_t level)
{
  uint64_t ccsidr;
  uint64_t line_power;
  uint64_t associativity;
  uint64_t way;
  uint64_t way_shift;

  ccsidr = AArch64_get_ccsidr_for_level(CSSELR_LEVEL(level));

  line_power = AArch64_ccsidr_get_line_power(ccsidr);
  associativity = AArch64_ccsidr_get_associativity(ccsidr);
  way_shift = __builtin_clz(associativity - 1);

  for (way = 0; way < associativity; ++way) {
    uint64_t num_sets = AArch64_ccsidr_get_num_sets(ccsidr);
    uint64_t set;

    for (set = 0; set < num_sets; ++set) {
      uint64_t set_and_way = (way << way_shift)
        | (set << line_power)
        | (level << 1);

      __asm__ volatile (
        "dc csw, %[set_and_way]"
        :
        : [set_and_way] "r" (set_and_way)
        : "memory"
      );
    }
  }
}

static inline uint64_t
AArch64_get_clidr(void)
{
  uint64_t val;

  __asm__ volatile (
    "mrs %[val], CLIDR_EL1\n"
    : [val] "=&r" (val)
  );

  return val;
}

#define CLIDR_LOC(val) BSP_FLD64(val, 24, 26)
#define CLIDR_LOC_GET(reg) BSP_FLD64GET(reg, 24, 26)
#define CLIDR_LOC_SET(reg, val) BSP_FLD64SET(reg, val, 24, 26)
#define CLIDR_CTYPE7(val) BSP_FLD64(val, 18, 20)
#define CLIDR_CTYPE7_GET(reg) BSP_FLD64GET(reg, 18, 20)
#define CLIDR_CTYPE7_SET(reg, val) BSP_FLD64SET(reg, val, 18, 20)
#define CLIDR_CTYPE6(val) BSP_FLD64(val, 15, 17)
#define CLIDR_CTYPE6_GET(reg) BSP_FLD64GET(reg, 15, 17)
#define CLIDR_CTYPE6_SET(reg, val) BSP_FLD64SET(reg, val, 15, 17)
#define CLIDR_CTYPE5(val) BSP_FLD64(val, 12, 14)
#define CLIDR_CTYPE5_GET(reg) BSP_FLD64GET(reg, 12, 14)
#define CLIDR_CTYPE5_SET(reg, val) BSP_FLD64SET(reg, val, 12, 14)
#define CLIDR_CTYPE4(val) BSP_FLD64(val, 9, 11)
#define CLIDR_CTYPE4_GET(reg) BSP_FLD64GET(reg, 9, 11)
#define CLIDR_CTYPE4_SET(reg, val) BSP_FLD64SET(reg, val, 9, 11)
#define CLIDR_CTYPE3(val) BSP_FLD64(val, 6, 8)
#define CLIDR_CTYPE3_GET(reg) BSP_FLD64GET(reg, 6, 8)
#define CLIDR_CTYPE3_SET(reg, val) BSP_FLD64SET(reg, val, 6, 8)
#define CLIDR_CTYPE2(val) BSP_FLD64(val, 3, 5)
#define CLIDR_CTYPE2_GET(reg) BSP_FLD64GET(reg, 3, 5)
#define CLIDR_CTYPE2_SET(reg, val) BSP_FLD64SET(reg, val, 3, 5)
#define CLIDR_CTYPE1(val) BSP_FLD64(val, 0, 2)
#define CLIDR_CTYPE1_GET(reg) BSP_FLD64GET(reg, 0, 2)
#define CLIDR_CTYPE1_SET(reg, val) BSP_FLD64SET(reg, val, 0, 2)

static inline
uint64_t AArch64_clidr_get_cache_type(uint64_t clidr, uint64_t level)
{
  switch (level)
  {
  case 1:
    return CLIDR_CTYPE1_GET(clidr);
  case 2:
    return CLIDR_CTYPE2_GET(clidr);
  case 3:
    return CLIDR_CTYPE3_GET(clidr);
  case 4:
    return CLIDR_CTYPE4_GET(clidr);
  case 5:
    return CLIDR_CTYPE5_GET(clidr);
  case 6:
    return CLIDR_CTYPE6_GET(clidr);
  case 7:
    return CLIDR_CTYPE7_GET(clidr);
  default:
    return 0;
  }
}

static inline uint64_t AArch64_clidr_get_level_of_coherency(uint64_t clidr)
{
  return CLIDR_LOC_GET(clidr);
}

static inline void AArch64_data_cache_clean_all_levels(void)
{
  uint64_t clidr = AArch64_get_clidr();
  uint64_t loc = AArch64_clidr_get_level_of_coherency(clidr);
  uint64_t level = 0;

  for (level = 0; level < loc; ++level) {
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

  ccsidr = AArch64_get_ccsidr_for_level(CSSELR_LEVEL(level));

  line_power = AArch64_ccsidr_get_line_power(ccsidr);
  associativity = AArch64_ccsidr_get_associativity(ccsidr);
  way_shift = __builtin_clz(associativity - 1);

  for (way = 0; way < associativity; ++way) {
    uint64_t num_sets = AArch64_ccsidr_get_num_sets(ccsidr);
    uint64_t set;

    for (set = 0; set < num_sets; ++set) {
      uint64_t set_and_way = (way << way_shift)
        | (set << line_power)
        | (level << 1);

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
  uint64_t clidr = AArch64_get_clidr();
  uint64_t loc = AArch64_clidr_get_level_of_coherency(clidr);
  uint64_t level = 0;

  for (level = 0; level < loc; ++level) {
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

static inline uint64_t
AArch64_get_sctlr(void)
{
  uint64_t val;

  __asm__ volatile (
    "mrs %[val], SCTLR_EL1\n"
    : [val] "=&r" (val)
  );

  return val;
}

static inline void
AArch64_set_sctlr(uint64_t val)
{
  __asm__ volatile (
    "msr SCTLR_EL1, %[val]\n"
    :
    : [val] "r" (val)
  );
}

#define SCTLR_TWEDEL(val) BSP_FLD64(val, 46, 49)
#define SCTLR_TWEDEL_GET(reg) BSP_FLD64GET(reg, 46, 49)
#define SCTLR_TWEDEL_SET(reg, val) BSP_FLD64SET(reg, val, 46, 49)
#define SCTLR_TWEDEN BSP_BIT64(45)
#define SCTLR_DSSBS BSP_BIT64(44)
#define SCTLR_ATA BSP_BIT64(43)
#define SCTLR_ATA0 BSP_BIT64(42)
#define SCTLR_TCF(val) BSP_FLD64(val, 40, 41)
#define SCTLR_TCF_GET(reg) BSP_FLD64GET(reg, 40, 41)
#define SCTLR_TCF_SET(reg, val) BSP_FLD64SET(reg, val, 40, 41)
#define SCTLR_TCF0(val) BSP_FLD64(val, 38, 39)
#define SCTLR_TCF0_GET(reg) BSP_FLD64GET(reg, 38, 39)
#define SCTLR_TCF0_SET(reg, val) BSP_FLD64SET(reg, val, 38, 39)
#define SCTLR_ITFSB BSP_BIT64(37)
#define SCTLR_BT1 BSP_BIT64(36)
#define SCTLR_BT0 BSP_BIT64(35)
#define SCTLR_ENIA BSP_BIT64(31)
#define SCTLR_ENIB BSP_BIT64(30)
#define SCTLR_LSMAOE BSP_BIT64(29)
#define SCTLR_NTLSMD BSP_BIT64(28)
#define SCTLR_ENDA BSP_BIT64(27)
#define SCTLR_UCI BSP_BIT64(26)
#define SCTLR_EE BSP_BIT64(25)
#define SCTLR_E0E BSP_BIT64(24)
#define SCTLR_SPAN BSP_BIT64(23)
#define SCTLR_EIS BSP_BIT64(22)
#define SCTLR_IESB BSP_BIT64(21)
#define SCTLR_TSCXT BSP_BIT64(20)
#define SCTLR_WXN BSP_BIT64(19)
#define SCTLR_NTWE BSP_BIT64(18)
#define SCTLR_NTWI BSP_BIT64(16)
#define SCTLR_UCT BSP_BIT64(15)
#define SCTLR_DZE BSP_BIT64(14)
#define SCTLR_ENDB BSP_BIT64(13)
#define SCTLR_I BSP_BIT64(12)
#define SCTLR_EOS BSP_BIT64(11)
#define SCTLR_ENRCTX BSP_BIT64(10)
#define SCTLR_UMA BSP_BIT64(9)
#define SCTLR_SED BSP_BIT64(8)
#define SCTLR_ITD BSP_BIT64(7)
#define SCTLR_NAA BSP_BIT64(6)
#define SCTLR_CP15BEN BSP_BIT64(5)
#define SCTLR_SA0 BSP_BIT64(4)
#define SCTLR_SA BSP_BIT64(3)
#define SCTLR_C BSP_BIT64(2)
#define SCTLR_A BSP_BIT64(1)
#define SCTLR_M BSP_BIT64(0)

static inline void _CPU_cache_enable_data(void)
{
  rtems_interrupt_level level;
  uint64_t sctlr;

  rtems_interrupt_local_disable(level);
  sctlr = AArch64_get_sctlr();
  sctlr |= SCTLR_C;
  AArch64_set_sctlr(sctlr);
  rtems_interrupt_local_enable(level);
}

static inline void _CPU_cache_disable_data(void)
{
  rtems_interrupt_level level;
  uint64_t sctlr;

  rtems_interrupt_local_disable(level);
  AArch64_data_cache_clean_all_levels();
  AArch64_data_cache_invalidate_all_levels();
  sctlr = AArch64_get_sctlr();
  sctlr &= ~SCTLR_C;
  AArch64_set_sctlr(sctlr);
  rtems_interrupt_local_enable(level);
}

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
  sctlr = AArch64_get_sctlr();
  sctlr |= SCTLR_I;
  AArch64_set_sctlr(sctlr);
  rtems_interrupt_local_enable(level);
}

static inline void _CPU_cache_disable_instruction(void)
{
  rtems_interrupt_level level;
  uint64_t sctlr;

  rtems_interrupt_local_disable(level);
  sctlr = AArch64_get_sctlr();
  sctlr &= ~SCTLR_I;
  AArch64_set_sctlr(sctlr);
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

  clidr = AArch64_get_clidr();
  loc = AArch64_clidr_get_level_of_coherency(clidr);

  if (level >= loc) {
    return 0;
  }

  if (level == 0) {
    level = loc - 1;
  }

  ccsidr = AArch64_get_ccsidr_for_level(
    CSSELR_LEVEL(level) | (instruction ? CSSELR_IND : 0)
  );

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
