/*
 *  Time of Day (TOD) Handler -- Set Time
 */

/*  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

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
 *    time                - pointer to the time and date structure
 *
 *  Output parameters: NONE
 */

void _TOD_Set(
  const struct timespec *time
)
{
  _Thread_Disable_dispatch();
  _TOD_Deactivate();

  if ( time->tv_sec < _TOD_Seconds_since_epoch )
    _Watchdog_Adjust_seconds( WATCHDOG_BACKWARD,
       _TOD_Seconds_since_epoch - time->tv_sec );
  else
    _Watchdog_Adjust_seconds( WATCHDOG_FORWARD,
       time->tv_sec - _TOD_Seconds_since_epoch );

  /* POSIX format TOD (timespec) */
  _TOD_Now                 = *time;
  _TOD_Is_set              = TRUE;

  _TOD_Activate();

  _Thread_Enable_dispatch();
}
