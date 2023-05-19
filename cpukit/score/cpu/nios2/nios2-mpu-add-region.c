/* SPDX-License-Identifier: BSD-2-Clause */

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

static bool _Nios2_MPU_Is_region_disabled(
  const Nios2_MPU_Configuration *config,
  uint32_t mpubase,
  uint32_t mpuacc
)
{
  bool disabled = false;

  if ( config->region_uses_limit ) {
    disabled = (mpubase & NIOS2_MPUBASE_BASE_MASK)
      > (mpuacc & NIOS2_MPUACC_LIMIT_MASK);
  } else {
    disabled = (mpuacc & NIOS2_MPUACC_MASK_MASK) == 0;
  }

  return disabled;
}

int _Nios2_MPU_Get_disabled_region_index(
  const Nios2_MPU_Configuration *config,
  bool data,
  int begin,
  int end
)
{
  int index = -1;
  int count = _Nios2_MPU_Get_region_count( config, data );

  if ( end < 0 || count < end ) {
    end = count;
  }

  if ( begin >= 0 ) {
    int i = 0;

    for ( i = begin; i < end && index < 0; ++i ) {
      uint32_t mpubase = 0;
      uint32_t mpuacc = 0;

      _Nios2_MPU_Get_region_registers( i, data, &mpubase, &mpuacc );

      if ( _Nios2_MPU_Is_region_disabled( config, mpubase, mpuacc ) ) {
        index = i;
      }
    }
  }

  return index;
}

bool _Nios2_MPU_Add_region(
  const Nios2_MPU_Configuration *config,
  const Nios2_MPU_Region_descriptor *desc,
  bool force
)
{
  bool ok = true;
  int index = desc->index;
  bool data = desc->data;
  uint32_t mpubase = 0;
  uint32_t mpuacc = 0;

  if ( _Nios2_MPU_Is_valid_index( config, index, data ) ) {
    if ( !force ) {
      _Nios2_MPU_Get_region_registers( index, data, &mpubase, &mpuacc );
      ok = _Nios2_MPU_Is_region_disabled( config, mpubase, mpuacc );
    }

    if ( ok ) {
      ok = _Nios2_MPU_Setup_region_registers(
        config,
        desc,
        &mpubase,
        &mpuacc
      );
      if ( ok ) {
        _Nios2_Set_ctlreg_mpubase(mpubase);
        _Nios2_Set_ctlreg_mpuacc(mpuacc);
      }
    }
  } else {
    ok = false;
  }

  return ok;
}
