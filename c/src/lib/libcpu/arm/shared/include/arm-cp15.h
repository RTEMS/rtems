/**
 * @file
 *
 * @ingroup ScoreCPUARMCP15
 *
 * @brief ARM co-processor 15 (CP15) API.
 */

/*
 * Copyright (c) 2013 Hesham AL-Matary
 * Copyright (c) 2009-2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBCPU_SHARED_ARM_CP15_H
#define LIBCPU_SHARED_ARM_CP15_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Allow users of this header file to optionally place the inline functions
 * into a non-standard section.
 */
#ifndef ARM_CP15_TEXT_SECTION
  #define ARM_CP15_TEXT_SECTION
#endif

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
 * See section B3.8.2, "Short-descriptor format memory region attributes,
 * without TEX remap" in the "ARM Architecture Reference Manual ARMv7-A and
 * ARMv7-R edition".
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

#ifdef RTEMS_SMP
  #define ARMV7_MMU_READ_WRITE_CACHED \
    (ARMV7_MMU_READ_WRITE \
      | ARM_MMU_SECT_TEX_0 | ARM_MMU_SECT_C | ARM_MMU_SECT_B | ARM_MMU_SECT_S)
#else
  #define ARMV7_MMU_READ_WRITE_CACHED \
    (ARMV7_MMU_READ_WRITE \
      | ARM_MMU_SECT_TEX_0 | ARM_MMU_SECT_C | ARM_MMU_SECT_B)
#endif

#define ARMV7_MMU_DATA_READ_ONLY \
  (ARMV7_MMU_READ_ONLY | ARM_MMU_SECT_TEX_0)

#define ARMV7_MMU_DATA_READ_ONLY_CACHED \
  ARMV7_MMU_READ_ONLY_CACHED

#define ARMV7_MMU_DATA_READ_WRITE \
  (ARMV7_MMU_READ_WRITE | ARM_MMU_SECT_TEX_0)

#define ARMV7_MMU_DATA_READ_WRITE_CACHED \
  ARMV7_MMU_READ_WRITE_CACHED

#define ARMV7_MMU_CODE \
  (ARMV7_MMU_READ_ONLY | ARM_MMU_SECT_TEX_0)

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
#define ARM_CP15_CTRL_XP (1U << 23)
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

/**
 * @name Fault Status Register Defines
 *
 * @{
 */

#define ARM_CP15_FAULT_STATUS_MASK 0x040F

#define ARM_CP15_FSR_ALIGNMENT_FAULT   0x00000001
#define ARM_CP15_FSR_BACKGROUND_FAULT  0x0000
#define ARM_CP15_FSR_ACCESS_PERMISSION_FAULT 0x000D
#define ARM_CP15_FSR_PRECISE_EXTERNAL_ABORT_FAULT 0x0008
#define ARM_CP15_FSR_IMPRECISE_EXTERNAL_ABORT_FAULT 0x0406
#define ARM_CP15_FSR_PRECISE_PARITY_ERROR_EXCEPTION 0x0006
#define ARM_CP15_FSR_IMPRECISE_PARITY_ERROR_EXCEPTION 0x0408
#define ARM_CP15_FSR_DEBUG_EVENT 0x0002

/** @} */

/**
 * @name CTR, Cache Type Register Defines
 *
 * The format can be obtained from CP15 by call
 * arm_cp15_cache_type_get_format(arm_cp15_get_cache_type());
 *
 * @{
 */

#define ARM_CP15_CACHE_TYPE_FORMAT_ARMV6 0
#define ARM_CP15_CACHE_TYPE_FORMAT_ARMV7 4

/** @} */

/**
 * @name CCSIDR, Cache Size ID Register Defines
 *
 * @{
 */

#define ARM_CP15_CACHE_CSS_ID_DATA 0
#define ARM_CP15_CACHE_CSS_ID_INSTRUCTION 1
#define ARM_CP15_CACHE_CSS_LEVEL(level) ((level) << 1)

