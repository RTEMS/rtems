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

Status_Control _CORE_mutex_Seize_interrupt_blocking(
  CORE_mutex_Control   *the_mutex,
  Thread_Control       *executing,
  Watchdog_Interval     timeout,
  Thread_queue_Context *queue_context
)
{
#if !defined(RTEMS_SMP)
  /*
   * We must disable thread dispatching here since we enable the interrupts for
   * priority inheritance mutexes.
   */
  _Thread_Dispatch_disable();
#endif

  if ( _CORE_mutex_Is_inherit_priority( &the_mutex->Attributes ) ) {
    Thread_Control *holder = the_mutex->holder;

#if !defined(RTEMS_SMP)
    /*
     * To enable interrupts here works only since exactly one executing thread
     * exists and only threads are allowed to seize and surrender mutexes with
     * the priority inheritance protocol.  On SMP configurations more than one
     * executing thread may exist, so here we must not release the lock, since
     * otherwise the current holder may be no longer the holder of the mutex
     * once we released the lock.
     */
    _CORE_mutex_Release( the_mutex, queue_context );
#endif

    _Thread_Inherit_priority( holder, executing );

#if !defined(RTEMS_SMP)
    _ISR_lock_ISR_disable( &queue_context->Lock_context );
    _CORE_mutex_Acquire_critical( the_mutex, queue_context );
#endif
  }

#if defined(RTEMS_SMP)
  _Thread_queue_Context_set_expected_level( queue_context, 1 );
#else
  _Thread_queue_Context_set_expected_level( queue_context, 2 );
#endif

  _Thread_queue_Enqueue_critical(
    &the_mutex->Wait_queue.Queue,
    the_mutex->operations,
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

