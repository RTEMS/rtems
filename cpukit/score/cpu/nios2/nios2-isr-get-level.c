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

uint32_t _CPU_ISR_Get_level( void )
{
  uint32_t status = _Nios2_Get_ctlreg_status();
  uint32_t level = 0;

  switch ( _Nios2_ISR_Get_status_mask() ) {
    case NIOS2_ISR_STATUS_MASK_IIC:
      level = (status & NIOS2_STATUS_PIE) == 0;
      break;
    case NIOS2_ISR_STATUS_MASK_EIC_IL:
      level = (status & NIOS2_STATUS_IL_MASK) >> NIOS2_STATUS_IL_OFFSET;
      break;
    case NIOS2_ISR_STATUS_MASK_EIC_RSIE:
      level = (status & NIOS2_STATUS_RSIE) == 0;
      break;
    default:
      /* FIXME */
      _Internal_error_Occurred( INTERNAL_ERROR_CORE, false, 0xdeadbeef );
      break;
  }

  return level;
}