/** @} */

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_id_code(void)
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

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_tcm_status(void)
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

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_control(void)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_control(uint32_t val)
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
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_mmu_disable(uint32_t cls)
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

ARM_CP15_TEXT_SECTION static inline uint32_t
*arm_cp15_get_translation_table_base(void)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_translation_table_base(uint32_t *base)
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

/* Translation Table Base Control Register - DDI0301H arm1176jzfs TRM 3.2.15 */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_translation_table_base_control_register(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t ttb_cr;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[ttb_cr], c2, c0, 2\n"
    ARM_SWITCH_BACK
    : [ttb_cr] "=&r" (ttb_cr) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return ttb_cr;
}

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_translation_table_base_control_register(uint32_t ttb_cr)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[ttb_cr], c2, c0, 2\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [ttb_cr] "r" (ttb_cr)
  );
}

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_domain_access_control(void)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_domain_access_control(uint32_t val)
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

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_data_fault_status(void)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_data_fault_status(uint32_t val)
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

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_instruction_fault_status(void)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_instruction_fault_status(uint32_t val)
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

ARM_CP15_TEXT_SECTION static inline void
*arm_cp15_get_fault_address(void)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_fault_address(const void *mva)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_tlb_invalidate(void)
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

  /*
   * ARM Architecture Reference Manual, ARMv7-A and ARMv7-R edition, Issue C,
   * B3.10.1 General TLB maintenance requirements.
   */
  _ARM_Data_synchronization_barrier();
  _ARM_Instruction_synchronization_barrier();
}

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_tlb_invalidate_entry(const void *mva)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_tlb_invalidate_entry_all_asids(const void *mva)
{
  ARM_SWITCH_REGISTERS;

  mva = ARM_CP15_TLB_PREPARE_MVA(mva);

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[mva], c8, c7, 3\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [mva] "r" (mva)
  );
}

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_tlb_instruction_invalidate(void)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_tlb_instruction_invalidate_entry(const void *mva)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_tlb_data_invalidate(void)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_tlb_data_invalidate_entry(const void *mva)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_tlb_lockdown_entry(const void *mva)
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

/* Read cache type register CTR */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_cache_type(void)
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

/* Extract format version from cache type CTR */
ARM_CP15_TEXT_SECTION static inline int
arm_cp15_cache_type_get_format(uint32_t ct)
{
  return (ct >> 29) & 0x7U;
}

/* Read size of smallest cache line of all instruction/data caches controlled by the processor */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_min_cache_line_size(void)
{
  uint32_t mcls = 0;
  uint32_t ct = arm_cp15_get_cache_type();
  uint32_t format = arm_cp15_cache_type_get_format(ct);

  if (format == ARM_CP15_CACHE_TYPE_FORMAT_ARMV7) {
    /* ARMv7 format */
    mcls = (1U << (ct & 0xf)) * 4;
  } else if (format == ARM_CP15_CACHE_TYPE_FORMAT_ARMV6) {
    /* ARMv6 format */
    uint32_t mask = (1U << 12) - 1;
    uint32_t dcls = (ct >> 12) & mask;
    uint32_t icls = ct & mask;

    mcls = dcls <= icls ? dcls : icls;
  }

  return mcls;
}

/* Read size of smallest data cache lines */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_data_cache_line_size(void)
{
  uint32_t mcls = 0;
  uint32_t ct = arm_cp15_get_cache_type();
  uint32_t format = arm_cp15_cache_type_get_format(ct);

  if (format == ARM_CP15_CACHE_TYPE_FORMAT_ARMV7) {
    /* ARMv7 format */
    mcls = (1U << ((ct & 0xf0000) >> 16)) * 4;
  } else if (format == ARM_CP15_CACHE_TYPE_FORMAT_ARMV6) {
    /* ARMv6 format */
    uint32_t mask = (1U << 12) - 1;
    mcls = (ct >> 12) & mask;
  }

  return mcls;
}

