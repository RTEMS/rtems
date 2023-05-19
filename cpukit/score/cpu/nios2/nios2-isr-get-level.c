/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2011, 2016 embedded brains GmbH & Co. KG
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

bool _CPU_ISR_Is_enabled( uint32_t level )
{
  switch ( _Nios2_ISR_Get_status_mask() ) {
    case NIOS2_ISR_STATUS_MASK_EIC_IL:
      return ((level & NIOS2_STATUS_IL_MASK) >> NIOS2_STATUS_IL_OFFSET) == 0;
    case NIOS2_ISR_STATUS_MASK_EIC_RSIE:
      return (level & NIOS2_STATUS_RSIE) != 0;
    default:
      return (level & NIOS2_STATUS_PIE) != 0;
  }
}

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
      _Terminate( INTERNAL_ERROR_CORE, 0xdeadbeef );
      break;
  }

  return level;
}
