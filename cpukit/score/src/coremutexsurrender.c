/*
 *  Mutex Handler
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the Mutex Handler.
 *  This handler provides synchronization and mutual exclusion capabilities.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

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
  Objects_Id                         id,
  CORE_mutex_API_mp_support_callout  api_mutex_mp_support
)
{
  Thread_Control *the_thread;
  Thread_Control *holder;

  holder    = the_mutex->holder;

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
    switch ( the_mutex->Attributes.lock_nesting_behavior ) {
      case CORE_MUTEX_NESTING_ACQUIRES:
        return CORE_MUTEX_STATUS_SUCCESSFUL;
      case CORE_MUTEX_NESTING_IS_ERROR:
        /* should never occur */
        return CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED;
      case CORE_MUTEX_NESTING_BLOCKS:
        /* Currently no API exercises this behavior. */
        break;
    }
  }

  if ( _CORE_mutex_Is_inherit_priority( &the_mutex->Attributes ) ||
       _CORE_mutex_Is_priority_ceiling( &the_mutex->Attributes ) )
    holder->resource_count--;
  the_mutex->holder    = NULL;
  the_mutex->holder_id = 0;

  /*
   *  Whether or not someone is waiting for the mutex, an
   *  inherited priority must be lowered if this is the last
   *  mutex (i.e. resource) this task has.
   */

  if ( _CORE_mutex_Is_inherit_priority( &the_mutex->Attributes ) ||
       _CORE_mutex_Is_priority_ceiling( &the_mutex->Attributes ) ) {
    if ( holder->resource_count == 0 && 
         holder->real_priority != holder->current_priority ) {
      _Thread_Change_priority( holder, holder->real_priority, TRUE );
    }
  }

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
      if ( _CORE_mutex_Is_inherit_priority( &the_mutex->Attributes ) ||
           _CORE_mutex_Is_priority_ceiling( &the_mutex->Attributes ) )
        the_thread->resource_count++;
      the_mutex->nest_count = 1;

     /*
      *  No special action for priority inheritance or priority ceiling
      *  because the_thread is guaranteed to be the highest priority
      *  thread waiting for the mutex.
      */
    }
  } else
    the_mutex->lock = CORE_MUTEX_UNLOCKED;

  return CORE_MUTEX_STATUS_SUCCESSFUL;
}

