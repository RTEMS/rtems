/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief NIOS2 ISR Set Level
 */

/*
 * Copyright (c) 2011 embedded brains GmbH & Co. KG
 *
 * Copyright (c) 2006 Kolja Waschk (rtemsdev/ixo.de)
 *
 * COPYRIGHT (c) 1989-2006
 * On-Line Applications Research Corporation (OAR).
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
      _Terminate( INTERNAL_ERROR_CORE, 0xdeadbeef );
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
