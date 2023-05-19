/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2007, 2014 embedded brains GmbH & Co. KG
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
