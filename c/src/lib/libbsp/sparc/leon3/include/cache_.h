/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LEON3_CACHE_H
#define LEON3_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

#define CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS

#define CPU_INSTRUCTION_CACHE_ALIGNMENT 64

static inline void _CPU_cache_flush_data_range(
  const void *d_addr,
  size_t n_bytes
)
{
  /* TODO */
}

static inline void _CPU_cache_invalidate_data_range(
  const void *d_addr,
  size_t n_bytes
)
{
  /* TODO */
}

static inline void _CPU_cache_freeze_data(void)
{
  /* TODO */
}

static inline void _CPU_cache_unfreeze_data(void)
{
  /* TODO */
}

static inline void _CPU_cache_invalidate_entire_instruction(void)
{
  __asm__ volatile ("flush");
}

static inline void _CPU_cache_invalidate_instruction_range(
  const void *i_addr,
  size_t n_bytes
)
{
  _CPU_cache_invalidate_entire_instruction();
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
  /* TODO */
}

static inline void _CPU_cache_invalidate_entire_data(void)
{
  /* TODO */
}

static inline void _CPU_cache_enable_data(void)
{
  /* TODO */
}

static inline void _CPU_cache_disable_data(void)
{
  /* TODO */
}

static inline void _CPU_cache_enable_instruction(void)
{
  /* TODO */
}

static inline void _CPU_cache_disable_instruction(void)
{
  /* TODO */
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LEON3_CACHE_H */
