/*
 *  Copyright (c) 2011 embedded brains GmbH
 *
 *  Copyright (c) 2006 Kolja Waschk (rtemsdev/ixo.de)
 *
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/cpu.h>
#include <rtems/score/nios2-utility.h>

void _CPU_Fatal_halt( uint32_t _source, uint32_t _error )
{
  /* write 0 to status register (disable interrupts) */
  __builtin_wrctl( NIOS2_CTLREG_INDEX_STATUS, 0 );

  /* write error code to ET register */
  __asm__ volatile ("mov et, %z0" : : "rM" (_error));

  while (1);
}
