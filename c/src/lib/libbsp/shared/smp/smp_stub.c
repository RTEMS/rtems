/*
 *  RTEMS SMP Support for Single Core
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/cpu.h>

uint32_t _CPU_SMP_Initialize( uint32_t configured_cpu_count )
{
  /* return the number of CPUs */
  return 1;
}

void _CPU_SMP_Send_interrupt( uint32_t target_processor_index )
{
}
