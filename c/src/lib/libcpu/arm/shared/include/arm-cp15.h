/**
 * @file
 *
 * @ingroup ScoreCPUARMCP15
 *
 * @brief ARM co-processor 15 (CP15) API.
 */

/*
 * Copyright (c) 2009-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBCPU_SHARED_ARM_CP15_H
#define LIBCPU_SHARED_ARM_CP15_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ARM_CP15_CACHE_PREPARE_MVA(mva) \
  ((const void *) (((uint32_t) (mva)) & ~0x1fU))

#define ARM_CP15_TLB_PREPARE_MVA(mva) \
  ((const void *) (((uint32_t) (mva)) & ~0x3fU))

/**
 * @defgroup ScoreCPUARMCP15 ARM Co-Processor 15 Support
 *
 * @ingroup ScoreCPUARM
 *
 * @brief ARM co-processor 15 (CP15) support.
 *
 * @{
 */

/**
 * @name MMU Defines
 *
 * @{
 */

#define ARM_MMU_SECT_BASE_SHIFT 20
#define ARM_MMU_SECT_BASE_MASK (0xfffU << ARM_MMU_SECT_BASE_SHIFT)
#define ARM_MMU_SECT_NS (1U << 19)
#define ARM_MMU_SECT_NG (1U << 17)
#define ARM_MMU_SECT_S (1U << 16)
#define ARM_MMU_SECT_AP_2 (1U << 15)
#define ARM_MMU_SECT_TEX_2 (1U << 14)
#define ARM_MMU_SECT_TEX_1 (1U << 13)
#define ARM_MMU_SECT_TEX_0 (1U << 12)
#define ARM_MMU_SECT_TEX_SHIFT 12
#define ARM_MMU_SECT_TEX_MASK (0x3U << ARM_MMU_SECT_TEX_SHIFT)
#define ARM_MMU_SECT_AP_1 (1U << 11)
#define ARM_MMU_SECT_AP_0 (1U << 10)
#define ARM_MMU_SECT_AP_SHIFT 10
#define ARM_MMU_SECT_AP_MASK (0x23U << ARM_MMU_SECT_AP_SHIFT)
#define ARM_MMU_SECT_DOMAIN_SHIFT 5
#define ARM_MMU_SECT_DOMAIN_MASK (0xfU << ARM_MMU_SECT_DOMAIN_SHIFT)
#define ARM_MMU_SECT_XN (1U << 4)
#define ARM_MMU_SECT_C (1U << 3)
#define ARM_MMU_SECT_B (1U << 2)
#define ARM_MMU_SECT_PXN (1U << 0)
#define ARM_MMU_SECT_DEFAULT 0x2U
#define ARM_MMU_SECT_GET_INDEX(mva) \
  (((uint32_t) (mva)) >> ARM_MMU_SECT_BASE_SHIFT)
#define ARM_MMU_SECT_MVA_ALIGN_UP(mva) \
  ((1U << ARM_MMU_SECT_BASE_SHIFT) \
    + ((((uint32_t) (mva) - 1U)) & ~((1U << ARM_MMU_SECT_BASE_SHIFT) - 1U)))

#define ARM_MMU_TRANSLATION_TABLE_ENTRY_SIZE 4U
#define ARM_MMU_TRANSLATION_TABLE_ENTRY_COUNT 4096U

#define ARM_MMU_DEFAULT_CLIENT_DOMAIN 15U

#define ARMV7_MMU_READ_ONLY \
  ((ARM_MMU_DEFAULT_CLIENT_DOMAIN << ARM_MMU_SECT_DOMAIN_SHIFT) \
    | ARM_MMU_SECT_AP_0 \
    | ARM_MMU_SECT_AP_2 \
    | ARM_MMU_SECT_DEFAULT)

#define ARMV7_MMU_READ_ONLY_CACHED \
  (ARMV7_MMU_READ_ONLY | ARM_MMU_SECT_TEX_0 | ARM_MMU_SECT_C | ARM_MMU_SECT_B)

#define ARMV7_MMU_READ_WRITE \
  ((ARM_MMU_DEFAULT_CLIENT_DOMAIN << ARM_MMU_SECT_DOMAIN_SHIFT) \
    | ARM_MMU_SECT_AP_0 \
    | ARM_MMU_SECT_DEFAULT)

