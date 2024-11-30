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

rtems_status_code rtems_clock_get_tod( rtems_time_of_day *time_of_day )
{
  struct timeval now;
  struct tm      buf;
  struct tm     *current_time_tm;

  if ( !time_of_day ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( !_TOD_Is_set() ) {
    return RTEMS_NOT_DEFINED;
  }

  /* Obtain the current time */
  _TOD_Get_timeval( &now );

  current_time_tm = gmtime_r( &now.tv_sec, &buf );
  _Assert( current_time_tm != NULL );

  time_of_day->year = 1900 + current_time_tm->tm_year;
  time_of_day->month = 1 + current_time_tm->tm_mon;
  time_of_day->day = current_time_tm->tm_mday;
  time_of_day->hour = current_time_tm->tm_hour;
  time_of_day->minute = current_time_tm->tm_min;
  time_of_day->second = current_time_tm->tm_sec;
  time_of_day->ticks = now.tv_usec /
                       rtems_configuration_get_microseconds_per_tick();

  return RTEMS_SUCCESSFUL;
}
