/**
 * @file
 *
 * @brief Surrender the Mutex
 * @ingroup ScoreMutex
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
#include <rtems/score/thread.h>

Status_Control _CORE_mutex_Surrender(
  CORE_mutex_Control   *the_mutex,
  Thread_queue_Context *queue_context
)
{
  Thread_Control *the_thread;
  Thread_Control *holder;

  holder = the_mutex->holder;

  /*
   *  The following code allows a thread (or ISR) other than the thread
   *  which acquired the mutex to release that mutex.  This is only
   *  allowed when the mutex in quetion is FIFO or simple Priority
   *  discipline.  But Priority Ceiling or Priority Inheritance mutexes
   *  must be released by the thread which acquired them.
   */

  if ( the_mutex->Attributes.only_owner_release ) {
    if ( !_Thread_Is_executing( holder ) ) {
      _ISR_lock_ISR_enable( &queue_context->Lock_context );
      return STATUS_NOT_OWNER;
    }
  }

  _CORE_mutex_Acquire_critical( the_mutex, queue_context );

  /* XXX already unlocked -- not right status */

  if ( !the_mutex->nest_count ) {
    _CORE_mutex_Release( the_mutex, queue_context );
    return STATUS_SUCCESSFUL;
  }

  the_mutex->nest_count--;

  if ( the_mutex->nest_count != 0 ) {
    /*
     *  All error checking is on the locking side, so if the lock was
     *  allowed to acquired multiple times, then we should just deal with
     *  that.  The RTEMS_DEBUG is just a validation.
     */
    #if defined(RTEMS_DEBUG)
      switch ( the_mutex->Attributes.lock_nesting_behavior ) {
        case CORE_MUTEX_NESTING_ACQUIRES:
          _CORE_mutex_Release( the_mutex, queue_context );
          return STATUS_SUCCESSFUL;
        #if defined(RTEMS_POSIX_API)
          case CORE_MUTEX_NESTING_IS_ERROR:
            /* should never occur */
            _CORE_mutex_Release( the_mutex, queue_context );
            return STATUS_NESTING_NOT_ALLOWED;
        #endif
        case CORE_MUTEX_NESTING_BLOCKS:
          /* Currently no API exercises this behavior. */
          break;
      }
    #else
      _CORE_mutex_Release( the_mutex, queue_context );
      /* must be CORE_MUTEX_NESTING_ACQUIRES or we wouldn't be here */
      return STATUS_SUCCESSFUL;
    #endif
  }

  /*
   *  Formally release the mutex before possibly transferring it to a
   *  blocked thread.
   */
  if ( _CORE_mutex_Is_inherit_priority( &the_mutex->Attributes ) ||
       _CORE_mutex_Is_priority_ceiling( &the_mutex->Attributes ) ) {
    holder->resource_count--;
  }
  the_mutex->holder = NULL;

  /*
   *  Now we check if another thread was waiting for this mutex.  If so,
   *  transfer the mutex to that thread.
   */
  if (
    ( the_thread = _Thread_queue_First_locked(
        &the_mutex->Wait_queue,
        the_mutex->operations
      )
    )
  ) {
    bool unblock;

    the_mutex->holder     = the_thread;
    the_mutex->nest_count = 1;

    /*
     * We must extract the thread now since this will restore its default
     * thread lock.  This is necessary to avoid a deadlock in the
     * _Thread_Change_priority() below due to a recursive thread queue lock
     * acquire.
     */
    unblock = _Thread_queue_Extract_locked(
      &the_mutex->Wait_queue.Queue,
      the_mutex->operations,
      the_thread,
      queue_context
    );

#if defined(RTEMS_MULTIPROCESSING)
    if ( _Objects_Is_local_id( the_thread->Object.id ) )
#endif
    {
      switch ( the_mutex->Attributes.discipline ) {
        case CORE_MUTEX_DISCIPLINES_FIFO:
        case CORE_MUTEX_DISCIPLINES_PRIORITY:
          break;
        case CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT:
          the_thread->resource_count++;
          _Thread_queue_Boost_priority( &the_mutex->Wait_queue.Queue, the_thread );
          break;
        case CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING:
          the_thread->resource_count++;
          _Thread_Raise_priority(
            the_thread,
            the_mutex->Attributes.priority_ceiling
          );
          break;
      }
    }

    _Thread_queue_Unblock_critical(
      unblock,
      &the_mutex->Wait_queue.Queue,
      the_thread,
      &queue_context->Lock_context
    );
  } else {
    _CORE_mutex_Release( the_mutex, queue_context );
  }

  /*
   *  Whether or not someone is waiting for the mutex, an
   *  inherited priority must be lowered if this is the last
   *  mutex (i.e. resource) this task has.
   */
  if ( !_Thread_Owns_resources( holder ) ) {
    /*
     * Ensure that the holder resource count is visible to all other processors
     * and that we read the latest priority restore hint.
     */
    _Atomic_Fence( ATOMIC_ORDER_ACQ_REL );

    if ( holder->priority_restore_hint ) {
      Per_CPU_Control *cpu_self;

      cpu_self = _Thread_Dispatch_disable();
      _Thread_Restore_priority( holder );
      _Thread_Dispatch_enable( cpu_self );
    }
  }

  return STATUS_SUCCESSFUL;
}