#define ARMV7_MMU_READ_WRITE_CACHED \
  (ARMV7_MMU_READ_WRITE | ARM_MMU_SECT_TEX_0 | ARM_MMU_SECT_C | ARM_MMU_SECT_B)

#define ARMV7_MMU_DATA_READ_ONLY \
  ARMV7_MMU_READ_ONLY

#define ARMV7_MMU_DATA_READ_ONLY_CACHED \
  ARMV7_MMU_READ_ONLY_CACHED

#define ARMV7_MMU_DATA_READ_WRITE \
  ARMV7_MMU_READ_WRITE

#define ARMV7_MMU_DATA_READ_WRITE_CACHED \
  ARMV7_MMU_READ_WRITE_CACHED

#define ARMV7_MMU_DATA_READ_WRITE_SHAREABLE \
  (ARMV7_MMU_READ_WRITE_CACHED | ARM_MMU_SECT_S)

#define ARMV7_MMU_CODE \
  ARMV7_MMU_READ_ONLY

#define ARMV7_MMU_CODE_CACHED \
  ARMV7_MMU_READ_ONLY_CACHED

#define ARMV7_MMU_DEVICE \
  (ARMV7_MMU_READ_WRITE | ARM_MMU_SECT_B)

/** @} */

/**
 * @name Control Register Defines
 *
 * @{
 */

#define ARM_CP15_CTRL_TE (1U << 30)
#define ARM_CP15_CTRL_AFE (1U << 29)
#define ARM_CP15_CTRL_TRE (1U << 28)
#define ARM_CP15_CTRL_NMFI (1U << 27)
#define ARM_CP15_CTRL_EE (1U << 25)
#define ARM_CP15_CTRL_VE (1U << 24)
#define ARM_CP15_CTRL_U (1U << 22)
#define ARM_CP15_CTRL_FI (1U << 21)
#define ARM_CP15_CTRL_UWXN (1U << 20)
#define ARM_CP15_CTRL_WXN (1U << 19)
#define ARM_CP15_CTRL_HA (1U << 17)
#define ARM_CP15_CTRL_L4 (1U << 15)
#define ARM_CP15_CTRL_RR (1U << 14)
#define ARM_CP15_CTRL_V (1U << 13)
#define ARM_CP15_CTRL_I (1U << 12)
#define ARM_CP15_CTRL_Z (1U << 11)
#define ARM_CP15_CTRL_SW (1U << 10)
#define ARM_CP15_CTRL_R (1U << 9)
#define ARM_CP15_CTRL_S (1U << 8)
#define ARM_CP15_CTRL_B (1U << 7)
#define ARM_CP15_CTRL_CP15BEN (1U << 5)
#define ARM_CP15_CTRL_C (1U << 2)
#define ARM_CP15_CTRL_A (1U << 1)
#define ARM_CP15_CTRL_M (1U << 0)

/** @} */

/**
 * @name Domain Access Control Defines
 *
 * @{
 */

#define ARM_CP15_DAC_NO_ACCESS 0x0U
#define ARM_CP15_DAC_CLIENT 0x1U
#define ARM_CP15_DAC_MANAGER 0x3U
#define ARM_CP15_DAC_DOMAIN(index, val) ((val) << (2 * index))

/** @} */

