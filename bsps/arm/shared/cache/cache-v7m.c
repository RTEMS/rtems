/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
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
