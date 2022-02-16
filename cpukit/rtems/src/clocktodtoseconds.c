/* SPDX-License-Identifier: BSD-2-Clause */

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
