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
  TOD_Control *the_tod
)
{
  unsigned32 days_in_month;

  if ((the_tod->ticks  >= _TOD_Ticks_per_second)  ||
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