/* Read size of smallest instruction cache lines */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_instruction_cache_line_size(void)
{
  uint32_t mcls = 0;
  uint32_t ct = arm_cp15_get_cache_type();
  uint32_t format = arm_cp15_cache_type_get_format(ct);

  if (format == ARM_CP15_CACHE_TYPE_FORMAT_ARMV7) {
    /* ARMv7 format */
    mcls = (1U << (ct & 0x0000f)) * 4;
  } else if (format == ARM_CP15_CACHE_TYPE_FORMAT_ARMV6) {
    /* ARMv6 format */
    uint32_t mask = (1U << 12) - 1;
    mcls = ct & mask;;
  }

  return mcls;
}

/* CCSIDR, Cache Size ID Register */

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_cache_size_id(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 1, %[val], c0, c0, 0\n"
     ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_ccsidr_get_line_power(uint32_t ccsidr)
{
  return (ccsidr & 0x7) + 4;
}

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_ccsidr_get_associativity(uint32_t ccsidr)
{
  return ((ccsidr >> 3) & 0x3ff) + 1;
}

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_ccsidr_get_num_sets(uint32_t ccsidr)
{
  return ((ccsidr >> 13) & 0x7fff) + 1;
}

/* CLIDR, Cache Level ID Register */

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_cache_level_id(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 1, %[val], c0, c0, 1\n"
     ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_clidr_get_level_of_coherency(uint32_t clidr)
{
  return (clidr >> 24) & 0x7;
}

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_clidr_get_cache_type(uint32_t clidr, uint32_t level)
{
  return (clidr >> (3 * level)) & 0x7;
}

/* CSSELR, Cache Size Selection Register */

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_cache_size_selection(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 2, %[val], c0, c0, 0\n"
     ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_cache_size_selection(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 2, %[val], c0, c0, 0\n"
     ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
    : "memory"
  );
}

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_cache_size_id_for_level(uint32_t level_and_inst_dat)
{
  rtems_interrupt_level irq_level;
  uint32_t ccsidr;

  rtems_interrupt_local_disable(irq_level);
  arm_cp15_set_cache_size_selection(level_and_inst_dat);
  _ARM_Instruction_synchronization_barrier();
  ccsidr = arm_cp15_get_cache_size_id();
  rtems_interrupt_local_enable(irq_level);

  return ccsidr;
}

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_cache_invalidate(void)
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

/* ICIALLUIS, Instruction Cache Invalidate All to PoU, Inner Shareable */

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_instruction_cache_inner_shareable_invalidate_all(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t sbz = 0;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[sbz], c7, c1, 0\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [sbz] "r" (sbz)
    : "memory"
  );
}

/* BPIALLIS, Branch Predictor Invalidate All, Inner Shareable */

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_branch_predictor_inner_shareable_invalidate_all(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t sbz = 0;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[sbz], c7, c1, 6\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [sbz] "r" (sbz)
    : "memory"
  );
}

/* BPIALL, Branch Predictor Invalidate All */

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_branch_predictor_invalidate_all(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t sbz = 0;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[sbz], c7, c5, 6\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [sbz] "r" (sbz)
    : "memory"
  );
}

