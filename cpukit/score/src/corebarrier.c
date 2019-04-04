/**
 *  @file
 *
 *  @brief Initialize CORE Barrier
 *  @ingroup RTEMSScoreBarrier
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/corebarrierimpl.h>

void _CORE_barrier_Initialize(
  CORE_barrier_Control       *the_barrier,
  CORE_barrier_Attributes    *the_barrier_attributes
)
{

  the_barrier->Attributes                = *the_barrier_attributes;
  the_barrier->number_of_waiting_threads = 0;

  _Thread_queue_Object_initialize( &the_barrier->Wait_queue );
}
