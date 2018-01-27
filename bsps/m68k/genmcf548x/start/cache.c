/*
 * Copyright (c) 2007-2014 embedded brains GmbH.  All rights reserved.
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

#include <bsp.h>

#define CPU_DATA_CACHE_ALIGNMENT 16

#define CPU_INSTRUCTION_CACHE_ALIGNMENT 16

/*
 * There is no complete cache lock (only 2 ways of 4 can be locked)
 */
static inline void _CPU_cache_freeze_data(void)
{
  /* Do nothing */
}

static inline void _CPU_cache_unfreeze_data(void)
{
  /* Do nothing */
}

static inline void _CPU_cache_freeze_instruction(void)
{
  /* Do nothing */
}

static inline void _CPU_cache_unfreeze_instruction(void)
{
  /* Do nothing */
}

static inline void _CPU_cache_enable_instruction(void)
{
  bsp_cacr_clear_flags( MCF548X_CACR_IDCM);
}

static inline void _CPU_cache_disable_instruction(void)
{
  bsp_cacr_set_flags( MCF548X_CACR_IDCM);
}

static inline void _CPU_cache_invalidate_entire_instruction(void)
{
  bsp_cacr_set_self_clear_flags( MCF548X_CACR_ICINVA);
}

static inline void _CPU_cache_invalidate_1_instruction_line(const void *addr)
{
  uint32_t a = (uint32_t) addr & ~0x3;

  __asm__ volatile ("cpushl %%ic,(%0)" :: "a" (a | 0x0));
  __asm__ volatile ("cpushl %%ic,(%0)" :: "a" (a | 0x1));
  __asm__ volatile ("cpushl %%ic,(%0)" :: "a" (a | 0x2));
  __asm__ volatile ("cpushl %%ic,(%0)" :: "a" (a | 0x3));
}

static inline void _CPU_cache_enable_data(void)
{
  bsp_cacr_clear_flags( MCF548X_CACR_DDCM( DCACHE_OFF_IMPRECISE));
}

static inline void _CPU_cache_disable_data(void)
{
  bsp_cacr_set_flags( MCF548X_CACR_DDCM( DCACHE_OFF_IMPRECISE));
}

static inline void _CPU_cache_invalidate_entire_data(void)
{
  bsp_cacr_set_self_clear_flags( MCF548X_CACR_DCINVA);
}

static inline void _CPU_cache_invalidate_1_data_line( const void *addr)
{
  uint32_t a = (uint32_t) addr & ~0x3;

  __asm__ volatile ("cpushl %%dc,(%0)" :: "a" (a | 0x0));
  __asm__ volatile ("cpushl %%dc,(%0)" :: "a" (a | 0x1));
  __asm__ volatile ("cpushl %%dc,(%0)" :: "a" (a | 0x2));
  __asm__ volatile ("cpushl %%dc,(%0)" :: "a" (a | 0x3));
}

static inline void _CPU_cache_flush_1_data_line( const void *addr)
{
  uint32_t a = (uint32_t) addr & ~0x3;

  __asm__ volatile ("cpushl %%dc,(%0)" :: "a" (a | 0x0));
  __asm__ volatile ("cpushl %%dc,(%0)" :: "a" (a | 0x1));
  __asm__ volatile ("cpushl %%dc,(%0)" :: "a" (a | 0x2));
  __asm__ volatile ("cpushl %%dc,(%0)" :: "a" (a | 0x3));
}

static inline void _CPU_cache_flush_entire_data( void)
{
  uint32_t line = 0;

  for (line = 0; line < 512; ++line) {
    _CPU_cache_flush_1_data_line( (const void *) (line * 16));
  }
}

#include "../../../shared/cache/cacheimpl.h"
