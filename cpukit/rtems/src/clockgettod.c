/**
 * @file
 *
 * @ingroup ClassicClock Clocks
 *
 * @brief Obtain Current Time of Day (Classic TOD)
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

#include <rtems/rtems/clock.h>
#include <rtems/score/todimpl.h>
#include <rtems/config.h>

#define RTEMS_SECS_PER_MINUTE (60UL)
#define RTEMS_MINUTE_PER_HOUR (60UL)
#define RTEMS_SECS_PER_HOUR   (RTEMS_SECS_PER_MINUTE * RTEMS_MINUTE_PER_HOUR)
#define RTEMS_HOURS_PER_DAY   (24UL)
#define RTEMS_SECS_PER_DAY    (RTEMS_SECS_PER_HOUR * RTEMS_HOURS_PER_DAY)
#define RTEMS_DAYS_PER_YEAR   (365UL)
#define RTEMS_YEAR_BASE       (1970UL)

extern const uint16_t _TOD_Days_to_date[2][13];

static bool _Leap_year(
  uint32_t year
)
{
  return (((year % 4) == 0) && ((year % 100) != 0)) || ((year % 400) == 0);
}

static uint32_t _Leap_years_before(
  uint32_t year
)
{
  year -= 1;
  return (year / 4) - (year / 100) + (year / 400);
}

static uint32_t _Leap_years_between(
  uint32_t from, uint32_t to
)
{
  return _Leap_years_before( to ) - _Leap_years_before( from + 1 );
}

static uint32_t _Year_day_as_month(
  uint32_t year, uint32_t *day
)
{
  const uint16_t* days_to_date;
  uint32_t        month = 0;

  if ( _Leap_year( year ) )
    days_to_date = _TOD_Days_to_date[1];
  else
    days_to_date = _TOD_Days_to_date[0];

  days_to_date += 2;

  while (month < 11) {
    if (*day < *days_to_date)
      break;
    ++month;
    ++days_to_date;
  }

  *day -= *(days_to_date - 1);

  return month;
}

rtems_status_code rtems_clock_get_tod(
  rtems_time_of_day  *time_buffer
)
{
  struct timeval now;
  uint32_t       days;
  uint32_t       day_secs;
  uint32_t       year;
  uint32_t       year_days;
  uint32_t       leap_years;

  if ( !time_buffer )
    return RTEMS_INVALID_ADDRESS;

  if ( !_TOD_Is_set() )
    return RTEMS_NOT_DEFINED;

  /* Obtain the current time */
  _TOD_Get_timeval( &now );

  /* How many days and how many seconds in the day ? */
  days = now.tv_sec / RTEMS_SECS_PER_DAY;
  day_secs = now.tv_sec % RTEMS_SECS_PER_DAY;

  /* How many non-leap year years ? */
  year = ( days / RTEMS_DAYS_PER_YEAR ) + RTEMS_YEAR_BASE;

  /* Determine the number of leap years. */
  leap_years = _Leap_years_between( RTEMS_YEAR_BASE, year );

  /* Adjust the remaining number of days based on the leap years. */
  year_days = ( days - leap_years ) % RTEMS_DAYS_PER_YEAR;

  /* Adjust the year and days in the year if in the leap year overflow. */
  if ( leap_years > ( days % RTEMS_DAYS_PER_YEAR ) ) {
    year -= 1;
    if ( _Leap_year( year ) ) {
      year_days += 1;
    }
  }

  time_buffer->year   = year;
  time_buffer->month  = _Year_day_as_month( year, &year_days ) + 1;
  time_buffer->day    = year_days + 1;
  time_buffer->hour   = day_secs / RTEMS_SECS_PER_HOUR;
  time_buffer->minute = day_secs % RTEMS_SECS_PER_HOUR;
  time_buffer->second = time_buffer->minute % RTEMS_SECS_PER_MINUTE;
  time_buffer->minute = time_buffer->minute / RTEMS_SECS_PER_MINUTE;
  time_buffer->ticks  = now.tv_usec /
    rtems_configuration_get_microseconds_per_tick( );

  return RTEMS_SUCCESSFUL;
}
