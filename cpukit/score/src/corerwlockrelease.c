/**
 * @file
 *
 * @brief Releases the RWLock
 *
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
#include <rtems/score/assert.h>

static bool _CORE_RWLock_Is_waiting_for_reading(
  const Thread_Control *the_thread
)
{
  if ( the_thread->Wait.option == CORE_RWLOCK_THREAD_WAITING_FOR_READ ) {
    return true;
  } else {
    _Assert( the_thread->Wait.option == CORE_RWLOCK_THREAD_WAITING_FOR_WRITE );
    return false;
  }
}

static Thread_Control *_CORE_RWLock_Flush_filter(
  Thread_Control       *the_thread,
  Thread_queue_Queue   *queue,
  Thread_queue_Context *queue_context
)
{
  CORE_RWLock_Control *the_rwlock;

  the_rwlock = RTEMS_CONTAINER_OF(
    queue,
    CORE_RWLock_Control,
    Queue.Queue
  );

  switch ( the_rwlock->current_state ) {
    case CORE_RWLOCK_LOCKED_FOR_READING:
      if ( _CORE_RWLock_Is_waiting_for_reading( the_thread ) ) {
        the_rwlock->number_of_readers += 1;
      } else {
        the_thread = NULL;
      }
      break;
    case CORE_RWLOCK_LOCKED_FOR_WRITING:
      the_thread = NULL;
      break;
    default:
      _Assert( the_rwlock->current_state == CORE_RWLOCK_UNLOCKED );
      if ( _CORE_RWLock_Is_waiting_for_reading( the_thread ) ) {
        the_rwlock->current_state = CORE_RWLOCK_LOCKED_FOR_READING;
        the_rwlock->number_of_readers = 1;
      } else {
        the_rwlock->current_state = CORE_RWLOCK_LOCKED_FOR_WRITING;
      }
      break;
  }

  return the_thread;
}

Status_Control _CORE_RWLock_Surrender( CORE_RWLock_Control *the_rwlock )
{
  Thread_queue_Context queue_context;

  /*
   *  If unlocked, then OK to read.
   *  Otherwise, we have to block.
   *  If locked for reading and no waiters, then OK to read.
   *  If any thread is waiting, then we wait.
   */

  _Thread_queue_Context_initialize( &queue_context );
  _CORE_RWLock_Acquire( the_rwlock, &queue_context );

  if ( the_rwlock->current_state == CORE_RWLOCK_UNLOCKED){
    /* This is an error at the caller site */
    _CORE_RWLock_Release( the_rwlock, &queue_context );
    return STATUS_SUCCESSFUL;
  }

  if ( the_rwlock->current_state == CORE_RWLOCK_LOCKED_FOR_READING ) {
    the_rwlock->number_of_readers -= 1;

    if ( the_rwlock->number_of_readers != 0 ) {
      /* must be unlocked again */
      _CORE_RWLock_Release( the_rwlock, &queue_context );
      return STATUS_SUCCESSFUL;
    }
  }

  _Assert(
    the_rwlock->current_state == CORE_RWLOCK_LOCKED_FOR_WRITING
      || ( the_rwlock->current_state == CORE_RWLOCK_LOCKED_FOR_READING
        && the_rwlock->number_of_readers == 0 )
  );

  /*
   * Implicitly transition to "unlocked" and find another thread interested
   * in obtaining this rwlock.
   */
  the_rwlock->current_state = CORE_RWLOCK_UNLOCKED;

  _Thread_queue_Flush_critical(
    &the_rwlock->Queue.Queue,
    CORE_RWLOCK_TQ_OPERATIONS,
    _CORE_RWLock_Flush_filter,
    &queue_context
  );
  return STATUS_SUCCESSFUL;
}
