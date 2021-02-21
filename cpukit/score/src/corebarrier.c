/**
 * @file
 *
 * @ingroup RTEMSScoreBarrier
 *
 * @brief This source file contains the implementation of
 *   _CORE_barrier_Initialize().
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/corebarrierimpl.h>

void _CORE_barrier_Initialize(
  CORE_barrier_Control *the_barrier,
  uint32_t              maximum_count
)
{
  the_barrier->number_of_waiting_threads = 0;
  the_barrier->maximum_count = maximum_count;

  _Thread_queue_Object_initialize( &the_barrier->Wait_queue );
}
