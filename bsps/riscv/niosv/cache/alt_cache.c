/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2024 Kevin Kirspel
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

#include <stdint.h>
#include <stddef.h>
#include <bsp_system.h>
#include <alt_cache.h>

#define CPU_DATA_CACHE_ALIGNMENT ALT_CPU_DCACHE_LINE_SIZE

#define CPU_INSTRUCTION_CACHE_ALIGNMENT ALT_CPU_ICACHE_LINE_SIZE

#define CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS

#define CPU_CACHE_SUPPORT_PROVIDES_CACHE_SIZE_FUNCTIONS

static inline void _CPU_cache_flush_1_data_line(const void *d_addr)
{
  alt_dcache_flush(d_addr, ALT_CPU_DCACHE_LINE_SIZE);
}

static inline void
_CPU_cache_flush_data_range(
  const void *d_addr,
  size_t      n_bytes
)
{
  alt_dcache_flush(d_addr, n_bytes);
}

static inline void _CPU_cache_invalidate_1_data_line(const void *d_addr)
{
  alt_dcache_flush_no_writeback(d_addr, ALT_CPU_DCACHE_LINE_SIZE);
}

static inline void
_CPU_cache_invalidate_data_range(
  const void *d_addr,
  size_t     n_bytes
)
{
  alt_dcache_flush_no_writeback(d_addr, n_bytes);
}

static inline void _CPU_cache_freeze_data(void)
{
  /* TODO */
}

static inline void _CPU_cache_unfreeze_data(void)
{
  /* TODO */
}

static inline void _CPU_cache_invalidate_1_instruction_line(const void *i_addr)
{
  alt_icache_flush(i_addr, ALT_CPU_ICACHE_LINE_SIZE);
}

static inline void
_CPU_cache_invalidate_instruction_range( const void *i_addr, size_t n_bytes)
{
  alt_icache_flush(i_addr, n_bytes);
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
  alt_dcache_flush_all();
}

static inline void _CPU_cache_invalidate_entire_data(void)
{
  alt_dcache_flush_all();
}

static inline void _CPU_cache_enable_data(void)
{
}

static inline void _CPU_cache_disable_data(void)
{
}

static inline void _CPU_cache_invalidate_entire_instruction(void)
{
  alt_icache_flush_all();
}

static inline void _CPU_cache_enable_instruction(void)
{
}

static inline void _CPU_cache_disable_instruction(void)
{
}

static inline size_t _CPU_cache_get_data_cache_size(uint32_t level)
{
  (void) level;

  return ALT_CPU_DCACHE_SIZE;
}

static inline size_t _CPU_cache_get_instruction_cache_size(uint32_t level)
{
  (void) level;

  return ALT_CPU_ICACHE_SIZE;
}

#include "../../shared/cache/cacheimpl.h"
