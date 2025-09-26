/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * COPYRIGHT (c) 2017 Kuan-Hsun Chen.
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
#include <rtems/rtems/ratemonimpl.h>
#include "test_support.h"

const char rtems_test_name[] = "SPRMSCHED 2";

/* forward declarations to avoid warnings */
rtems_task Init( rtems_task_argument argument );
static void modify_count( rtems_id id );

static void modify_count(
  rtems_id            id
)
{
  Rate_monotonic_Control                  *the_period;
  ISR_lock_Context                        lock_context;

  the_period = _Rate_monotonic_Get( id, &lock_context );
  _Rate_monotonic_Acquire_critical( the_period, &lock_context );
  the_period->postponed_jobs = UINT32_MAX;
  _Rate_monotonic_Release( the_period, &lock_context );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_id                                period_id;
  rtems_name                              period_name;
  rtems_rate_monotonic_period_status      period_status;
  rtems_status_code                       status;
  int                                     i;

  period_name = rtems_build_name('P','E','R','1');

  TEST_BEGIN();

  /* create period */
  status = rtems_rate_monotonic_create(
      period_name,
      &period_id
  );
  directive_failed( status, "rate_monotonic_create" );

  /* modify the count to UINT32_MAX and attempt to miss deadline*/
  puts( "Testing overflow condition" );
  rtems_test_spin_until_next_tick();
  status = rtems_rate_monotonic_period( period_id, 50 );
  directive_failed( status, "rate_monotonic_period above loop" );

  puts( "Modify the count of postponed_job manually" );
  modify_count( period_id );

  /* Check the status */
  status = rtems_rate_monotonic_get_status( period_id, &period_status );
  directive_failed( status, "rate_monotonic_get_status" );
  printf( "Init Postponed jobs = %"PRIu32", and expected %"PRIu32"\n", period_status.postponed_jobs_count, UINT32_MAX );
  rtems_test_assert( period_status.postponed_jobs_count == UINT32_MAX );

  for ( i=1 ; i <= 2 ; i++ ) {
    status = rtems_task_wake_after( 100 );
    directive_failed( status, "rtems_task_wake_after(100)" );
    puts( "Task misses its deadline." );

    /* Check the status */
    status = rtems_rate_monotonic_get_status( period_id, &period_status );
    directive_failed( status, "rate_monotonic_get_status" );

    /* print out the count which should keep in UINT32_MAX, since the period still misses its deadline */
    printf( "Count = %"PRIu32", and expected %"PRIu32"\n", period_status.postponed_jobs_count, UINT32_MAX);
    rtems_test_assert( period_status.postponed_jobs_count == UINT32_MAX);

    rtems_test_spin_until_next_tick();
    status = rtems_rate_monotonic_period( period_id, 50 );
    fatal_directive_status(
      status,
      RTEMS_TIMEOUT,
      "rtems_rate_monotonic_period 2-n"
    );


  }

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
