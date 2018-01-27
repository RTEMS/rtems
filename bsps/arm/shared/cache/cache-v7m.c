/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <chip.h>

#define CPU_DATA_CACHE_ALIGNMENT 32

#define CPU_INSTRUCTION_CACHE_ALIGNMENT 32

#define CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS

static inline void _CPU_cache_flush_data_range(
  const void *d_addr,
  size_t n_bytes
)
{
  SCB_CleanInvalidateDCache_by_Addr(
    RTEMS_DECONST(uint32_t *, (const uint32_t *) d_addr),
    n_bytes
  );
}

static inline void _CPU_cache_invalidate_data_range(
  const void *d_addr,
  size_t n_bytes
)
{
  SCB_InvalidateDCache_by_Addr(
    RTEMS_DECONST(uint32_t *, (const uint32_t *) d_addr),
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

static inline void _CPU_cache_invalidate_instruction_range(
  const void *i_addr,
  size_t n_bytes
)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  SCB_InvalidateICache();
  rtems_interrupt_enable(level);
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
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  SCB_CleanDCache();
  rtems_interrupt_enable(level);
}

static inline void _CPU_cache_invalidate_entire_data(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  SCB_InvalidateDCache();
  rtems_interrupt_enable(level);
}

static inline void _CPU_cache_enable_data(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  SCB_EnableDCache();
  rtems_interrupt_enable(level);
}

static inline void _CPU_cache_disable_data(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  SCB_DisableDCache();
  rtems_interrupt_enable(level);
}

static inline void _CPU_cache_invalidate_entire_instruction(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  SCB_InvalidateICache();
  rtems_interrupt_enable(level);
}

static inline void _CPU_cache_enable_instruction(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  SCB_EnableICache();
  rtems_interrupt_enable(level);
}

static inline void _CPU_cache_disable_instruction(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  SCB_DisableICache();
  rtems_interrupt_enable(level);
}

#include "../../shared/cache/cacheimpl.h"
