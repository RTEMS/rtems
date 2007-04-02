/*
 *  Time of Day (TOD) Handler -- Validate Classic TOD
 *
 *
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/rtems/clock.h>

/*
 *  The following array contains the number of days in all months.
 *  The first dimension should be 1 for leap years, and 0 otherwise.
 *  The second dimension should range from 1 to 12 for January to
 *  February, respectively.
 */
const uint32_t   _TOD_Days_per_month[ 2 ][ 13 ] = {
  { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
  { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

/*PAGE
 *
 *  _TOD_Validate
 *
 *  This kernel routine checks the validity of a date and time structure.
 *
 *  Input parameters:
 *    the_tod - pointer to a time and date structure
 *
 *  Output parameters:
 *    TRUE  - if the date, time, and tick are valid
 *    FALSE - if the the_tod is invalid
 *
 *  NOTE: This routine only works for leap-years through 2099.
 */

boolean _TOD_Validate(
  rtems_time_of_day *the_tod
)
{
  uint32_t   days_in_month;

  if ((!the_tod)                                  ||
      (the_tod->ticks  >= _TOD_Ticks_per_second)  ||
      (the_tod->second >= TOD_SECONDS_PER_MINUTE) ||
      (the_tod->minute >= TOD_MINUTES_PER_HOUR)   ||
      (the_tod->hour   >= TOD_HOURS_PER_DAY)      ||
      (the_tod->month  == 0)                      ||
      (the_tod->month  >  TOD_MONTHS_PER_YEAR)    ||
      (the_tod->year   <  TOD_BASE_YEAR)          ||
      (the_tod->day    == 0) )
     return FALSE;

  if ( (the_tod->year % 4) == 0 )
    days_in_month = _TOD_Days_per_month[ 1 ][ the_tod->month ];
  else
    days_in_month = _TOD_Days_per_month[ 0 ][ the_tod->month ];

  if ( the_tod->day > days_in_month )
    return FALSE;

  return TRUE;
}
