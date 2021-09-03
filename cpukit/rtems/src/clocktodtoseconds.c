/**
 * @file
 *
 * @ingroup RTEMSImplClassicClock
 *
 * @brief This source file contains the implementation of
 *   _TOD_To_seconds().
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/clockimpl.h>
#include <rtems/score/todimpl.h>

const uint16_t _TOD_Days_to_date[ 2 ][ 13 ] = {
  { 0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 },
  { 0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 }
};

/*
 *  The following array contains the number of days in the years
 *  since the last leap year.  The index should be 0 for leap
 *  years, and the number of years since the beginning of a leap
 *  year otherwise.
 */
const uint16_t   _TOD_Days_since_last_leap_year[4] = { 0, 366, 731, 1096 };


Watchdog_Interval   _TOD_To_seconds(
  const rtems_time_of_day *the_tod
)
{
  uint32_t time;
  size_t   leap_year_index;

  time = the_tod->day - 1;

  leap_year_index = _TOD_Get_leap_year_index( the_tod->year );
  time += _TOD_Days_to_date[ leap_year_index ][ the_tod->month ];

  time += ( (the_tod->year - TOD_BASE_YEAR) / 4 ) *
            ( (TOD_DAYS_PER_YEAR * 4) + 1);

  time += _TOD_Days_since_last_leap_year[ the_tod->year % 4 ];

  time *= TOD_SECONDS_PER_DAY;

  time += ((the_tod->hour * TOD_MINUTES_PER_HOUR) + the_tod->minute)
             * TOD_SECONDS_PER_MINUTE;

  time += the_tod->second;
  time += TOD_SECONDS_1970_THROUGH_1988;

  return( time );
}
