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
  if ( _CORE_mutex_Is_priority_ceiling( &the_mutex->Attributes ) ) {
    if ( executing->current_priority < the_mutex->Attributes.priority_ceiling) {
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
    if ( _CORE_mutex_Is_priority_ceiling( &the_mutex->Attributes ) ||
         _CORE_mutex_Is_inherit_priority( &the_mutex->Attributes ) )
      executing->resource_count++;
    _ISR_Enable( level );
    /*
     *  if CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT then nothing to do
     *  because this task is already the highest priority.
     */

    if ( _CORE_mutex_Is_priority_ceiling( &the_mutex->Attributes ) ) {
      if (the_mutex->Attributes.priority_ceiling < executing->current_priority){
        _Thread_Change_priority(
          the_mutex->holder,
          the_mutex->Attributes.priority_ceiling,
          FALSE
        );
      }
    }
    executing->Wait.return_code = CORE_MUTEX_STATUS_SUCCESSFUL;
    return;
  }

  if ( _Thread_Is_executing( the_mutex->holder ) ) {
    switch ( the_mutex->Attributes.lock_nesting_behavior ) {
      case CORE_MUTEX_NESTING_ACQUIRES:
        the_mutex->nest_count++;
        _ISR_Enable( level );
        return;
      case CORE_MUTEX_NESTING_IS_ERROR:
        executing->Wait.return_code = CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED;
        _ISR_Enable( level );
        return;
      case CORE_MUTEX_NESTING_BLOCKS:
        break;
    }
  }

  if ( !wait ) {
    _ISR_Enable( level );
    executing->Wait.return_code = CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT;
    return;
  }

  _Thread_queue_Enter_critical_section( &the_mutex->Wait_queue );
  executing->Wait.queue = &the_mutex->Wait_queue;
  executing->Wait.id    = id;
  _ISR_Enable( level );

  if ( _CORE_mutex_Is_inherit_priority( &the_mutex->Attributes ) ) {
    if ( the_mutex->holder->current_priority > executing->current_priority ) {
      _Thread_Change_priority(
        the_mutex->holder,
        executing->current_priority,
        FALSE
      );
    }
  }

  _Thread_queue_Enqueue( &the_mutex->Wait_queue, timeout );

  if ( _Thread_Executing->Wait.return_code == CORE_MUTEX_STATUS_SUCCESSFUL ) {
    if ( _CORE_mutex_Is_priority_ceiling( &the_mutex->Attributes ) ) {
      if (the_mutex->Attributes.priority_ceiling < executing->current_priority){
        _Thread_Change_priority(
          executing,
          the_mutex->Attributes.priority_ceiling,
          FALSE
        );
      }
    }
  }
}

