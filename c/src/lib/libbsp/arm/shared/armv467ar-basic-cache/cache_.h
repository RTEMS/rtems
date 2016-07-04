/**
 * @file
 *
 * @ingroup arm
 *
 * @brief ARM cache defines and implementation.
 */

/*
 * Copyright (c) 2009-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_ARMV467AR_BASIC_CACHE_H
#define LIBBSP_ARM_ARMV467AR_BASIC_CACHE_H

#include <libcpu/arm-cp15.h>
#include "../include/arm-cache-l1.h"

#define CPU_DATA_CACHE_ALIGNMENT 32
#define CPU_INSTRUCTION_CACHE_ALIGNMENT 32
#if defined(__ARM_ARCH_7A__)
/* Some/many ARM Cortex-A cores have L1 data line lenght 64 bytes */
#define CPU_MAXIMAL_CACHE_ALIGNMENT 64
#endif

#define CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS \
          ARM_CACHE_L1_CPU_SUPPORT_PROVIDES_RANGE_FUNCTIONS


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
  arm_cp15_drain_write_buffer();
  arm_cache_l1_flush_data_range(
    d_addr,
    n_bytes
  );
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
  arm_cp15_data_cache_test_and_clean();
}

static inline void _CPU_cache_invalidate_entire_data(void)
{
  arm_cp15_data_cache_invalidate();
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
  arm_cp15_instruction_cache_invalidate();
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

#endif /* LIBBSP_ARM_ARMV467AR_BASIC_CACHE_H */
