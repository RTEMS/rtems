/**
 *  @file
 *
 *  @brief RTEMS Task Wake After
 *  @ingroup ClassicTasks
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasks.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/watchdogimpl.h>

rtems_status_code rtems_task_wake_after(
  rtems_interval ticks
)
{
  /*
   * It is critical to obtain the executing thread after thread dispatching is
   * disabled on SMP configurations.
   */
  Thread_Control *executing;

  _Thread_Disable_dispatch();
    executing = _Thread_Executing;

    if ( ticks == 0 ) {
      _Scheduler_Yield( executing );
    } else {
      _Thread_Set_state( executing, STATES_DELAYING );
      _Watchdog_Initialize(
        &executing->Timer,
        _Thread_Delay_ended,
        executing->Object.id,
        NULL
      );
      _Watchdog_Insert_ticks( &executing->Timer, ticks );
    }
  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}
