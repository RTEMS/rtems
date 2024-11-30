/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * This test exercises the get time of day and set time of day
 * 
 */

/*
 * Copyright (C) 2025 Zak Leung <zakthertemsdev@gmail.com>
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

#define CONFIGURE_INIT
#include "system.h"
#include <tmacros.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <rtems.h>
#include <rtems/libio.h>
#include <sys/time.h>

const char rtems_test_name[] = "SP TOD";

void check_a_tod( rtems_time_of_day *the_tod );

/*
 *  List of dates and times to test.
 */
#define NUMBER_OF_DATES 9
rtems_time_of_day Dates[ NUMBER_OF_DATES ] = {
  /* YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, TICKS */
  { 1988, 1, 1, 12, 45, 00, 0 },
  { 1988, 12, 31, 9, 00, 00, 0 },
  { 1999, 12, 31, 23, 55, 59, 0 },
  { 1999, 06, 30, 00, 01, 30, 0 },
  { 2000, 1, 1, 0, 15, 59, 0 },
  { 2005, 2, 2, 5, 10, 59, 0 },
  { 2010, 3, 3, 10, 5, 59, 0 },
  { 2020, 4, 4, 15, 0, 59, 0 },
  { 2199, 4, 4, 15, 0, 59, 0 }
};

/*
 *  Check out a single date and time
 */

void check_a_tod( rtems_time_of_day *the_tod )
{
  rtems_status_code status;
  rtems_time_of_day new_tod;
  status = rtems_clock_set( the_tod );
  rtems_test_assert( !status );

  status = rtems_clock_get_tod( &new_tod );
  rtems_test_assert( !status );

  rtems_test_assert( new_tod.year == the_tod->year );
  rtems_test_assert( new_tod.month == the_tod->month );
  rtems_test_assert( new_tod.day == the_tod->day );
  rtems_test_assert( new_tod.hour == the_tod->hour );
  rtems_test_assert( new_tod.minute == the_tod->minute );
  rtems_test_assert( new_tod.second == the_tod->second );
  rtems_test_assert( new_tod.ticks == the_tod->ticks );
}

/*
 *  main entry point to the test
 */

rtems_task Init( rtems_task_argument argument )
{
  (void) argument;

  int i;

  TEST_BEGIN();

  for ( i = 0; i < NUMBER_OF_DATES; i++ ) {
    check_a_tod( &Dates[ i ] );
  }

  TEST_END();
  rtems_test_exit( 0 );
}
