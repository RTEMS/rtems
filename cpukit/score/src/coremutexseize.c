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

#if defined(__RTEMS_DO_NOT_INLINE_CORE_MUTEX_SEIZE__)
void _CORE_mutex_Seize(
  CORE_mutex_Control  *_the_mutex,
  Objects_Id           _id,
  bool                 _wait,
  Watchdog_Interval    _timeout,
  ISR_Level            _level
)
{
  _CORE_mutex_Seize_body( _the_mutex, _id, _wait, _timeout, _level );
}
#endif

/*
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
    if ( _Scheduler_Is_priority_higher_than(
         executing->current_priority,
         the_mutex->holder->current_priority)) {
      _Thread_Change_priority(
        the_mutex->holder,
        executing->current_priority,
        false
      );
    }
  }

  the_mutex->blocked_count++;
  _Thread_queue_Enqueue( &the_mutex->Wait_queue, timeout );

  _Thread_Enable_dispatch();
}

