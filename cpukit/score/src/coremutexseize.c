/**
 * @file
 *
 * @ingroup RTEMSScoreMutex
 *
 * @brief This source file contains the implementation of
 *   _CORE_mutex_Seize_slow().
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

#include <rtems/score/coremuteximpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>

Status_Control _CORE_mutex_Seize_slow(
  CORE_mutex_Control            *the_mutex,
  const Thread_queue_Operations *operations,
  Thread_Control                *executing,
  bool                           wait,
  Thread_queue_Context          *queue_context
)
{
  if ( wait ) {
    _Thread_queue_Context_set_thread_state(
      queue_context,
      STATES_WAITING_FOR_MUTEX
    );
    _Thread_queue_Context_set_deadlock_callout(
      queue_context,
      _Thread_queue_Deadlock_status
    );
    _Thread_queue_Enqueue(
      &the_mutex->Wait_queue.Queue,
      operations,
      executing,
      queue_context
    );
    return _Thread_Wait_get_status( executing );
  } else {
    _CORE_mutex_Release( the_mutex, queue_context );
    return STATUS_UNAVAILABLE;
  }
}
