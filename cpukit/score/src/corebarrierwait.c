/**
 *  @file
 *
 *  @brief Wait For The Barrier
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
#include <rtems/score/statesimpl.h>

void _CORE_barrier_Do_seize(
  CORE_barrier_Control    *the_barrier,
  Thread_Control          *executing,
  bool                     wait,
  Watchdog_Interval        timeout,
#if defined(RTEMS_MULTIPROCESSING)
  Thread_queue_MP_callout  mp_callout,
#endif
  ISR_lock_Context        *lock_context
)
{
  uint32_t number_of_waiting_threads;

  executing->Wait.return_code = CORE_BARRIER_STATUS_SUCCESSFUL;

  _CORE_barrier_Acquire_critical( the_barrier, lock_context );

  number_of_waiting_threads = the_barrier->number_of_waiting_threads;
  ++number_of_waiting_threads;

  if (
    _CORE_barrier_Is_automatic( &the_barrier->Attributes )
      && number_of_waiting_threads == the_barrier->Attributes.maximum_count
  ) {
    executing->Wait.return_code = CORE_BARRIER_STATUS_AUTOMATICALLY_RELEASED;
    _CORE_barrier_Surrender( the_barrier, mp_callout, lock_context );
  } else {
    the_barrier->number_of_waiting_threads = number_of_waiting_threads;
    _Thread_queue_Enqueue_critical(
      &the_barrier->Wait_queue.Queue,
      CORE_BARRIER_TQ_OPERATIONS,
      executing,
      STATES_WAITING_FOR_BARRIER,
      timeout,
      CORE_BARRIER_TIMEOUT,
      lock_context
    );
  }
}
