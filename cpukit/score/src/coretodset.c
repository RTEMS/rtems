/*
 *  Time of Day (TOD) Handler
 *
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
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

/*PAGE
 *
 *  _TOD_Set
 *
 *  This rountine sets the current date and time with the specified
 *  new date and time structure.
 *
 *  Input parameters:
 *    the_tod             - pointer to the time and date structure
 *    seconds_since_epoch - seconds since system epoch
 *
 *  Output parameters: NONE
 */

void _TOD_Set(
  TOD_Control *the_tod,
  Watchdog_Interval  seconds_since_epoch
)
{
  Watchdog_Interval ticks_until_next_second;

  _Thread_Disable_dispatch();
  _TOD_Deactivate();

  if ( seconds_since_epoch < _TOD_Seconds_since_epoch )
    _Watchdog_Adjust_seconds( WATCHDOG_BACKWARD,
       _TOD_Seconds_since_epoch - seconds_since_epoch );
  else
    _Watchdog_Adjust_seconds( WATCHDOG_FORWARD,
       seconds_since_epoch - _TOD_Seconds_since_epoch );

  ticks_until_next_second = _TOD_Ticks_per_second;
  if ( ticks_until_next_second > the_tod->ticks )
    ticks_until_next_second -= the_tod->ticks;

  _TOD_Current             = *the_tod;
  _TOD_Seconds_since_epoch = seconds_since_epoch;
  _TOD_Is_set              = TRUE;
  _TOD_Activate( ticks_until_next_second );

  _Thread_Enable_dispatch();
}

