/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This test checks rtems_cpu_usage_top command with
 *  30 tasks being created and deleted.  The command
 *  should show the task list grow to the top 20 tasks
 *  then shrink back down to 5 tasks.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 2014.
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
#include "system.h"

#include <rtems/shell.h>

const char rtems_test_name[] = "TOP";

/*
 * This method is called by Task_3 to provide
 * a variable lengh run time for each instance
 * of the task.
 */

void add_some(
  uint32_t  per_loop,
  uint32_t *sum,
  uint32_t *next
)
{
  int i;

  for ( i=0 ; i<per_loop ; i++ ) {
    *sum += *next;
    *next += 1;
  }
}

static void notification(int fd, int seconds_remaining, void *arg)
{
  (void) fd;
  (void) arg;

  printf(
    "Press any key to enter top test (%is remaining)\n",
    seconds_remaining
  );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code status;
  rtems_time_of_day time;

  TEST_BEGIN();

  status = rtems_shell_wait_for_input(
    STDIN_FILENO,
    20,
    notification,
    NULL
  );
  if ( status != RTEMS_SUCCESSFUL ) {
    TEST_END();

    rtems_test_exit( 0 );
  }

  build_time( &time, 12, 31, 1988, 9, 15, 0, 0 );

  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  TicksPerSecond = rtems_clock_get_ticks_per_second();
  if (TicksPerSecond <= 0) {
    printf(
      "Invalid ticks per second: %" PRIdrtems_interval "\n",
      TicksPerSecond
    );
    exit (1);
  }

  /* Create and start the task to run top command. */
  Task_name[ 2 ] =  rtems_build_name( 'T', 'A', '0', '2' );
  status = rtems_task_create(
     Task_name[ 2 ],
     2,
     RTEMS_MINIMUM_STACK_SIZE,
     RTEMS_TIMESLICE,
     RTEMS_FLOATING_POINT,
     &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TA02" );
  status = rtems_task_start( Task_id[ 2 ], Task_2, 0 );
  directive_failed( status, "rtems_task_start of TA2" );

  /* Create and start task to run the test. */
  Task_name[ 1 ] =  rtems_build_name( 'T', 'A', '0', '1' );
  status = rtems_task_create(
     Task_name[ 1 ],
     2,
     RTEMS_MINIMUM_STACK_SIZE,
     RTEMS_TIMESLICE,
     RTEMS_FLOATING_POINT,
     &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA01" );
  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA01" );

  /*
   * We suspend the Init task rather than delete it so it still
   * shows up in the output.
   */
  status = rtems_task_suspend( RTEMS_SELF );
  directive_failed( status, "rtems_task_suspend of RTEMS_SELF" );
}
