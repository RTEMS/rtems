/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2009.
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

#include <tmacros.h>
#include <rtems/cpuuse.h>

const char rtems_test_name[] = "SP 46";

rtems_task Periodic_Task(
  rtems_task_argument argument
);
rtems_task Init(
  rtems_task_argument argument
);

volatile int partial_loop = 0;

rtems_task Periodic_Task(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code  status;
  rtems_name         period_name = rtems_build_name('P','E','R','a');
  rtems_id           period_id;
  rtems_interval     start;
  rtems_interval     end;

  puts( "Periodic - Create Period" );
  /* create period */
  status = rtems_rate_monotonic_create( period_name, &period_id );
  directive_failed(status, "rate_monotonic_create");

  partial_loop = 0;
  while (1) {
    /* start period with initial value */
    status = rtems_rate_monotonic_period( period_id, 25 );
    directive_failed(status, "rate_monotonic_period");
    partial_loop = 0;

    start = rtems_clock_get_ticks_since_boot();
    end   = start + 5;
    while ( end <= rtems_clock_get_ticks_since_boot() )
      ;

    partial_loop = 1;

    rtems_task_wake_after( 5 );
  }

  puts( "Periodic - Deleting self" );
  rtems_task_exit();
}

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code  status;
  rtems_id           task_id;

  TEST_BEGIN();

  /*
   * Initialize Tasks
   */


  puts( "INIT - rtems_task_create - creating task 1" );
  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '1', ' ' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  directive_failed( status, "rtems_task_create of TA1" );

  puts( "INIT - rtems_task_start - TA1 " );
  status = rtems_task_start( task_id, Periodic_Task, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  while ( !partial_loop ) {
    status = rtems_task_wake_after( 2 );
    directive_failed( status, "rtems_task_wake_after" );
  }

  rtems_cpu_usage_reset();

  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  /*
   *  Exit test
   */
  TEST_END();
  rtems_test_exit( 0 );
}

#define CONFIGURE_INIT
/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

/* Two Tasks: Init and Timer Server */
#define CONFIGURE_MAXIMUM_TASKS           2
#define CONFIGURE_MAXIMUM_PERIODS         1
#define CONFIGURE_INIT_TASK_STACK_SIZE    (RTEMS_MINIMUM_STACK_SIZE * 2)
#define CONFIGURE_INIT_TASK_PRIORITY      10
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS       (1 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>

