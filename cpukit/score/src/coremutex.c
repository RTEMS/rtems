/*
 *  Mutex Handler
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the Mutex Handler.
 *  This handler provides synchronization and mutual exclusion capabilities.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

/*PAGE
 *
 *  _CORE_mutex_Initialize
 *
 *  This routine initializes a mutex at create time and set the control
 *  structure according to the values passed.
 *
 *  Input parameters: 
 *    the_mutex             - the mutex control block to initialize
 *    the_class             - the API class of the object
 *    the_mutex_attributes  - the mutex attributes specified at create time
 *    initial_lock          - mutex initial lock or unlocked status
 *    proxy_extract_callout - MP specific extract callout
 *
 *  Output parameters:  NONE
 */

void _CORE_mutex_Initialize(
  CORE_mutex_Control           *the_mutex,
  Objects_Classes               the_class,
  CORE_mutex_Attributes        *the_mutex_attributes,
  unsigned32                    initial_lock,
  Thread_queue_Extract_callout  proxy_extract_callout
)
{

/* Add this to the RTEMS environment later ????????? 
  rtems_assert( initial_lock == CORE_MUTEX_LOCKED ||
                initial_lock == CORE_MUTEX_UNLOCKED );
 */

  the_mutex->Attributes = *the_mutex_attributes;
  the_mutex->lock          = initial_lock;

  if ( initial_lock == CORE_MUTEX_LOCKED ) {
    the_mutex->nest_count = 1;
    the_mutex->holder     = _Thread_Executing;
    the_mutex->holder_id  = _Thread_Executing->Object.id;
    _Thread_Executing->resource_count++;
  } else {
    the_mutex->nest_count = 0;
    the_mutex->holder     = NULL;
    the_mutex->holder_id  = 0;
  }

  _Thread_queue_Initialize(
    &the_mutex->Wait_queue,
    the_class,
    _CORE_mutex_Is_priority( the_mutex_attributes ) ?
      THREAD_QUEUE_DISCIPLINE_PRIORITY : THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_MUTEX,
    proxy_extract_callout,
    CORE_MUTEX_TIMEOUT
  );
}

/*PAGE
 *
 *  _CORE_mutex_Seize
 *
 *  This routine attempts to allocate a mutex to the calling thread.
 *
 *  Input parameters:
 *    the_mutex - pointer to mutex control block
 *    id        - id of object to wait on
 *    wait      - TRUE if wait is allowed, FALSE otherwise
 *    timeout   - number of ticks to wait (0 means forever)
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    available
 *    wait
 */

