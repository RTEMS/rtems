/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup arm
 *
 * @brief ARM cache defines and implementation.
 */

/*
 * Copyright (C) 2009, 2018 embedded brains GmbH & Co. KG
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

#include <libcpu/arm-cp15.h>

#include "cache-cp15.h"

#define CPU_DATA_CACHE_ALIGNMENT 32

#define CPU_INSTRUCTION_CACHE_ALIGNMENT 32

#ifdef ARM_MULTILIB_CACHE_LINE_MAX_64
  #define CPU_MAXIMAL_CACHE_ALIGNMENT 64
#endif

#define CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS

#define CPU_CACHE_SUPPORT_PROVIDES_CACHE_SIZE_FUNCTIONS

#if __ARM_ARCH >= 7
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
#if __ARM_ARCH < 7
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
#if __ARM_ARCH >= 7
  arm_cp15_data_cache_clean_all_levels();
#else
  arm_cp15_data_cache_clean_and_invalidate();
  arm_cp15_drain_write_buffer();
#endif
  _ARM_Data_synchronization_barrier();
}

static inline void _CPU_cache_invalidate_entire_data(void)
{
#if __ARM_ARCH >= 7
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

#if __ARM_ARCH >= 6
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
#else
static inline size_t _CPU_cache_get_data_cache_size(uint32_t level)
{
  uint32_t cache_type;

  if (level > 0) {
    return 0;
  }

  cache_type = arm_cp15_get_cache_type();
  return 1U << (((cache_type >> (12 + 6)) & 0xf) + 9);
}

static inline size_t _CPU_cache_get_instruction_cache_size(uint32_t level)
{
  uint32_t cache_type;

  if (level > 0) {
    return 0;
  }

  cache_type = arm_cp15_get_cache_type();
  return 1U << (((cache_type >> (0 + 6)) & 0xf) + 9);
}
#endif

#include "../../shared/cache/cacheimpl.h"
