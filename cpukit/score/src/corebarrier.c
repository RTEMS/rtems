/**
 *  @file
 *
 *  @brief Initialize CORE Barrier
 *  @ingroup ScoreBarrier
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

  _Thread_queue_Initialize( &the_barrier->Wait_queue );
}

Thread_Control *_CORE_barrier_Was_deleted(
  Thread_Control       *the_thread,
  Thread_queue_Queue   *queue,
  Thread_queue_Context *queue_context
)
{
  the_thread->Wait.return_code = CORE_BARRIER_WAS_DELETED;

  return the_thread;
}
