/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/cpuuse.h>
#include <tmacros.h>
#include "test_support.h"

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_id                                period_id;
  rtems_name                              period_name;
  rtems_rate_monotonic_period_status      period_status;
  rtems_status_code                       status;
  rtems_rate_monotonic_period_statistics  statistics;
  int                                     i;

  period_name = rtems_build_name('P','E','R','1');

  puts( "\n\n*** TEST 69 ***" );

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
  #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
    rtems_test_assert( period_status.since_last_period.tv_sec == 0 );
    rtems_test_assert( period_status.since_last_period.tv_nsec == 0 );
    rtems_test_assert( period_status.executed_since_last_period.tv_sec == 0 );
    rtems_test_assert( period_status.executed_since_last_period.tv_nsec == 0 );
  #else
    rtems_test_assert( period_status.since_last_period == 0 );
    rtems_test_assert( period_status.executed_since_last_period == 0 );
  #endif

  /*
   * Check get_status error cases.
   */
  puts( "rtems_rate_monotonic_get_status - check RTEMS_NOT_DEFINED" );

  /* Do some work to get a non-zero cpu usage */
  rtems_test_spin_for_ticks( 10 );

  status = rtems_rate_monotonic_period( period_id, 100 );
  directive_failed( status, "rate_monotonic_period" );

  /* Do some more work */
  rtems_test_spin_for_ticks( 10 );

  /* Reset the cpu usage statistics. */
  rtems_cpu_usage_reset();

  /* Status should be undefined. */
  status = rtems_rate_monotonic_get_status( period_id, &period_status );
  fatal_directive_status(
    status,
    RTEMS_NOT_DEFINED,
    "rtems_rate_monotonic_get_status after cpu usage reset"
  );

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
  #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
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
  #else
    printf(
      "wall time should be ~60 is %" PRId32 "\n",
      (int) period_status.since_last_period
    );
    printf(
      "cpu time should be ~10 is %" PRId32 "\n",
      (int) period_status.executed_since_last_period
    );
    rtems_test_assert( period_status.since_last_period >= 60 );
    rtems_test_assert( period_status.since_last_period <= 61 );
    rtems_test_assert( period_status.executed_since_last_period >= 10 );
    rtems_test_assert( period_status.executed_since_last_period <= 12 );
  #endif

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
  
  puts( "*** END OF TEST 69 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MILLISECONDS_PER_TICK 1

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_MAXIMUM_PERIODS           1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE


#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
