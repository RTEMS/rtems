/*
 * Copyright (c) 2011 embedded brains GmbH
 *
 * Copyright (c) 2006 Kolja Waschk (rtemsdev/ixo.de)
 *
 * COPYRIGHT (c) 1989-2006
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/cpu.h>
#include <rtems/score/interr.h>
#include <rtems/score/nios2-utility.h>

uint32_t _Nios2_ISR_Set_level( uint32_t new_level, uint32_t status )
{
  switch ( _Nios2_ISR_Get_status_mask() ) {
    case NIOS2_ISR_STATUS_MASK_IIC:
      if ( new_level == 0 ) {
        status |= NIOS2_STATUS_PIE;
      } else {
        status &= ~NIOS2_STATUS_PIE;
      }
      break;
    case NIOS2_ISR_STATUS_MASK_EIC_IL:
      status &= ~NIOS2_STATUS_IL_MASK;
      status |= (new_level << NIOS2_STATUS_IL_OFFSET) & NIOS2_STATUS_IL_MASK;
      break;
    case NIOS2_ISR_STATUS_MASK_EIC_RSIE:
      if ( new_level == 0 ) {
        status |= NIOS2_STATUS_RSIE;
      } else {
        status &= ~NIOS2_STATUS_RSIE;
      }
      break;
    default:
      /* FIXME */
      _Internal_error_Occurred( INTERNAL_ERROR_CORE, false, 0xdeadbeef );
      break;
  }

  return status;
}

void _CPU_ISR_Set_level( uint32_t new_level )
{
  uint32_t status = _Nios2_Get_ctlreg_status();

  status = _Nios2_ISR_Set_level( new_level, status );

  _Nios2_Set_ctlreg_status( status );
}
