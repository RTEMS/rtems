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

void _Nios2_MPU_Reset( const Nios2_MPU_Configuration *config )
{
  uint32_t data_mpubase = (1U << config->data_region_size_log2)
    | NIOS2_MPUBASE_D;
  uint32_t inst_mpubase = 1U << config->instruction_region_size_log2;
  uint32_t mpuacc = NIOS2_MPUACC_WR;
  int data_count = config->data_region_count;
  int inst_count = config->instruction_region_count;
  int i = 0;

  _Nios2_MPU_Disable();

  for ( i = 0; i < data_count; ++i ) {
    uint32_t index = ((uint32_t) i) << NIOS2_MPUBASE_INDEX_OFFSET;

    _Nios2_Set_ctlreg_mpubase( data_mpubase | index );
    _Nios2_Set_ctlreg_mpuacc( mpuacc );
  }

  for ( i = 0; i < inst_count; ++i ) {
    uint32_t index = ((uint32_t) i) << NIOS2_MPUBASE_INDEX_OFFSET;

    _Nios2_Set_ctlreg_mpubase( inst_mpubase | index );
    _Nios2_Set_ctlreg_mpuacc( mpuacc );
  }
}
