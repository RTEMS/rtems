/**
 *  @file
 *
 *  LEON3 SMP Obtain CPU Core Number
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/score/cpu.h>

uint32_t _CPU_SMP_Get_current_processor( void )
{
  uint32_t id;
  __asm__ __volatile__( "rd     %%asr17,%0\n\t" : "=r" (id) : );

  return ((id >> 28) & 0xff);
}

