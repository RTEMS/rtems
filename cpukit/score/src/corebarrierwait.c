/**
 *  @file
 *
 *  @brief Wait For The Barrier
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
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadimpl.h>

Status_Control _CORE_barrier_Seize(
  CORE_barrier_Control *the_barrier,
  Thread_Control       *executing,
  bool                  wait,
  Thread_queue_Context *queue_context
)
{
  uint32_t number_of_waiting_threads;

  _CORE_barrier_Acquire_critical( the_barrier, queue_context );

  number_of_waiting_threads = the_barrier->number_of_waiting_threads;
  ++number_of_waiting_threads;

  if (
    _CORE_barrier_Is_automatic( &the_barrier->Attributes )
      && number_of_waiting_threads == the_barrier->Attributes.maximum_count
  ) {
    _CORE_barrier_Surrender( the_barrier, queue_context );
    return STATUS_BARRIER_AUTOMATICALLY_RELEASED;
  } else {
    the_barrier->number_of_waiting_threads = number_of_waiting_threads;
    _Thread_queue_Context_set_thread_state(
      queue_context,
      STATES_WAITING_FOR_BARRIER
    );
    _Thread_queue_Enqueue(
      &the_barrier->Wait_queue.Queue,
      CORE_BARRIER_TQ_OPERATIONS,
      executing,
      queue_context
    );
    return _Thread_Wait_get_status( executing );
  }
}