/* Flush Prefetch Buffer - DDI0301H arm1176jzfs TRM 3.2.22 */
ARM_CP15_TEXT_SECTION static inline void
arm_cp15_flush_prefetch_buffer(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t sbz = 0;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[sbz], c7, c5, 4\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [sbz] "r" (sbz)
    : "memory"
  );
}

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_instruction_cache_invalidate(void)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_instruction_cache_invalidate_line(const void *mva)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_instruction_cache_invalidate_line_by_set_and_way(uint32_t set_and_way)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_instruction_cache_prefetch_line(const void *mva)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_data_cache_invalidate(void)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_data_cache_invalidate_line(const void *mva)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_data_cache_invalidate_line_by_set_and_way(uint32_t set_and_way)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_cache_invalidate_level(uint32_t level, uint32_t inst_data_fl)
{
  uint32_t ccsidr;
  uint32_t line_power;
  uint32_t associativity;
  uint32_t way;
  uint32_t way_shift;

  ccsidr = arm_cp15_get_cache_size_id_for_level((level << 1) | inst_data_fl);

  line_power = arm_ccsidr_get_line_power(ccsidr);
  associativity = arm_ccsidr_get_associativity(ccsidr);
  way_shift = __builtin_clz(associativity - 1);

  for (way = 0; way < associativity; ++way) {
    uint32_t num_sets = arm_ccsidr_get_num_sets(ccsidr);
    uint32_t set;

    for (set = 0; set < num_sets; ++set) {
      uint32_t set_way = (way << way_shift)
        | (set << line_power)
        | (level << 1);

      arm_cp15_data_cache_invalidate_line_by_set_and_way(set_way);
    }
  }
}

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_data_cache_invalidate_all_levels(void)
{
  uint32_t clidr = arm_cp15_get_cache_level_id();
  uint32_t loc = arm_clidr_get_level_of_coherency(clidr);
  uint32_t level = 0;

  for (level = 0; level < loc; ++level) {
    uint32_t ctype = arm_clidr_get_cache_type(clidr, level);

    /* Check if this level has a data cache or unified cache */
    if (((ctype & (0x6)) == 2) || (ctype == 4)) {
      arm_cp15_cache_invalidate_level(level, 0);
    }
  }
}

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_data_cache_clean_line(const void *mva)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_data_cache_clean_line_by_set_and_way(uint32_t set_and_way)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_data_cache_clean_level(uint32_t level)
{
  uint32_t ccsidr;
  uint32_t line_power;
  uint32_t associativity;
  uint32_t way;
  uint32_t way_shift;

  ccsidr = arm_cp15_get_cache_size_id_for_level(level << 1);

  line_power = arm_ccsidr_get_line_power(ccsidr);
  associativity = arm_ccsidr_get_associativity(ccsidr);
  way_shift = __builtin_clz(associativity - 1);

  for (way = 0; way < associativity; ++way) {
    uint32_t num_sets = arm_ccsidr_get_num_sets(ccsidr);
    uint32_t set;

    for (set = 0; set < num_sets; ++set) {
      uint32_t set_way = (way << way_shift)
        | (set << line_power)
        | (level << 1);

      arm_cp15_data_cache_clean_line_by_set_and_way(set_way);
    }
  }
}

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_data_cache_clean_all_levels(void)
{
  uint32_t clidr = arm_cp15_get_cache_level_id();
  uint32_t loc = arm_clidr_get_level_of_coherency(clidr);
  uint32_t level = 0;

  for (level = 0; level < loc; ++level) {
    uint32_t ctype = arm_clidr_get_cache_type(clidr, level);

    /* Check if this level has a data cache or unified cache */
    if (((ctype & (0x6)) == 2) || (ctype == 4)) {
      arm_cp15_data_cache_clean_level(level);
    }
  }
}

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_data_cache_test_and_clean(void)
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
ARM_CP15_TEXT_SECTION static inline void
arm_cp15_data_cache_clean_and_invalidate(void)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_data_cache_clean_and_invalidate_line(const void *mva)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_data_cache_clean_and_invalidate_line_by_set_and_way(uint32_t set_and_way)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_data_cache_test_and_clean_and_invalidate(void)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_drain_write_buffer(void)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_wait_for_interrupt(void)
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

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_multiprocessor_affinity(void)
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

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cortex_a9_get_multiprocessor_cpu_id(void)
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

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_auxiliary_control(void)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_auxiliary_control(uint32_t val)
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

ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_processor_feature_1(void)
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

ARM_CP15_TEXT_SECTION static inline void
*arm_cp15_get_vector_base_address(void)
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

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_vector_base_address(void *base)
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

ARM_CP15_TEXT_SECTION static inline void
*arm_cp15_get_hyp_vector_base_address(void)
{
  ARM_SWITCH_REGISTERS;
  void *base;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 4, %[base], c12, c0, 0\n"
    ARM_SWITCH_BACK
    : [base] "=&r" (base) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return base;
}

ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_hyp_vector_base_address(void *base)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 4, %[base], c12, c0, 0\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [base] "r" (base)
  );
}

