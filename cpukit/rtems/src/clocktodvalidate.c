/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicClock
 *
 * @brief This source file contains the implementation of
 *   _TOD_Validate().
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
#include <rtems/config.h>

/*
 *  The following array contains the number of days in all months.
 *  The first dimension should be 0 for leap years, and 1 otherwise.
 *  The second dimension should range from 1 to 12 for January to
 *  December, respectively.
 */
static const uint32_t _TOD_Days_per_month[ 2 ][ 13 ] = {
  { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
  { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

rtems_status_code _TOD_Validate(
  const rtems_time_of_day *the_tod,
  TOD_Ticks_validation     ticks_validation
)
{
  size_t  leap_year_index;
  uint32_t days_in_month;
  uint32_t ticks_per_second;
  uint32_t ticks_mask;

  if ( the_tod == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  ticks_per_second = rtems_clock_get_ticks_per_second();
  ticks_mask = (uint32_t) ticks_validation;

  if ( ( the_tod->ticks & ticks_mask ) >= ticks_per_second ) {
    return RTEMS_INVALID_CLOCK;
  }

  if ( the_tod->second >= TOD_SECONDS_PER_MINUTE ) {
    return RTEMS_INVALID_CLOCK;
  }

  if ( the_tod->minute >= TOD_MINUTES_PER_HOUR ) {
    return RTEMS_INVALID_CLOCK;
  }

  if ( the_tod->hour >= TOD_HOURS_PER_DAY ) {
    return RTEMS_INVALID_CLOCK;
  }

  if ( the_tod->month == 0 || the_tod->month > TOD_MONTHS_PER_YEAR ) {
    return RTEMS_INVALID_CLOCK;
  }

  if ( the_tod->year < TOD_BASE_YEAR || the_tod->year > TOD_LATEST_YEAR ) {
    return RTEMS_INVALID_CLOCK;
  }

  if ( the_tod->day == 0 ) {
    return RTEMS_INVALID_CLOCK;
  }

  leap_year_index = _TOD_Get_leap_year_index( the_tod->year );
  days_in_month = _TOD_Days_per_month[ leap_year_index ][ the_tod->month ];

  if ( the_tod->day > days_in_month ) {
    return RTEMS_INVALID_CLOCK;
  }

  return RTEMS_SUCCESSFUL;
}