void _CORE_mutex_Seize(
  CORE_mutex_Control  *the_mutex,
  Objects_Id           id,
  boolean              wait,
  Watchdog_Interval    timeout
)
{
  Thread_Control *executing;
  ISR_Level       level;

  executing = _Thread_Executing;
  switch ( the_mutex->Attributes.discipline ) {
    case CORE_MUTEX_DISCIPLINES_FIFO:
    case CORE_MUTEX_DISCIPLINES_PRIORITY:
    case CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT:
      break;
    case CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING:
      if ( executing->current_priority <
                              the_mutex->Attributes.priority_ceiling) {
        executing->Wait.return_code = CORE_MUTEX_STATUS_CEILING_VIOLATED;
        return;
      }
  }
  executing->Wait.return_code = CORE_MUTEX_STATUS_SUCCESSFUL;
  _ISR_Disable( level );
  if ( ! _CORE_mutex_Is_locked( the_mutex ) ) {
    the_mutex->lock       = CORE_MUTEX_LOCKED;
    the_mutex->holder     = executing;
    the_mutex->holder_id  = executing->Object.id;
    the_mutex->nest_count = 1;
    executing->resource_count++;
    _ISR_Enable( level );
    switch ( the_mutex->Attributes.discipline ) {
      case CORE_MUTEX_DISCIPLINES_FIFO:
      case CORE_MUTEX_DISCIPLINES_PRIORITY:
      case CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT:
        /* already the highest priority */
        break;
      case CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING:
      if ( the_mutex->Attributes.priority_ceiling <
                                           executing->current_priority ) {
        _Thread_Change_priority(
          the_mutex->holder,
          the_mutex->Attributes.priority_ceiling
        );
      }
    }
    return;
  }

  if ( _Objects_Are_ids_equal(
              _Thread_Executing->Object.id, the_mutex->holder_id ) ) {
    if ( _CORE_mutex_Is_nesting_allowed( &the_mutex->Attributes ) )
      the_mutex->nest_count++;
    else 
      executing->Wait.return_code = CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED;

    _ISR_Enable( level );
    return;
  }

  if ( !wait ) {
    _ISR_Enable( level );
    executing->Wait.return_code = CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT;
    return;
  }

  _Thread_queue_Enter_critical_section( &the_mutex->Wait_queue );
  executing->Wait.queue      = &the_mutex->Wait_queue;
  executing->Wait.id         = id;
  _ISR_Enable( level );

  switch ( the_mutex->Attributes.discipline ) {
    case CORE_MUTEX_DISCIPLINES_FIFO:
    case CORE_MUTEX_DISCIPLINES_PRIORITY:
    case CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING:
      break;
    case CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT:
      if ( the_mutex->holder->current_priority > executing->current_priority ) {
        _Thread_Change_priority(
          the_mutex->holder,
          executing->current_priority
        );
      }
      break;
  }

  _Thread_queue_Enqueue( &the_mutex->Wait_queue, timeout );

  if ( _Thread_Executing->Wait.return_code == CORE_MUTEX_STATUS_SUCCESSFUL ) {
    switch ( the_mutex->Attributes.discipline ) {
      case CORE_MUTEX_DISCIPLINES_FIFO:
      case CORE_MUTEX_DISCIPLINES_PRIORITY:
      case CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT:
        break;
      case CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING:
        if ( the_mutex->Attributes.priority_ceiling <
                                           executing->current_priority ) {
          _Thread_Change_priority(
            executing,
            the_mutex->Attributes.priority_ceiling
          );
        };
        break;
    }
  }
}

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
  Thread_Control *executing;

  executing = _Thread_Executing;

  if ( !_Objects_Are_ids_equal(
           _Thread_Executing->Object.id, the_mutex->holder_id ) )
    return( CORE_MUTEX_STATUS_NOT_OWNER_OF_RESOURCE );

  the_mutex->nest_count--;

  if ( the_mutex->nest_count != 0 )
    return( CORE_MUTEX_STATUS_SUCCESSFUL );

  _Thread_Executing->resource_count--;
  the_mutex->holder    = NULL;
  the_mutex->holder_id = 0;

  /*
   *  Whether or not someone is waiting for the mutex, an
   *  inherited priority must be lowered if this is the last
   *  mutex (i.e. resource) this task has.
   */

  switch ( the_mutex->Attributes.discipline ) {
    case CORE_MUTEX_DISCIPLINES_FIFO:
    case CORE_MUTEX_DISCIPLINES_PRIORITY:
      break;
    case CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING:
    case CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT:
      if ( executing->resource_count == 0 &&
           executing->real_priority !=
           executing->current_priority ) {
         _Thread_Change_priority( executing, executing->real_priority );
      }
      break;
  }


  if ( ( the_thread = _Thread_queue_Dequeue( &the_mutex->Wait_queue ) ) ) {

    if ( !_Objects_Is_local_id( the_thread->Object.id ) ) {
      
      the_mutex->holder     = NULL;
      the_mutex->holder_id  = the_thread->Object.id;
      the_mutex->nest_count = 1;

      ( *api_mutex_mp_support)( the_thread, id );

    } else {

      the_mutex->holder     = the_thread;
      the_mutex->holder_id  = the_thread->Object.id;
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

  return( CORE_MUTEX_STATUS_SUCCESSFUL );
}

/*PAGE
 *
 *  _CORE_mutex_Flush
 *
 *  This function a flushes the mutex's task wait queue.
 *
 *  Input parameters:
 *    the_mutex              - the mutex to be flushed
 *    remote_extract_callout - function to invoke remotely
 *    status                 - status to pass to thread
 *
 *  Output parameters:  NONE
 */

void _CORE_mutex_Flush(
  CORE_mutex_Control         *the_mutex,
  Thread_queue_Flush_callout  remote_extract_callout,
  unsigned32                  status
)
{
  _Thread_queue_Flush(
    &the_mutex->Wait_queue,
    remote_extract_callout,
    status
  );
}
