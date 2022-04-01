/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  This routine is the initialization task of test to exercise some
 *  timezone functionality.
 */

/*
 *  COPYRIGHT (c) 2007-2012.
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

#define CONFIGURE_INIT

#include <tmacros.h>
#include <time.h>
#include <stdlib.h>

const char rtems_test_name[] = "TZTEST";
 
/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void tztester(void);

void tztester(void)
{
  struct tm *tm;
  time_t now;

  printf("TZ:\"%s\"\n", getenv("TZ"));

  time(&now);
  tm = localtime(&now);
  printf ("%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d\n",
     1900+tm->tm_year, tm->tm_mon+1, tm->tm_mday,
     tm->tm_hour, tm->tm_min, tm->tm_sec);

  tm = gmtime(&now);
  printf ("%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d\n",
     1900+tm->tm_year, tm->tm_mon+1, tm->tm_mday,
     tm->tm_hour, tm->tm_min, tm->tm_sec);
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_time_of_day time;
  rtems_status_code status;

  TEST_BEGIN();

  build_time( &time, 3, 14, 2007, 15, 9, 26, 5 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  setenv( "TZ", "CST6CDT,M 3.2.0,M 11.1.0", 1 );

  tzset();

  tztester();

  TEST_END();
  exit(0);
}


#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 1

#include <rtems/confdefs.h>
