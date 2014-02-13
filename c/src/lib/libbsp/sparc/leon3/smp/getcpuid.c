/**
 * @file
 * @ingroup sparc_leon3
 * @brief LEON3 SMP Obtain CPU Core Number
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
  return _LEON3_Get_current_processor();
}

