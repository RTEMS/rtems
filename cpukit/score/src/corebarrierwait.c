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
#include <rtems/score/isrlevel.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadqimpl.h>

void _CORE_barrier_Wait(
  CORE_barrier_Control                *the_barrier,
  Thread_Control                      *executing,
  Objects_Id                           id,
  bool                                 wait,
  Watchdog_Interval                    timeout,
  CORE_barrier_API_mp_support_callout  api_barrier_mp_support
)
{
  ISR_lock_Context lock_context;

  executing->Wait.return_code = CORE_BARRIER_STATUS_SUCCESSFUL;
  _Thread_queue_Acquire( &the_barrier->Wait_queue, &lock_context );
  the_barrier->number_of_waiting_threads++;
  if ( _CORE_barrier_Is_automatic( &the_barrier->Attributes ) ) {
    if ( the_barrier->number_of_waiting_threads ==
	 the_barrier->Attributes.maximum_count) {
      executing->Wait.return_code = CORE_BARRIER_STATUS_AUTOMATICALLY_RELEASED;
      _Thread_queue_Release( &the_barrier->Wait_queue, &lock_context );
      _CORE_barrier_Release( the_barrier, id, api_barrier_mp_support );
      return;
    }
  }

  executing->Wait.id = id;

  _Thread_queue_Enqueue_critical(
    &the_barrier->Wait_queue.Queue,
    the_barrier->Wait_queue.operations,
    executing,
    STATES_WAITING_FOR_BARRIER,
    timeout,
    CORE_BARRIER_TIMEOUT,
    &lock_context
  );
}
