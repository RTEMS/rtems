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
 *  http://www.rtems.com/license/LICENSE.
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
 *  _CORE_mutex_Seize (interrupt blocking support)
 *
 *  This routine blocks the caller thread after an attempt attempts to obtain
 *  the specified mutex has failed.
 *
 *  Input parameters:
 *    the_mutex - pointer to mutex control block
 *    timeout   - number of ticks to wait (0 means forever)
 */

void _CORE_mutex_Seize_interrupt_blocking(
  CORE_mutex_Control  *the_mutex,
  Watchdog_Interval    timeout
)
{
  Thread_Control   *executing;

  executing = _Thread_Executing;
  if ( _CORE_mutex_Is_inherit_priority( &the_mutex->Attributes ) ) {
    if ( the_mutex->holder->current_priority > executing->current_priority ) {
      _Thread_Change_priority(
        the_mutex->holder,
        executing->current_priority,
        FALSE
      );
    }
  }

  the_mutex->blocked_count++;
  _Thread_queue_Enqueue( &the_mutex->Wait_queue, timeout );

  if ( _Thread_Executing->Wait.return_code == CORE_MUTEX_STATUS_SUCCESSFUL ) {
    /*
     *  if CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT then nothing to do
     *  because this task is already the highest priority.
     */

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
  _Thread_Enable_dispatch();
}

#if !defined(RTEMS_INLINES)
int _CORE_mutex_Seize_interrupt_trylock(
  CORE_mutex_Control  *the_mutex,
  ISR_Level           *level_p
)
{ 
  Thread_Control   *executing;
  ISR_Level         level = *level_p;
  
  /* disabled when you get here */
    
  executing = _Thread_Executing;
  executing->Wait.return_code = CORE_MUTEX_STATUS_SUCCESSFUL;
  if ( !_CORE_mutex_Is_locked( the_mutex ) ) {
    the_mutex->lock       = CORE_MUTEX_LOCKED;
    the_mutex->holder     = executing;
    the_mutex->holder_id  = executing->Object.id;
    the_mutex->nest_count = 1;
    if ( _CORE_mutex_Is_inherit_priority( &the_mutex->Attributes ) ||
         _CORE_mutex_Is_priority_ceiling( &the_mutex->Attributes ) )
      executing->resource_count++;

    if ( !_CORE_mutex_Is_priority_ceiling( &the_mutex->Attributes ) ) {
      _ISR_Enable( level );
      return 0;
    }
    /* else must be CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING */
    {
       Priority_Control  ceiling;
       Priority_Control  current;

       ceiling = the_mutex->Attributes.priority_ceiling;
       current = executing->current_priority;
       if ( current == ceiling ) {
         _ISR_Enable( level );
         return 0;
       }
       if ( current > ceiling ) {
        _Thread_Disable_dispatch();
        _ISR_Enable( level );
        _Thread_Change_priority(
          the_mutex->holder,
          the_mutex->Attributes.priority_ceiling,
          FALSE
        );
        _Thread_Enable_dispatch();
        return 0;
      }
      /* if ( current < ceiling ) */ {
        executing->Wait.return_code = CORE_MUTEX_STATUS_CEILING_VIOLATED;
        the_mutex->nest_count = 0;     /* undo locking above */
        executing->resource_count--;   /* undo locking above */
        _ISR_Enable( level );
        return 0;
      }
    }
    return 0;
  }

  if ( _Thread_Is_executing( the_mutex->holder ) ) {
    switch ( the_mutex->Attributes.lock_nesting_behavior ) {
      case CORE_MUTEX_NESTING_ACQUIRES:
        the_mutex->nest_count++;
        _ISR_Enable( level );
        return 0;
      case CORE_MUTEX_NESTING_IS_ERROR:
        executing->Wait.return_code = CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED;
        _ISR_Enable( level );
        return 0;
      case CORE_MUTEX_NESTING_BLOCKS:
        break;
    }
  }

  return 1;
}
#endif
