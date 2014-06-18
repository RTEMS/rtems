/**
 *  @file
 *
 *  @brief RTEMS Task Wake When
 *  @ingroup ClassicTasks
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasks.h>
#include <rtems/rtems/clock.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/watchdogimpl.h>

rtems_status_code rtems_task_wake_when(
  rtems_time_of_day *time_buffer
)
{
  Watchdog_Interval   seconds;
  Thread_Control     *executing;

  if ( !_TOD_Is_set() )
    return RTEMS_NOT_DEFINED;

  if ( !time_buffer )
    return RTEMS_INVALID_ADDRESS;

  time_buffer->ticks = 0;

  if ( !_TOD_Validate( time_buffer ) )
    return RTEMS_INVALID_CLOCK;

  seconds = _TOD_To_seconds( time_buffer );

  if ( seconds <= _TOD_Seconds_since_epoch() )
    return RTEMS_INVALID_CLOCK;

  _Thread_Disable_dispatch();
    executing = _Thread_Executing;
    _Thread_Set_state( executing, STATES_WAITING_FOR_TIME );
    _Watchdog_Initialize(
      &executing->Timer,
      _Thread_Delay_ended,
      0,
      executing
    );
    _Watchdog_Insert_seconds(
      &executing->Timer,
      seconds - _TOD_Seconds_since_epoch()
    );
  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}