static inline uint32_t arm_cp15_get_id_code(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c0, c0, 0\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

static inline uint32_t arm_cp15_get_tcm_status(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c0, c0, 2\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

static inline uint32_t arm_cp15_get_control(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c1, c0, 0\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

static inline void arm_cp15_set_control(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[val], c1, c0, 0\n"
    "nop\n"
    "nop\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
    : "memory"
  );
}

/**
 * @name MMU Functions
 *
 * @{
 */

/**
 * @brief Disable the MMU.
 *
 * This function will clean and invalidate eight cache lines before and after
 * the current stack pointer.
 *
 * @param[in] cls The data cache line size.
 *
 * @return The current control register value.
 */
static inline uint32_t arm_cp15_mmu_disable(uint32_t cls)
{
  ARM_SWITCH_REGISTERS;
  uint32_t ctrl;
  uint32_t tmp_0;
  uint32_t tmp_1;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[ctrl], c1, c0, 0\n"
    "bic %[tmp_0], %[ctrl], #1\n"
    "mcr p15, 0, %[tmp_0], c1, c0, 0\n"
    "nop\n"
    "nop\n"
    "mov %[tmp_1], sp\n"
    "rsb %[tmp_0], %[cls], #0\n"
    "and %[tmp_0], %[tmp_0], %[tmp_1]\n"
    "sub %[tmp_0], %[tmp_0], %[cls], asl #3\n"
    "add %[tmp_1], %[tmp_0], %[cls], asl #4\n"
    "1:\n"
    "mcr p15, 0, %[tmp_0], c7, c14, 1\n"
    "add %[tmp_0], %[tmp_0], %[cls]\n"
    "cmp %[tmp_1], %[tmp_0]\n"
    "bne 1b\n"
    ARM_SWITCH_BACK
    : [ctrl] "=&r" (ctrl),
      [tmp_0] "=&r" (tmp_0),
      [tmp_1] "=&r" (tmp_1)
      ARM_SWITCH_ADDITIONAL_OUTPUT
    : [cls] "r" (cls)
    : "memory", "cc"
  );

  return ctrl;
}

static inline uint32_t *arm_cp15_get_translation_table_base(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t *base;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[base], c2, c0, 0\n"
    ARM_SWITCH_BACK
    : [base] "=&r" (base) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return base;
}

static inline void arm_cp15_set_translation_table_base(uint32_t *base)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[base], c2, c0, 0\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [base] "r" (base)
  );
}

static inline uint32_t arm_cp15_get_domain_access_control(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c3, c0, 0\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

static inline void arm_cp15_set_domain_access_control(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[val], c3, c0, 0\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
  );
}

static inline uint32_t arm_cp15_get_data_fault_status(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c5, c0, 0\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

static inline void arm_cp15_set_data_fault_status(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[val], c5, c0, 0\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
  );
}

static inline uint32_t arm_cp15_get_instruction_fault_status(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c5, c0, 1\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

static inline void arm_cp15_set_instruction_fault_status(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[val], c5, c0, 1\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
  );
}

static inline void *arm_cp15_get_fault_address(void)
{
  ARM_SWITCH_REGISTERS;
  void *mva;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[mva], c6, c0, 0\n"
    ARM_SWITCH_BACK
    : [mva] "=&r" (mva) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return mva;
}

static inline void arm_cp15_set_fault_address(const void *mva)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[mva], c6, c0, 0\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [mva] "r" (mva)
  );
}

static inline void arm_cp15_tlb_invalidate(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t sbz = 0;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[sbz], c8, c7, 0\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [sbz] "r" (sbz)
  );
}

static inline void arm_cp15_tlb_invalidate_entry(const void *mva)
{
  ARM_SWITCH_REGISTERS;

  mva = ARM_CP15_TLB_PREPARE_MVA(mva);

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[mva], c8, c7, 1\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [mva] "r" (mva)
  );
}

static inline void arm_cp15_tlb_instruction_invalidate(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t sbz = 0;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[sbz], c8, c5, 0\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [sbz] "r" (sbz)
  );
}

static inline void arm_cp15_tlb_instruction_invalidate_entry(const void *mva)
{
  ARM_SWITCH_REGISTERS;

  mva = ARM_CP15_TLB_PREPARE_MVA(mva);

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[mva], c8, c5, 1\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [mva] "r" (mva)
  );
}

static inline void arm_cp15_tlb_data_invalidate(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t sbz = 0;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[sbz], c8, c6, 0\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [sbz] "r" (sbz)
  );
}

static inline void arm_cp15_tlb_data_invalidate_entry(const void *mva)
{
  ARM_SWITCH_REGISTERS;

  mva = ARM_CP15_TLB_PREPARE_MVA(mva);

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[mva], c8, c6, 1\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [mva] "r" (mva)
  );
}

