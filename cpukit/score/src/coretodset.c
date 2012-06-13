/*
 *  Time of Day (TOD) Handler -- Set Time
 */

/*  COPYRIGHT (c) 1989-2007.
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
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/timestamp.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

void _TOD_Set_with_timestamp(
  const Timestamp_Control *tod
)
{
  uint32_t nanoseconds = _Timestamp_Get_nanoseconds( tod );
  Watchdog_Interval seconds_next = _Timestamp_Get_seconds( tod );
  Watchdog_Interval seconds_now;

  _Thread_Disable_dispatch();
  _TOD_Deactivate();

  seconds_now = _TOD_Seconds_since_epoch();

  if ( seconds_next < seconds_now )
    _Watchdog_Adjust_seconds( WATCHDOG_BACKWARD, seconds_now - seconds_next );
  else
    _Watchdog_Adjust_seconds( WATCHDOG_FORWARD, seconds_next - seconds_now );

  _TOD.now = *tod;
  _TOD.seconds_trigger = nanoseconds;
  _TOD.is_set = true;

  _TOD_Activate();
  _Thread_Enable_dispatch();
}
