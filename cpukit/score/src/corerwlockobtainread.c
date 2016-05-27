/**
 * @file 
 * 
 * @brief Obtain RWLock for reading
 * @ingroup ScoreRWLock
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

#include <rtems/score/corerwlockimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/watchdog.h>

Status_Control _CORE_RWLock_Seize_for_reading(
  CORE_RWLock_Control  *the_rwlock,
  Thread_Control       *executing,
  bool                  wait,
  Watchdog_Interval     timeout,
  Thread_queue_Context *queue_context
)
{
  /*
   *  If unlocked, then OK to read.
   *  If locked for reading and no waiters, then OK to read.
   *  If any thread is waiting, then we wait.
   */

  _CORE_RWLock_Acquire_critical( the_rwlock, queue_context );

  switch ( the_rwlock->current_state ) {
    case CORE_RWLOCK_UNLOCKED:
      the_rwlock->current_state = CORE_RWLOCK_LOCKED_FOR_READING;
      the_rwlock->number_of_readers += 1;
      _CORE_RWLock_Release( the_rwlock, queue_context );
      return STATUS_SUCCESSFUL;

    case CORE_RWLOCK_LOCKED_FOR_READING: {
      Thread_Control *waiter;
      waiter = _Thread_queue_First_locked(
        &the_rwlock->Wait_queue,
        CORE_RWLOCK_TQ_OPERATIONS
      );
      if ( !waiter ) {
        the_rwlock->number_of_readers += 1;
        _CORE_RWLock_Release( the_rwlock, queue_context );
        return STATUS_SUCCESSFUL;
      }
      break;
    }
    case CORE_RWLOCK_LOCKED_FOR_WRITING:
      break;
  }

  /*
   *  If the thread is not willing to wait, then return immediately.
   */

  if ( !wait ) {
    _CORE_RWLock_Release( the_rwlock, queue_context );
    return STATUS_UNAVAILABLE;
  }

  /*
   *  We need to wait to enter this critical section
   */

  executing->Wait.option = CORE_RWLOCK_THREAD_WAITING_FOR_READ;

  _Thread_queue_Context_set_expected_level( queue_context, 1 );
  _Thread_queue_Enqueue_critical(
     &the_rwlock->Wait_queue.Queue,
     CORE_RWLOCK_TQ_OPERATIONS,
     executing,
     STATES_WAITING_FOR_RWLOCK,
     timeout,
     queue_context
  );
  return _Thread_Wait_get_status( executing );
}