static inline void arm_cp15_tlb_lockdown_entry(const void *mva)
{
  uint32_t arm_switch_reg;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "add %[arm_switch_reg], pc, #16\n"
    "mcr p15, 0, %[arm_switch_reg], c7, c13, 1\n"
    "mcr p15, 0, %[mva], c8, c7, 1\n"
    "mrc p15, 0, %[arm_switch_reg], c10, c0, 0\n"
    "orr %[arm_switch_reg], #0x1\n"
    "mcr p15, 0, %[arm_switch_reg], c10, c0, 0\n"
    "ldr %[mva], [%[mva]]\n"
    "mrc p15, 0, %[arm_switch_reg], c10, c0, 0\n"
    "bic %[arm_switch_reg], #0x1\n"
    "mcr p15, 0, %[arm_switch_reg], c10, c0, 0\n"
    ARM_SWITCH_BACK
    : [mva] "=r" (mva), [arm_switch_reg] "=&r" (arm_switch_reg)
    : "[mva]" (mva)
  );
}

/** @} */

/**
 * @name Cache Functions
 *
 * @{
 */

static inline uint32_t arm_cp15_get_cache_type(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c0, c0, 1\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

static inline uint32_t arm_cp15_get_min_cache_line_size(void)
{
  uint32_t mcls = 0;
  uint32_t ct = arm_cp15_get_cache_type();
  uint32_t format = (ct >> 29) & 0x7U;

  if (format == 0x4) {
    mcls = (1U << (ct & 0xf)) * 4;
  } else if (format == 0x0) {
    uint32_t mask = (1U << 12) - 1;
    uint32_t dcls = (ct >> 12) & mask;
    uint32_t icls = ct & mask;

    mcls = dcls <= icls ? dcls : icls;
  }

  return mcls;
}

static inline void arm_cp15_cache_invalidate(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t sbz = 0;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[sbz], c7, c7, 0\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [sbz] "r" (sbz)
    : "memory"
  );
}

static inline void arm_cp15_instruction_cache_invalidate(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t sbz = 0;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[sbz], c7, c5, 0\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [sbz] "r" (sbz)
    : "memory"
  );
}

static inline void arm_cp15_instruction_cache_invalidate_line(const void *mva)
{
  ARM_SWITCH_REGISTERS;

  mva = ARM_CP15_CACHE_PREPARE_MVA(mva);

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[mva], c7, c5, 1\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [mva] "r" (mva)
    : "memory"
  );
}

static inline void arm_cp15_instruction_cache_invalidate_line_by_set_and_way(uint32_t set_and_way)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[set_and_way], c7, c5, 2\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [set_and_way] "r" (set_and_way)
    : "memory"
  );
}

static inline void arm_cp15_instruction_cache_prefetch_line(const void *mva)
{
  ARM_SWITCH_REGISTERS;

  mva = ARM_CP15_CACHE_PREPARE_MVA(mva);

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[mva], c7, c13, 1\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [mva] "r" (mva)
  );
}

static inline void arm_cp15_data_cache_invalidate(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t sbz = 0;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[sbz], c7, c6, 0\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [sbz] "r" (sbz)
    : "memory"
  );
}

static inline void arm_cp15_data_cache_invalidate_line(const void *mva)
{
  ARM_SWITCH_REGISTERS;

  mva = ARM_CP15_CACHE_PREPARE_MVA(mva);

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[mva], c7, c6, 1\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [mva] "r" (mva)
    : "memory"
  );
}

static inline void arm_cp15_data_cache_invalidate_line_by_set_and_way(uint32_t set_and_way)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[set_and_way], c7, c6, 2\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [set_and_way] "r" (set_and_way)
    : "memory"
  );
}

static inline void arm_cp15_data_cache_clean_line(const void *mva)
{
  ARM_SWITCH_REGISTERS;

  mva = ARM_CP15_CACHE_PREPARE_MVA(mva);

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[mva], c7, c10, 1\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [mva] "r" (mva)
    : "memory"
  );
}

static inline void arm_cp15_data_cache_clean_line_by_set_and_way(uint32_t set_and_way)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[set_and_way], c7, c10, 2\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [set_and_way] "r" (set_and_way)
    : "memory"
  );
}

static inline void arm_cp15_data_cache_test_and_clean(void)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "1:\n"
    "mrc p15, 0, r15, c7, c10, 3\n"
    "bne 1b\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    :
    : "memory"
  );
}

