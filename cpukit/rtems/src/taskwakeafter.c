/*
 *  RTEMS Task Manager
 *
 *
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

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/modes.h>
#include <rtems/score/object.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/stack.h>
#include <rtems/score/states.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tod.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>
#include <rtems/score/sysstate.h>

/*
 *  rtems_task_wake_after
 *
 *  This directive suspends the requesting thread for the given amount
 *  of ticks.
 *
 *  Input parameters:
 *    ticks - number of ticks to wait
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - always successful
 */

rtems_status_code rtems_task_wake_after(
  rtems_interval ticks
)
{
  _Thread_Disable_dispatch();
    if ( ticks == 0 ) {
      _Scheduler_Yield();
    } else {
      _Thread_Set_state( _Thread_Executing, STATES_DELAYING );
      _Watchdog_Initialize(
        &_Thread_Executing->Timer,
        _Thread_Delay_ended,
        _Thread_Executing->Object.id,
        NULL
      );
      _Watchdog_Insert_ticks( &_Thread_Executing->Timer, ticks );
    }
  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}
