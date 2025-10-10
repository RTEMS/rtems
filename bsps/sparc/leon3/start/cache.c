/**
 * @file
 *
 * @ingroup RTEMSBSPsSPARCLEON3
 *
 * @brief This source file contains the implementation of the Cache Manager.
 */

/*
 * Copyright (C) 2014 embedded brains GmbH & Co. KG
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

#include <grlib/l2cache-regs.h>
#include <grlib/io.h>

#include <bsp/leon3.h>

#if !defined(LEON3_L2CACHE_BASE)
#include <grlib/ambapp.h>
#endif

#if !defined(LEON3_L2CACHE_BASE) || LEON3_L2CACHE_BASE != 0
#define LEON3_MAYBE_HAS_L2CACHE
#endif

#define CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS

#define CPU_CACHE_SUPPORT_PROVIDES_CACHE_SIZE_FUNCTIONS

#define CPU_CACHE_NO_INSTRUCTION_CACHE_SNOOPING

#define CPU_INSTRUCTION_CACHE_ALIGNMENT 64

#define CPU_DATA_CACHE_ALIGNMENT 64

#if RTEMS_INTERRUPT_LOCK_NEEDS_OBJECT
rtems_interrupt_lock leon3_l2c_lock =
  RTEMS_INTERRUPT_LOCK_INITIALIZER( "LEON3 L2C" );
#endif

static inline uint32_t l2c_load_32(const volatile uint32_t *address)
{
  rtems_interrupt_lock_context lock_context;
  uint32_t value;

  rtems_interrupt_lock_acquire(&leon3_l2c_lock, &lock_context);
  value = grlib_load_32(address);
  rtems_interrupt_lock_release(&leon3_l2c_lock, &lock_context);

  return value;
}

#if !defined(LEON3_L2CACHE_BASE)
static inline l2cache *get_l2c_regs(void)
{
  struct ambapp_dev *adev;

  adev = (struct ambapp_dev *) ambapp_for_each(
    ambapp_plb(),
    OPTIONS_ALL | OPTIONS_AHB_SLVS,
    VENDOR_GAISLER,
    GAISLER_L2CACHE,
    ambapp_find_by_idx,
    NULL
  );

  if (adev == NULL) {
    return NULL;
  }

  return (l2cache *) DEV_TO_AHB(adev)->start[1];
}
#endif

static inline size_t get_l1_size(uint32_t l1_cfg)
{
  uint32_t ways = ((l1_cfg >> 24) & 0x7) + 1;
  uint32_t wsize = UINT32_C(1) << (((l1_cfg >> 20) & 0xf) + 10);

  return ways * wsize;
}

#if defined(LEON3_MAYBE_HAS_L2CACHE)
static inline size_t get_l2_size(void)
{
  l2cache *regs;
  unsigned status;
  unsigned ways;
  unsigned set_size;

#if defined(LEON3_L2CACHE_BASE)
  regs = (l2cache *) LEON3_L2CACHE_BASE;
#else
  regs = get_l2c_regs();

  if (regs == NULL) {
    return 0;
  }
#endif

  status = l2c_load_32(&regs->l2cs);
  ways = L2CACHE_L2CS_WAY_GET(status) + 1;
  set_size = L2CACHE_L2CS_WAY_SIZE_GET(status) * 1024;

  return ways * set_size;
}

static inline size_t get_max_size(size_t a, size_t b)
{
  return a < b ? b : a;
}
#endif

static inline size_t get_cache_size(uint32_t level, uint32_t l1_cfg)
{
  size_t size;

  switch (level) {
    case 0:
#if defined(LEON3_MAYBE_HAS_L2CACHE)
      size = get_max_size(get_l1_size(l1_cfg), get_l2_size());
#else
      size = get_l1_size(l1_cfg);
#endif
      break;
    case 1:
      size = get_l1_size(l1_cfg);
      break;
#if defined(LEON3_MAYBE_HAS_L2CACHE)
    case 2:
      size = get_l2_size();
      break;
#endif
    default:
      size = 0;
      break;
  }

  return size;
}

static inline size_t _CPU_cache_get_data_cache_size(uint32_t level)
{
  return get_cache_size(level, leon3_get_data_cache_config_register());
}

static inline void _CPU_cache_flush_data_range(
  const void *d_addr,
  size_t n_bytes
)
{
  (void) d_addr;
  (void) n_bytes;

  /* TODO */
}

static inline void _CPU_cache_invalidate_data_range(
  const void *d_addr,
  size_t n_bytes
)
{
  (void) d_addr;
  (void) n_bytes;

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
  uint32_t cache_reg = leon3_get_cache_control_register();

  cache_reg |= LEON3_REG_CACHE_CTRL_FI;
  leon3_set_cache_control_register(cache_reg);
}

static inline void _CPU_cache_invalidate_instruction_range(
  const void *i_addr,
  size_t n_bytes
)
{
  (void) i_addr;
  (void) n_bytes;

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

static inline size_t _CPU_cache_get_instruction_cache_size( uint32_t level )
{
  return get_cache_size(level, leon3_get_inst_cache_config_register());
}

static inline void _CPU_cache_enable_instruction(void)
{
  /* TODO */
}

static inline void _CPU_cache_disable_instruction(void)
{
  /* TODO */
}

#include "../../../shared/cache/cacheimpl.h"
