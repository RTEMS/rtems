/*
 *  Time of Day (TOD) Handler - get TOD
 *
 *  COPYRIGHT (c) 1989-2008.
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
#include <rtems/score/isr.h>
#include <rtems/score/timespec.h>
#include <rtems/score/timestamp.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

/*
 *  _TOD_Get
 *
 *  This routine is used to obtain the current date and time.
 *
 *  Input parameters:
 *    time  - pointer to the time and date structure
 *
 *  Output parameters: NONE
 */

void _TOD_Get(
  struct timespec *time
)
{
  ISR_Level         level;
  Timestamp_Control offset;
  Timestamp_Control now;
  long              nanoseconds;

  /* assume time checked for NULL by caller */

  /* _TOD_Now is the native current time */
  nanoseconds = 0;
  _ISR_Disable( level );
    now = _TOD_Now;
    if ( _Watchdog_Nanoseconds_since_tick_handler )
      nanoseconds = (*_Watchdog_Nanoseconds_since_tick_handler)();
  _ISR_Enable( level );

  _Timestamp_Set( &offset, 0, nanoseconds );
  _Timestamp_Add_to( &now, &offset );
  _Timestamp_To_timespec( &now, time );
}
