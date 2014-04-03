/**
 *  @file
 *
 *  @brief Seize Mutex with Blocking
 *  @ingroup ScoreMutex
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
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/thread.h>

#if defined(__RTEMS_DO_NOT_INLINE_CORE_MUTEX_SEIZE__)
void _CORE_mutex_Seize(
  CORE_mutex_Control  *_the_mutex,
  Thread_Control      *_executing,
  Objects_Id           _id,
  bool                 _wait,
  Watchdog_Interval    _timeout,
  ISR_Level            _level
)
{
  _CORE_mutex_Seize_body(
    _the_mutex,
    _executing,
    _id,
    _wait,
    _timeout,
    _level
  );
}
#endif

void _CORE_mutex_Seize_interrupt_blocking(
  CORE_mutex_Control  *the_mutex,
  Thread_Control      *executing,
  Watchdog_Interval    timeout
)
{

  if ( _CORE_mutex_Is_inherit_priority( &the_mutex->Attributes ) ) {
    Thread_Control *holder = the_mutex->holder;

    _Scheduler_Change_priority_if_higher(
      _Scheduler_Get( holder ),
      holder,
      executing->current_priority,
      false
    );
  }

  _Thread_queue_Enqueue( &the_mutex->Wait_queue, executing, timeout );

  _Thread_Enable_dispatch();
}

