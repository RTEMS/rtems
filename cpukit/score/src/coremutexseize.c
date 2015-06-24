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

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/coremuteximpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/thread.h>

#if defined(__RTEMS_DO_NOT_INLINE_CORE_MUTEX_SEIZE__)
void _CORE_mutex_Seize(
  CORE_mutex_Control  *_the_mutex,
  Thread_Control      *_executing,
  Objects_Id           _id,
  bool                 _wait,
  Watchdog_Interval    _timeout,
  ISR_Level            _level
)
{
  _CORE_mutex_Seize_body(
    _the_mutex,
    _executing,
    _id,
    _wait,
    _timeout,
    _level
  );
}
#endif

void _CORE_mutex_Seize_interrupt_blocking(
  CORE_mutex_Control  *the_mutex,
  Thread_Control      *executing,
  Watchdog_Interval    timeout,
  ISR_lock_Context    *lock_context
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
    _Thread_queue_Release( &the_mutex->Wait_queue, lock_context );
#endif

    _Thread_Raise_priority( holder, executing->current_priority );

#if !defined(RTEMS_SMP)
    _Thread_queue_Acquire( &the_mutex->Wait_queue, lock_context );
#endif
  }

  _Thread_queue_Enqueue_critical(
    &the_mutex->Wait_queue.Queue,
    the_mutex->Wait_queue.operations,
    executing,
    STATES_WAITING_FOR_MUTEX,
    timeout,
    CORE_MUTEX_TIMEOUT,
    lock_context
  );

#if !defined(RTEMS_SMP)
  _Thread_Dispatch_enable( _Per_CPU_Get() );
#endif
}

