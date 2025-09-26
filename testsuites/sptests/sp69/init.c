/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
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

#include <rtems/cpuuse.h>
#include <tmacros.h>
#include "test_support.h"

const char rtems_test_name[] = "SP 69";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_id                                period_id;
  rtems_name                              period_name;
  rtems_rate_monotonic_period_status      period_status;
  rtems_status_code                       status;
  rtems_rate_monotonic_period_statistics  statistics;
  int                                     i;

  period_name = rtems_build_name('P','E','R','1');

  TEST_BEGIN();

  /* create period */
  status = rtems_rate_monotonic_create(
      period_name,
      &period_id
  );
  directive_failed( status, "rate_monotonic_create" );

  /*
   * Check get_status on an inactive period.
   */
  puts(
    "rtems_rate_monotonic_get_status - verify values of an inactive period"
  );

  status = rtems_rate_monotonic_get_status( period_id, &period_status );
  directive_failed( status, "rate_monotonic_get_status" );

  /* Check status values. */
  rtems_test_assert( period_status.owner == rtems_task_self() );
  rtems_test_assert( period_status.state == RATE_MONOTONIC_INACTIVE );
  rtems_test_assert( period_status.since_last_period.tv_sec == 0 );
  rtems_test_assert( period_status.since_last_period.tv_nsec == 0 );
  rtems_test_assert( period_status.executed_since_last_period.tv_sec == 0 );
  rtems_test_assert( period_status.executed_since_last_period.tv_nsec == 0 );

  /* Clean up. */
  status = rtems_rate_monotonic_cancel( period_id );
  directive_failed( status, "rate_monotonic_cancel" );

  /*
   * Check normal get_status results.
   */
  puts( "rtems_rate_monotonic_get_status - verify values of an active period" );
  rtems_test_spin_until_next_tick();
  status = rtems_rate_monotonic_period( period_id, 100 );
  directive_failed( status, "rate_monotonic_period" );

  /* Do some work */
  rtems_test_spin_for_ticks( 10 );

  /* Block a little */
  status = rtems_task_wake_after( 50 );

  /* Check the status */
  status = rtems_rate_monotonic_get_status( period_id, &period_status );
  directive_failed( status, "rate_monotonic_get_status" );

  /* Check status values. */
  /* Note: POSIX mandates struct timespec->tv_nsec to be a "long" */
  printf(
    "wall time should be ~600000000 is %ld\n",
    period_status.since_last_period.tv_nsec
  );
  printf(
    "cpu time should be ~100000000 is %ld\n",
    period_status.executed_since_last_period.tv_nsec
  );
  rtems_test_assert( period_status.since_last_period.tv_sec == 0 );
  rtems_test_assert( period_status.since_last_period.tv_nsec >= 600000000 );
  rtems_test_assert( period_status.since_last_period.tv_nsec <= 610000000 );
  rtems_test_assert( period_status.executed_since_last_period.tv_sec == 0 );
  rtems_test_assert(
    period_status.executed_since_last_period.tv_nsec >= 100000000
  );
  rtems_test_assert(
    period_status.executed_since_last_period.tv_nsec <= 110000000
  );

  /* ensure the missed periods are properly accounted for */
  puts( "rtems_rate_monotonic_cancel -  OK" );
  status = rtems_rate_monotonic_cancel( period_id );
  directive_failed( status, "rate_monotonic_cancel" );

  puts( "Testing statistics on missed periods" );
  rtems_test_spin_until_next_tick();
  status = rtems_rate_monotonic_period( period_id, 50 );
  directive_failed( status, "rate_monotonic_period above loop" );

  for ( i=1 ; i <= 3 ; i++ ) {
    status = rtems_task_wake_after( 100 );
    directive_failed( status, "rtems_task_wake_after(100)" );

    rtems_test_spin_until_next_tick();
    status = rtems_rate_monotonic_period( period_id, 50 );
    fatal_directive_status(
      status,
      RTEMS_TIMEOUT,
      "rtems_rate_monotonic_period 2-n"
    );

    status = rtems_rate_monotonic_get_statistics( period_id, &statistics );
    directive_failed( status, "rate_monotonic_get_statistics" );
    if ( statistics.missed_count != i ) {
      printf(
        "Expected %d got %" PRIu32 " for missed_count\n",
        i,
        statistics.missed_count
      );
    }

    rtems_test_assert( statistics.missed_count == i );
  }

  /* Check the status */
  status = rtems_rate_monotonic_get_status( period_id, &period_status );
  directive_failed( status, "rate_monotonic_get_status" );
  puts(
    "rtems_rate_monotonic_get_status - verify value of a postponed jobs count"
  );
  rtems_test_assert( period_status.postponed_jobs_count == 3 );

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_MAXIMUM_PERIODS           1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE


#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