/* PMCCNTR */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_performance_monitors_cycle_count(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c9, c13, 0\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

/* PMCCNTR */
ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_performance_monitors_cycle_count(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[val], c9, c13, 0\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
  );
}

/* PMCEID0 */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_performance_monitors_common_event_id_0(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c9, c12, 6\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

/* PMCEID1 */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_performance_monitors_common_event_id_1(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c9, c12, 7\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

#define ARM_CP15_PMCLRSET_CYCLE_COUNTER 0x80000000

/* PMCCNTENCLR */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_performance_monitors_count_enable_clear(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c9, c12, 2\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

/* PMCCNTENCLR */
ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_performance_monitors_count_enable_clear(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[val], c9, c12, 2\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
  );
}

/* PMCCNTENSET */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_performance_monitors_count_enable_set(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c9, c12, 1\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

/* PMCCNTENSET */
ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_performance_monitors_count_enable_set(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[val], c9, c12, 1\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
  );
}

#define ARM_CP15_PMCR_IMP(x) ((x) << 24)
#define ARM_CP15_PMCR_IDCODE(x) ((x) << 16)
#define ARM_CP15_PMCR_N(x) ((x) << 11)
#define ARM_CP15_PMCR_DP (1U << 5)
#define ARM_CP15_PMCR_X (1U << 3)
#define ARM_CP15_PMCR_D (1U << 4)
#define ARM_CP15_PMCR_C (1U << 2)
#define ARM_CP15_PMCR_P (1U << 1)
#define ARM_CP15_PMCR_E (1U << 0)

/* PMCR */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_performance_monitors_control(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c9, c12, 0\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

/* PMCR */
ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_performance_monitors_control(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[val], c9, c12, 0\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
  );
}

/* PMINTENCLR */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_performance_monitors_interrupt_enable_clear(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c9, c14, 2\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

/* PMINTENCLR */
ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_performance_monitors_interrupt_enable_clear(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[val], c9, c14, 2\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
  );
}

/* PMINTENSET */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_performance_monitors_interrupt_enable_set(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c9, c14, 1\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

/* PMINTENSET */
ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_performance_monitors_interrupt_enable_set(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[val], c9, c14, 1\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
  );
}

/* PMOVSR */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_performance_monitors_overflow_flag_status(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c9, c12, 3\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

/* PMOVSR */
ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_performance_monitors_overflow_flag_status(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[val], c9, c12, 3\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
  );
}

/* PMOVSSET */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_performance_monitors_overflow_flag_status_set(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c9, c14, 3\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

/* PMOVSSET */
ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_performance_monitors_overflow_flag_status_set(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[val], c9, c14, 3\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
  );
}

/* PMSELR */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_performance_monitors_event_counter_selection(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c9, c12, 5\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

/* PMSELR */
ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_performance_monitors_event_counter_selection(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[val], c9, c12, 5\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
  );
}

/* PMSWINC */
ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_performance_monitors_software_increment(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[val], c9, c12, 4\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
  );
}

/* PMUSERENR */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_performance_monitors_user_enable(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c9, c14, 0\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

/* PMUSERENR */
ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_performance_monitors_user_enable(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[val], c9, c14, 0\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
  );
}

/* PMXEVCNTR */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_performance_monitors_event_count(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c9, c13, 2\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

/* PMXEVCNTR */
ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_performance_monitors_event_count(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[val], c9, c13, 2\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
  );
}

/* PMXEVTYPER */
ARM_CP15_TEXT_SECTION static inline uint32_t
arm_cp15_get_performance_monitors_event_type_select(void)
{
  ARM_SWITCH_REGISTERS;
  uint32_t val;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrc p15, 0, %[val], c9, c13, 1\n"
    ARM_SWITCH_BACK
    : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return val;
}

/* PMXEVTYPER */
ARM_CP15_TEXT_SECTION static inline void
arm_cp15_set_performance_monitors_event_type_select(uint32_t val)
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mcr p15, 0, %[val], c9, c13, 1\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [val] "r" (val)
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
