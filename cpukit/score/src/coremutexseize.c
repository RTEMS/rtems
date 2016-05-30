/**
 *  @file
 *
 *  @brief Seize Mutex with Blocking
 *  @ingroup ScoreMutex
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

#include <rtems/score/coremuteximpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/thread.h>

Status_Control _CORE_mutex_Seize_slow(
  CORE_mutex_Control   *the_mutex,
  Thread_Control       *executing,
  Thread_Control       *owner,
  bool                  wait,
  Watchdog_Interval     timeout,
  Thread_queue_Context *queue_context
)
{
  if ( !wait ) {
    _CORE_mutex_Release( the_mutex, queue_context );
    return STATUS_UNAVAILABLE;
  }

#if !defined(RTEMS_SMP)
  /*
   * We must disable thread dispatching here since we enable the interrupts for
   * priority inheritance mutexes.
   */
  _Thread_Dispatch_disable();

  /*
   * To enable interrupts here works only since exactly one executing thread
   * exists and only threads are allowed to seize and surrender mutexes with
   * the priority inheritance protocol.  On SMP configurations more than one
   * executing thread may exist, so here we must not release the lock, since
   * otherwise the current owner may be no longer the owner of the mutex
   * once we released the lock.
   */
  _CORE_mutex_Release( the_mutex, queue_context );
#endif

  _Thread_Inherit_priority( owner, executing );

#if defined(RTEMS_SMP)
  _Thread_queue_Context_set_expected_level( queue_context, 1 );
#else
  _ISR_lock_ISR_disable( &queue_context->Lock_context );
  _CORE_mutex_Acquire_critical( the_mutex, queue_context );
  _Thread_queue_Context_set_expected_level( queue_context, 2 );
#endif

  _Thread_queue_Enqueue_critical(
    &the_mutex->Wait_queue.Queue,
    CORE_MUTEX_TQ_OPERATIONS,
    executing,
    STATES_WAITING_FOR_MUTEX,
    timeout,
    queue_context
  );

#if !defined(RTEMS_SMP)
  _Thread_Dispatch_enable( _Per_CPU_Get() );
#endif

  return _Thread_Wait_get_status( executing );
}

Status_Control _CORE_mutex_Seize_no_protocol_slow(
  CORE_mutex_Control            *the_mutex,
  const Thread_queue_Operations *operations,
  Thread_Control                *executing,
  bool                           wait,
  Watchdog_Interval              timeout,
  Thread_queue_Context          *queue_context
)
{
  if ( wait ) {
    _Thread_queue_Context_set_expected_level( queue_context, 1 );
    _Thread_queue_Enqueue_critical(
      &the_mutex->Wait_queue.Queue,
      operations,
      executing,
      STATES_WAITING_FOR_MUTEX,
      timeout,
      queue_context
    );
    return _Thread_Wait_get_status( executing );
  } else {
    _CORE_mutex_Release( the_mutex, queue_context );
    return STATUS_UNAVAILABLE;
  }
}
