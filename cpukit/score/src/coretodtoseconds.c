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
 *  _TOD_To_seconds
 *
 *  This routine returns the seconds from the epoch until the
 *  current date and time.
 *
 *  Input parameters:
 *    the_tod - pointer to the time and date structure
 *
 *  Output parameters:
 *    returns    - seconds since epoch until the_tod
 */

unsigned32 _TOD_To_seconds(
  TOD_Control *the_tod
)
{
  unsigned32 time;
  unsigned32 year_mod_4;

  time = the_tod->day - 1;
  year_mod_4 = the_tod->year & 3;

  if ( year_mod_4 == 0 )
    time += _TOD_Days_to_date[ 1 ][ the_tod->month ];
  else
    time += _TOD_Days_to_date[ 0 ][ the_tod->month ];

  time += ( (the_tod->year - TOD_BASE_YEAR) / 4 ) *
            ( (TOD_DAYS_PER_YEAR * 4) + 1);

  time += _TOD_Days_since_last_leap_year[ year_mod_4 ];

  time *= TOD_SECONDS_PER_DAY;

  time += ((the_tod->hour * TOD_MINUTES_PER_HOUR) + the_tod->minute)
             * TOD_SECONDS_PER_MINUTE;

  time += the_tod->second;

  return( time );
}

