/*
 *  Time of Day (TOD) Handler - Classic TOD to Seconds
 *
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/rtems/clock.h>

#define TOD_SECONDS_AT_2100_03_01_00_00 4107538800UL

/*
 *  The following array contains the number of days in all months
 *  up to the month indicated by the index of the second dimension.
 *  The first dimension should be 1 for leap years, and 0 otherwise.
 */
const uint16_t   _TOD_Days_to_date[2][13] = {
  { 0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
  { 0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 }
};

/*
 *  The following array contains the number of days in the years
 *  since the last leap year.  The index should be 0 for leap
 *  years, and the number of years since the beginning of a leap
 *  year otherwise.
 */
const uint16_t   _TOD_Days_since_last_leap_year[4] = { 0, 366, 731, 1096 };



/*
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

uint32_t   _TOD_To_seconds(
  const rtems_time_of_day *the_tod
)
{
  uint32_t   time;
  uint32_t   year_mod_4;

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

  /* The year 2100 is not a leap year */
  if ( time
      >= (TOD_SECONDS_AT_2100_03_01_00_00 - TOD_SECONDS_1970_THROUGH_1988)) {
    time -= TOD_SECONDS_PER_DAY;
  }

  time += TOD_SECONDS_1970_THROUGH_1988;

  return( time );
}
