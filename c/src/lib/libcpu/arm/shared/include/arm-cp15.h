/**
 * @file
 *
 * @ingroup ScoreCPUARMCP15
 *
 * @brief ARM co-processor 15 (CP15) API.
 */

/*
 * Copyright (c) 2009, 2010
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
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
#define ARM_MMU_SECT_DOMAIN_SHIFT 5
#define ARM_MMU_SECT_DOMAIN_MASK (0xfU << ARM_MMU_SECT_DOMAIN_SHIFT)
#define ARM_MMU_SECT_AP_1 (1U << 11)
#define ARM_MMU_SECT_AP_0 (1U << 10)
#define ARM_MMU_SECT_AP_SHIFT 10
#define ARM_MMU_SECT_AP_MASK (0x3U << ARM_MMU_SECT_AP_SHIFT)
#define ARM_MMU_SECT_C (1U << 3)
#define ARM_MMU_SECT_B (1U << 2)
#define ARM_MMU_SECT_DEFAULT 0x12U
#define ARM_MMU_SECT_GET_INDEX(mva) \
  (((uint32_t) (mva)) >> ARM_MMU_SECT_BASE_SHIFT)
#define ARM_MMU_SECT_MVA_ALIGN_UP(mva) \
  ((1U << ARM_MMU_SECT_BASE_SHIFT) \
    + ((((uint32_t) (mva) - 1U)) & ~((1U << ARM_MMU_SECT_BASE_SHIFT) - 1U)))

#define ARM_MMU_TRANSLATION_TABLE_ENTRY_SIZE 4U
#define ARM_MMU_TRANSLATION_TABLE_ENTRY_COUNT 4096U

/** @} */

/**
 * @name Control Register Defines
 *
 * @{
 */

#define ARM_CP15_CTRL_L4 (1U << 15)
#define ARM_CP15_CTRL_RR (1U << 14)
#define ARM_CP15_CTRL_V (1U << 13)
#define ARM_CP15_CTRL_I (1U << 12)
#define ARM_CP15_CTRL_R (1U << 9)
#define ARM_CP15_CTRL_S (1U << 8)
#define ARM_CP15_CTRL_B (1U << 7)
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

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCPU_SHARED_ARM_CP15_H */
