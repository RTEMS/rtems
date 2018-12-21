/**
 * @file
 *
 * @ingroup arm
 *
 * @brief ARM cache defines and implementation.
 */

/*
 * Copyright (C) 2009, 2018 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <libcpu/arm-cp15.h>

#include "cache-cp15.h"

#define CPU_DATA_CACHE_ALIGNMENT 32

#define CPU_INSTRUCTION_CACHE_ALIGNMENT 32

#if defined(__ARM_ARCH_7A__)
/* Some/many ARM Cortex-A cores have L1 data line length 64 bytes */
#define CPU_MAXIMAL_CACHE_ALIGNMENT 64
#endif

#define CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS

#define CPU_CACHE_SUPPORT_PROVIDES_CACHE_SIZE_FUNCTIONS

#if __ARM_ARCH >= 7 && (__ARM_ARCH_PROFILE == 65 || __ARM_ARCH_PROFILE == 82)
#define CPU_CACHE_SUPPORT_PROVIDES_DISABLE_DATA
#endif

static inline void _CPU_cache_flush_1_data_line(const void *d_addr)
{
  arm_cache_l1_flush_1_data_line(d_addr);
}

static inline void
_CPU_cache_flush_data_range(
  const void *d_addr,
  size_t      n_bytes
)
{
  _ARM_Data_synchronization_barrier();
  arm_cache_l1_flush_data_range(
    d_addr,
    n_bytes
  );
  #if !defined(__ARM_ARCH_7A__)
  arm_cp15_drain_write_buffer();
  #endif
 _ARM_Data_synchronization_barrier();
}

static inline void _CPU_cache_invalidate_1_data_line(const void *d_addr)
{
  arm_cache_l1_invalidate_1_data_line(d_addr);
}

static inline void
_CPU_cache_invalidate_data_range(
  const void *addr_first,
  size_t     n_bytes
)
{
  arm_cache_l1_invalidate_data_range(
    addr_first,
    n_bytes
  );
}

static inline void _CPU_cache_freeze_data(void)
{
  /* TODO */
}

static inline void _CPU_cache_unfreeze_data(void)
{
  /* TODO */
}

static inline void _CPU_cache_invalidate_1_instruction_line(const void *d_addr)
{
  arm_cache_l1_invalidate_1_instruction_line(d_addr);
}

static inline void
_CPU_cache_invalidate_instruction_range( const void *i_addr, size_t n_bytes)
{
  arm_cache_l1_invalidate_instruction_range( i_addr, n_bytes );
  _ARM_Instruction_synchronization_barrier();
}

static inline void _CPU_cache_freeze_instruction(void)
{
  /* TODO */
}

static inline void _CPU_cache_unfreeze_instruction(void)
{
  /* TODO */
}

static inline void _CPU_cache_flush_entire_data(void)
{
  _ARM_Data_synchronization_barrier();
  #if defined(__ARM_ARCH_7A__)
  arm_cp15_data_cache_clean_all_levels();
  #else
  arm_cp15_data_cache_clean_and_invalidate();
  arm_cp15_drain_write_buffer();
  #endif
  _ARM_Data_synchronization_barrier();
}

static inline void _CPU_cache_invalidate_entire_data(void)
{
  #if defined(__ARM_ARCH_7A__)
  arm_cp15_data_cache_invalidate_all_levels();
  #else
  arm_cp15_data_cache_invalidate();
  #endif
}

static inline void _CPU_cache_enable_data(void)
{
  rtems_interrupt_level level;
  uint32_t ctrl;

  rtems_interrupt_local_disable(level);
  ctrl = arm_cp15_get_control();
  ctrl |= ARM_CP15_CTRL_C;
  arm_cp15_set_control(ctrl);
  rtems_interrupt_local_enable(level);
}

static inline void _CPU_cache_disable_data(void)
{
  rtems_interrupt_level level;
  uint32_t ctrl;

  rtems_interrupt_local_disable(level);
  arm_cp15_data_cache_test_and_clean_and_invalidate();
  ctrl = arm_cp15_get_control();
  ctrl &= ~ARM_CP15_CTRL_C;
  arm_cp15_set_control(ctrl);
  rtems_interrupt_local_enable(level);
}

static inline void _CPU_cache_invalidate_entire_instruction(void)
{
  arm_cache_l1_invalidate_entire_instruction();
  _ARM_Instruction_synchronization_barrier();
}

static inline void _CPU_cache_enable_instruction(void)
{
  rtems_interrupt_level level;
  uint32_t ctrl;

  rtems_interrupt_local_disable(level);
  ctrl = arm_cp15_get_control();
  ctrl |= ARM_CP15_CTRL_I;
  arm_cp15_set_control(ctrl);
  rtems_interrupt_local_enable(level);
}

static inline void _CPU_cache_disable_instruction(void)
{
  rtems_interrupt_level level;
  uint32_t ctrl;

  rtems_interrupt_local_disable(level);
  ctrl = arm_cp15_get_control();
  ctrl &= ~ARM_CP15_CTRL_I;
  arm_cp15_set_control(ctrl);
  rtems_interrupt_local_enable(level);
}

static inline size_t arm_cp15_get_cache_size(
  uint32_t level,
  uint32_t which
)
{
  uint32_t clidr;
  uint32_t loc;
  uint32_t ccsidr;

  clidr = arm_cp15_get_cache_level_id();
  loc = arm_clidr_get_level_of_coherency(clidr);

  if (level >= loc) {
    return 0;
  }

  if (level == 0) {
    level = loc - 1;
  }

  ccsidr = arm_cp15_get_cache_size_id_for_level(
    ARM_CP15_CACHE_CSS_LEVEL(level) | which
  );

  return (1U << arm_ccsidr_get_line_power(ccsidr))
    * arm_ccsidr_get_associativity(ccsidr)
    * arm_ccsidr_get_num_sets(ccsidr);
}

static inline size_t _CPU_cache_get_data_cache_size(uint32_t level)
{
  return arm_cp15_get_cache_size(level, ARM_CP15_CACHE_CSS_ID_DATA);
}

static inline size_t _CPU_cache_get_instruction_cache_size(uint32_t level)
{
  return arm_cp15_get_cache_size(level, ARM_CP15_CACHE_CSS_ID_INSTRUCTION);
}

#include "../../shared/cache/cacheimpl.h"
