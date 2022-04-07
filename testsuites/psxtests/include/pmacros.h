/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2009, 2017.
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

#ifndef __POSIX_TEST_MACROS_h
#define __POSIX_TEST_MACROS_h

#if defined(__rtems__)
#include <bsp.h>
#endif
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <tmacros.h>
#include <buffer_test_io.h>

/*
 *  These help manipulate the "struct tm" form of time
 */

#define TM_SUNDAY    0
#define TM_MONDAY    1
#define TM_TUESDAY   2
#define TM_WEDNESDAY 3
#define TM_THURSDAY  4
#define TM_FRIDAY    5
#define TM_SATURDAY  6

#define TM_JANUARY     0
#define TM_FEBRUARY    1
#define TM_MARCH       2
#define TM_APRIL       3
#define TM_MAY         4
#define TM_JUNE        5
#define TM_JULY        6
#define TM_AUGUST      7
#define TM_SEPTEMBER   8
#define TM_OCTOBER     9
#define TM_NOVEMBER   10
#define TM_DECEMBER   11

#ifndef tm_build_time
#define tm_build_time( TM, WEEKDAY, MON, DAY, YR, HR, MIN, SEC ) \
  { (TM)->tm_year = YR;  \
    (TM)->tm_mon  = MON; \
    (TM)->tm_mday = DAY; \
    (TM)->tm_wday  = WEEKDAY; \
    (TM)->tm_hour = HR;  \
    (TM)->tm_min  = MIN; \
    (TM)->tm_sec  = SEC; }
#endif

#define set_time( WEEKDAY, MON, DAY, YR, HR, MIN, SEC ) \
  do { \
    struct tm tm; \
    struct timespec tv; \
    int status; \
    \
    tm_build_time( &tm, WEEKDAY, MON, DAY, YR, HR, MIN, SEC ); \
    \
    tv.tv_sec = mktime( &tm ); \
    tv.tv_nsec = 0; \
    rtems_test_assert(  tv.tv_sec != -1 ); \
    \
    status = clock_settime( CLOCK_REALTIME, &tv ); \
    rtems_test_assert(  !status ); \
  } while ( 0 )

#define print_current_time(s1, s2) \
  do { \
    char _time_buffer[32]; \
    int  _status; \
    struct timespec _tv; \
    \
    _status = clock_gettime( CLOCK_REALTIME, &_tv ); \
    rtems_test_assert(  !_status ); \
    \
    (void) ctime_r( &_tv.tv_sec, _time_buffer ); \
    _time_buffer[ strlen( _time_buffer ) - 1 ] = 0; \
    printf( "%s%s%s\n", s1, _time_buffer, s2 ); \
  } while ( 0 )

#define empty_line() puts( "" )

#endif

/* end of file */
