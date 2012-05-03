/*
 *  Mutex Handler
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the Mutex Handler.
 *  This handler provides synchronization and mutual exclusion capabilities.
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

#ifdef __RTEMS_STRICT_ORDER_MUTEX__
  static inline void _CORE_mutex_Push_priority(
    CORE_mutex_Control *mutex,
    Thread_Control *thread
  )
  {
    _Chain_Prepend_unprotected(
      &thread->lock_mutex,
      &mutex->queue.lock_queue
    );
    mutex->queue.priority_before = thread->current_priority;
  }

  static inline CORE_mutex_Status _CORE_mutex_Pop_priority(
    CORE_mutex_Control *mutex,
    Thread_Control *holder
  )
  {
    /*
     *  Check whether the holder release the mutex in LIFO order if not return
     *  error code.
     */
    if ( _Chain_First( holder->lock_mutex ) != &mutex->queue.lock_queue ) {
      mutex->nest_count++;

      return CORE_MUTEX_RELEASE_NOT_ORDER;
    }

    /*
     *  This pops the first node from the list.
     */
    _Chain_Get_first_unprotected( &holder->lock_mutex );

    if ( mutex->queue.priority_before != holder->current_priority )
      _Thread_Change_priority( holder, mutex->queue.priority_before, true );

    return CORE_MUTEX_STATUS_SUCCESSFUL;
  }
#else
  #define _CORE_mutex_Push_priority( mutex, thread ) ((void) 0)

  #define _CORE_mutex_Pop_priority( mutex, thread ) \
    CORE_MUTEX_STATUS_SUCCESSFUL
#endif

/*
 *  _CORE_mutex_Surrender
 *
 *  DESCRIPTION:
 *
 *  This routine frees a unit to the mutex.  If a task was blocked waiting for
 *  a unit from this mutex, then that task will be readied and the unit
 *  given to that task.  Otherwise, the unit will be returned to the mutex.
 *
 *  Input parameters:
 *    the_mutex            - the mutex to be flushed
 *    id                   - id of parent mutex
 *    api_mutex_mp_support - api dependent MP support actions
 *
 *  Output parameters:
 *    CORE_MUTEX_STATUS_SUCCESSFUL - if successful
 *    core error code              - if unsuccessful
 */

CORE_mutex_Status _CORE_mutex_Surrender(
  CORE_mutex_Control                *the_mutex,
#if defined(RTEMS_MULTIPROCESSING)
  Objects_Id                         id,
  CORE_mutex_API_mp_support_callout  api_mutex_mp_support
#else
  Objects_Id                         id __attribute__((unused)),
  CORE_mutex_API_mp_support_callout  api_mutex_mp_support __attribute__((unused))
#endif
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
    if ( !_Thread_Is_executing( holder ) )
      return CORE_MUTEX_STATUS_NOT_OWNER_OF_RESOURCE;
  }

  /* XXX already unlocked -- not right status */

  if ( !the_mutex->nest_count )
    return CORE_MUTEX_STATUS_SUCCESSFUL;

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
          return CORE_MUTEX_STATUS_SUCCESSFUL;
        #if defined(RTEMS_POSIX_API)
          case CORE_MUTEX_NESTING_IS_ERROR:
            /* should never occur */
            return CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED;
        #endif
        case CORE_MUTEX_NESTING_BLOCKS:
          /* Currently no API exercises this behavior. */
          break;
      }
    #else
      /* must be CORE_MUTEX_NESTING_ACQUIRES or we wouldn't be here */
      return CORE_MUTEX_STATUS_SUCCESSFUL;
    #endif
  }

  /*
   *  Formally release the mutex before possibly transferring it to a
   *  blocked thread.
   */
  if ( _CORE_mutex_Is_inherit_priority( &the_mutex->Attributes ) ||
       _CORE_mutex_Is_priority_ceiling( &the_mutex->Attributes ) ) {
    CORE_mutex_Status pop_status =
      _CORE_mutex_Pop_priority( the_mutex, holder );

    if ( pop_status != CORE_MUTEX_STATUS_SUCCESSFUL )
      return pop_status;

    holder->resource_count--;

    /*
     *  Whether or not someone is waiting for the mutex, an
     *  inherited priority must be lowered if this is the last
     *  mutex (i.e. resource) this task has.
     */
    if ( holder->resource_count == 0 &&
         holder->real_priority != holder->current_priority ) {
      _Thread_Change_priority( holder, holder->real_priority, true );
    }
  }
  the_mutex->holder    = NULL;
  the_mutex->holder_id = 0;

  /*
   *  Now we check if another thread was waiting for this mutex.  If so,
   *  transfer the mutex to that thread.
   */
  if ( ( the_thread = _Thread_queue_Dequeue( &the_mutex->Wait_queue ) ) ) {

#if defined(RTEMS_MULTIPROCESSING)
    if ( !_Objects_Is_local_id( the_thread->Object.id ) ) {

      the_mutex->holder     = NULL;
      the_mutex->holder_id  = the_thread->Object.id;
      the_mutex->nest_count = 1;

      ( *api_mutex_mp_support)( the_thread, id );

    } else
#endif
    {

      the_mutex->holder     = the_thread;
      the_mutex->holder_id  = the_thread->Object.id;
      the_mutex->nest_count = 1;

      switch ( the_mutex->Attributes.discipline ) {
        case CORE_MUTEX_DISCIPLINES_FIFO:
        case CORE_MUTEX_DISCIPLINES_PRIORITY:
          break;
        case CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT:
          _CORE_mutex_Push_priority( the_mutex, the_thread );
          the_thread->resource_count++;
          break;
        case CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING:
          _CORE_mutex_Push_priority( the_mutex, the_thread );
          the_thread->resource_count++;
          if (the_mutex->Attributes.priority_ceiling <
              the_thread->current_priority){
              _Thread_Change_priority(
                the_thread,
                the_mutex->Attributes.priority_ceiling,
                false
              );
          }
          break;
      }
    }
  } else
    the_mutex->lock = CORE_MUTEX_UNLOCKED;

  return CORE_MUTEX_STATUS_SUCCESSFUL;
}
