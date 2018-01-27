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
 * http://www.rtems.org/license/LICENSE.
 */

#include <amba.h>
#include <leon.h>

#define CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS

#define CPU_CACHE_SUPPORT_PROVIDES_CACHE_SIZE_FUNCTIONS

#define CPU_CACHE_NO_INSTRUCTION_CACHE_SNOOPING

#define CPU_INSTRUCTION_CACHE_ALIGNMENT 64

#define CPU_DATA_CACHE_ALIGNMENT 64

static inline volatile struct l2c_regs *get_l2c_regs(void)
{
  volatile struct l2c_regs *l2c = NULL;
  struct ambapp_dev *adev;

  adev = (void *) ambapp_for_each(
    &ambapp_plb,
    OPTIONS_ALL | OPTIONS_AHB_SLVS,
    VENDOR_GAISLER,
    GAISLER_L2CACHE,
    ambapp_find_by_idx,
    NULL
  );
  if (adev != NULL) {
    l2c = (volatile struct l2c_regs *) DEV_TO_AHB(adev)->start[1];
  }

  return l2c;
}

static inline size_t get_l2_size(void)
{
  size_t size = 0;
  volatile struct l2c_regs *l2c = get_l2c_regs();

  if (l2c != NULL) {
    unsigned status = l2c->status;
    unsigned ways = (status & 0x3) + 1;
    unsigned set_size = ((status & 0x7ff) >> 2) * 1024;

    size = ways * set_size;
  }

  return size;
}

static inline size_t get_l1_size(uint32_t l1_cfg)
{
  uint32_t ways = ((l1_cfg >> 24) & 0x7) + 1;
  uint32_t wsize = UINT32_C(1) << (((l1_cfg >> 20) & 0xf) + 10);

  return ways * wsize;
}

static inline size_t get_max_size(size_t a, size_t b)
{
  return a < b ? b : a;
}

static inline size_t get_cache_size(uint32_t level, uint32_t l1_cfg)
{
  size_t size;

  switch (level) {
    case 0:
      size = get_max_size(get_l1_size(l1_cfg), get_l2_size());
      break;
    case 1:
      size = get_l1_size(l1_cfg);
      break;
    case 2:
      size = get_l2_size();
      break;
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
  uint32_t cache_reg = leon3_get_cache_control_register();

  cache_reg |= LEON3_REG_CACHE_CTRL_FI;
  leon3_set_cache_control_register(cache_reg);
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