/*	In DDI0301H_arm1176jzfs_r0p7_trm
 * 	'MCR p15, 0, <Rd>, c7, c14, 0' means
 * 	Clean and Invalidate Entire Data Cache
 */
static inline void arm_cp15_data_cache_clean_and_invalidate(void)
{
  ARM_SWITCH_REGISTERS;

  uint32_t sbz = 0;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[sbz], c7, c14, 0\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [sbz] "r" (sbz)
    : "memory"
  );

}

static inline void arm_cp15_data_cache_clean_and_invalidate_line(const void *mva)
{
  ARM_SWITCH_REGISTERS;

  mva = ARM_CP15_CACHE_PREPARE_MVA(mva);

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[mva], c7, c14, 1\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [mva] "r" (mva)
    : "memory"
  );
}

static inline void arm_cp15_data_cache_clean_and_invalidate_line_by_set_and_way(uint32_t set_and_way)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[set_and_way], c7, c14, 2\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [set_and_way] "r" (set_and_way)
    : "memory"
  );
}

static inline void arm_cp15_data_cache_test_and_clean_and_invalidate(void)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "1:\n"
    "mrc p15, 0, r15, c7, c14, 3\n"
    "bne 1b\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    :
    : "memory"
  );
}

/** @} */

static inline void arm_cp15_drain_write_buffer(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t sbz = 0;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[sbz], c7, c10, 4\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [sbz] "r" (sbz)
    : "memory"
  );
}

static inline void arm_cp15_wait_for_interrupt(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t sbz = 0;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[sbz], c7, c0, 4\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [sbz] "r" (sbz)
    : "memory"
  );
}

static inline uint32_t arm_cp15_get_multiprocessor_affinity(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t mpidr;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
	  "mrc p15, 0, %[mpidr], c0, c0, 5\n"
    ARM_SWITCH_BACK
    : [mpidr] "=&r" (mpidr) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return mpidr & 0xff;
}

static inline uint32_t arm_cortex_a9_get_multiprocessor_cpu_id(void)
{
  return arm_cp15_get_multiprocessor_affinity() & 0xff;
}

#define ARM_CORTEX_A9_ACTL_FW (1U << 0)
#define ARM_CORTEX_A9_ACTL_L2_PREFETCH_HINT_ENABLE (1U << 1)
#define ARM_CORTEX_A9_ACTL_L1_PREFETCH_ENABLE (1U << 2)
#define ARM_CORTEX_A9_ACTL_WRITE_FULL_LINE_OF_ZEROS_MODE (1U << 3)
#define ARM_CORTEX_A9_ACTL_SMP (1U << 6)
#define ARM_CORTEX_A9_ACTL_EXCL (1U << 7)
#define ARM_CORTEX_A9_ACTL_ALLOC_IN_ONE_WAY (1U << 8)
#define ARM_CORTEX_A9_ACTL_PARITY_ON (1U << 9)

static inline uint32_t arm_cp15_get_auxiliary_control(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c1, c0, 1\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

static inline void arm_cp15_set_auxiliary_control(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[val], c1, c0, 1\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
  );
}

/* ID_PFR1, Processor Feature Register 1 */

static inline uint32_t arm_cp15_get_processor_feature_1(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c0, c1, 1\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

/* VBAR, Vector Base Address Register, Security Extensions */

static inline void *arm_cp15_get_vector_base_address(void)
{
  ARM_SWITCH_REGISTERS;
  void *base;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[base], c12, c0, 0\n"
    ARM_SWITCH_BACK
    : [base] "=&r" (base) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return base;
}

static inline void arm_cp15_set_vector_base_address(void *base)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[base], c12, c0, 0\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [base] "r" (base)
  );
}

/**
 * @brief Sets the @a section_flags for the address range [@a begin, @a end).
 *
 * @return Previous section flags of the first modified entry.
 */
uint32_t arm_cp15_set_translation_table_entries(
  const void *begin,
  const void *end,
  uint32_t section_flags
);

void arm_cp15_set_exception_handler(
  Arm_symbolic_exception_name exception,
  void (*handler)(void)
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCPU_SHARED_ARM_CP15_H */
