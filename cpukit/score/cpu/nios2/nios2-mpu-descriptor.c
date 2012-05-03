/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/nios2-utility.h>

static bool _Nios2_Is_power_of_two( uint32_t size )
{
  bool ok = false;
  int i = 0;

  for ( i = 0; !ok && i < 32; ++i ) {
    ok = size == (1U << i);
  }

  return ok;
}

static bool _Nios2_Is_valid_base_and_end(
  const Nios2_MPU_Configuration *config,
  bool data,
  uint32_t base,
  uint32_t end,
  uint32_t *mask_or_limit
)
{
  uint32_t size = end - base;
  uint32_t end_limit = data ?
    (1U << config->data_address_width)
      : (1U << config->instruction_address_width);
  uint32_t mask = data ?
    ((1U << config->data_region_size_log2)) - 1
      : ((1U << config->instruction_region_size_log2)) - 1;
  bool ok = base < end && end <= end_limit
    && (base & mask) == 0 && (end & mask) == 0;

  if ( config->region_uses_limit ) {
    *mask_or_limit = end;
  } else {
    ok = ok && _Nios2_Is_power_of_two( size );
    *mask_or_limit = (~(size - 1)) & NIOS2_MPUACC_MASK_MASK;
  }

  return ok;
}

static bool _Nios2_Is_valid_permission(
  bool data,
  int perm
)
{
  int max = data ? 6 : 2;

  return 0 <= perm && perm <= max && (!data || (data && perm != 3));
}

bool _Nios2_MPU_Setup_region_registers(
  const Nios2_MPU_Configuration *config,
  const Nios2_MPU_Region_descriptor *desc,
  uint32_t *mpubase,
  uint32_t *mpuacc
)
{
  uint32_t base = (uint32_t) desc->base;
  uint32_t end = (uint32_t) desc->end;
  uint32_t mask_or_limit = 0;
  bool is_valid_base_and_end = _Nios2_Is_valid_base_and_end(
    config,
    desc->data,
    base,
    end,
    &mask_or_limit
  );
  bool ok = is_valid_base_and_end
    && _Nios2_MPU_Is_valid_index( config, desc->index, desc->data )
    && _Nios2_Is_valid_permission( desc->data, desc->perm )
    && !(!desc->data && desc->cacheable)
    && !(desc->read && desc->write);

  if ( ok ) {
    *mpubase = (base & NIOS2_MPUBASE_BASE_MASK)
      | ((desc->index << NIOS2_MPUBASE_INDEX_OFFSET) & NIOS2_MPUBASE_INDEX_MASK)
      | (desc->data ? NIOS2_MPUBASE_D : 0);
    *mpuacc = mask_or_limit
      | (desc->cacheable ? NIOS2_MPUACC_C : 0)
      | ((desc->perm << NIOS2_MPUACC_PERM_OFFSET) & NIOS2_MPUACC_PERM_MASK)
      | (desc->read ? NIOS2_MPUACC_RD : 0)
      | (desc->write ? NIOS2_MPUACC_WR : 0);
  }

  return ok;
}

bool _Nios2_MPU_Get_region_descriptor(
  const Nios2_MPU_Configuration *config,
  int index,
  bool data,
  Nios2_MPU_Region_descriptor *desc
)
{
  bool ok = _Nios2_MPU_Is_valid_index( config, index, data );

  if ( ok ) {
    uint32_t mpubase;
    uint32_t mpuacc;

    _Nios2_MPU_Get_region_registers( index, data, &mpubase, &mpuacc );

    desc->index = index;
    desc->base = (void *) (mpubase & NIOS2_MPUBASE_BASE_MASK);
    if ( config->region_uses_limit ) {
      desc->end = (void *) (mpuacc & NIOS2_MPUACC_LIMIT_MASK);
    } else {
      desc->end = (void *) ((mpuacc & NIOS2_MPUACC_MASK_MASK) + 1);
    }
    desc->perm = (mpuacc & NIOS2_MPUACC_PERM_MASK) >> NIOS2_MPUACC_PERM_OFFSET;
    desc->data = data;
    desc->cacheable = (mpuacc & NIOS2_MPUACC_C) != 0;
    desc->read = (mpuacc & NIOS2_MPUACC_RD) != 0;
    desc->write = (mpuacc & NIOS2_MPUACC_WR) != 0;
  }

  return ok;
}
