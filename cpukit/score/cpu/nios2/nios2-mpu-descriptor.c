/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief NIOS2 MPU Descriptor
 */

/*
 * Copyright (c) 2011 embedded brains GmbH & Co. KG
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
