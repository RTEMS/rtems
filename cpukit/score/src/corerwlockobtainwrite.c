/**
 * @file
 *
 * @brief RWLock Obtain for Writing
 * @ingroup RTEMSScoreRWLock
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

Status_Control _CORE_RWLock_Seize_for_writing(
  CORE_RWLock_Control  *the_rwlock,
  bool                  wait,
  Thread_queue_Context *queue_context
)
{
  Thread_Control *executing;

  /*
   *  If unlocked, then OK to read.
   *  Otherwise, we have to block.
   *  If locked for reading and no waiters, then OK to read.
   *  If any thread is waiting, then we wait.
   */

  executing = _CORE_RWLock_Acquire( the_rwlock, queue_context );

  switch ( the_rwlock->current_state ) {
    case CORE_RWLOCK_UNLOCKED:
      the_rwlock->current_state = CORE_RWLOCK_LOCKED_FOR_WRITING;
      _CORE_RWLock_Release( the_rwlock, queue_context );
      return STATUS_SUCCESSFUL;

    case CORE_RWLOCK_LOCKED_FOR_READING:
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

  executing->Wait.option = CORE_RWLOCK_THREAD_WAITING_FOR_WRITE;

  _Thread_queue_Context_set_thread_state(
    queue_context,
    STATES_WAITING_FOR_RWLOCK
  );
  _Thread_queue_Enqueue(
     &the_rwlock->Queue.Queue,
     CORE_RWLOCK_TQ_OPERATIONS,
     executing,
     queue_context
  );
  return _Thread_Wait_get_status( executing );
}
