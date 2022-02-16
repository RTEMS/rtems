/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicClock
 *
 * @brief This source file contains the implementation of
 *   rtems_clock_get_tod().
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
    days_to_date = _TOD_Days_to_date[0];
  else
    days_to_date = _TOD_Days_to_date[1];

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
  rtems_time_of_day  *time_of_day
)
{
  struct timeval now;
  uint32_t       days;
  uint32_t       day_secs;
  uint32_t       year;
  uint32_t       year_days;
  uint32_t       leap_years;

  if ( !time_of_day )
    return RTEMS_INVALID_ADDRESS;

  if ( !_TOD_Is_set() )
    return RTEMS_NOT_DEFINED;

  /* Obtain the current time */
  _TOD_Get_timeval( &now );

  /*
   * How many days and how many seconds in the day?
   *
   * A 32-bit integer can represent enough days for several 1000 years.  When
   * the current time is valid, the integer conversions below are well defined.
   */
  days = (uint32_t) ( now.tv_sec / RTEMS_SECS_PER_DAY );
  day_secs = (uint32_t) ( now.tv_sec % RTEMS_SECS_PER_DAY );

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

  time_of_day->year   = year;
  time_of_day->month  = _Year_day_as_month( year, &year_days ) + 1;
  time_of_day->day    = year_days + 1;
  time_of_day->hour   = day_secs / RTEMS_SECS_PER_HOUR;
  time_of_day->minute = day_secs % RTEMS_SECS_PER_HOUR;
  time_of_day->second = time_of_day->minute % RTEMS_SECS_PER_MINUTE;
  time_of_day->minute = time_of_day->minute / RTEMS_SECS_PER_MINUTE;
  time_of_day->ticks  = now.tv_usec /
    rtems_configuration_get_microseconds_per_tick( );

  return RTEMS_SUCCESSFUL;
}
