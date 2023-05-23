/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2011.
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
#include "test_support.h"

const char rtems_test_name[] = "SMP 7";

volatile bool TaskRan = false;
volatile bool TSRFired = false;
rtems_id      Semaphore;

rtems_task Init(
  rtems_task_argument argument
);

rtems_task Test_task(
  rtems_task_argument argument
);

static void success(void)
{
  TEST_END( );
  rtems_test_exit( 0 );
}

rtems_task Test_task(
  rtems_task_argument argument
)
{
  uint32_t          cpu_num;
  rtems_status_code sc;
  char              name[5];
  char             *p;

  /* Get the task name */
  p = rtems_object_get_name( RTEMS_SELF, 5, name );
  rtems_test_assert( p != NULL );

   /* Get the CPU Number */
  cpu_num = rtems_scheduler_get_processor();

  /* Print that the task is up and running. */
  locked_printf(" CPU %" PRIu32 " runnng Task %s and blocking\n", cpu_num, name);

  sc = rtems_semaphore_obtain( Semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT );
  directive_failed( sc,"obtain in test task");

  if ( !TSRFired )
    locked_printf( "*** ERROR TSR DID NOT FIRE BUT TEST TASK AWAKE***" );

  /* Print that the task is up and running. */
  locked_printf(
    " CPU %" PRIu32 " running Task %s after semaphore release\n",
    cpu_num,
    name
  );

  TaskRan = true;

  rtems_task_exit();
}


static rtems_timer_service_routine TimerMethod(
  rtems_id  timer,
  void     *arg
)
{
  /*
   * Set flag and release the semaphore, allowing the blocked tasks to start.
   */
  TSRFired = true;

  rtems_semaphore_release( Semaphore );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  int                cpu_num;
  rtems_id           id;
  rtems_status_code  status;
  rtems_interval     per_second;
  rtems_interval     then;
  rtems_id           Timer;

  locked_print_initialize();
  TEST_BEGIN();

  if ( rtems_scheduler_get_processor_maximum() == 1 ) {
    success();
  }

  /* Create/verify semaphore */
  status = rtems_semaphore_create(
    rtems_build_name ('S', 'E', 'M', '1'),
    1,
    RTEMS_LOCAL                   |
    RTEMS_SIMPLE_BINARY_SEMAPHORE |
    RTEMS_PRIORITY,
    1,
    &Semaphore
  );
  directive_failed( status, "rtems_semaphore_create" );

  /* Lock semaphore */
  status = rtems_semaphore_obtain( Semaphore, RTEMS_WAIT, 0);
  directive_failed( status,"rtems_semaphore_obtain of SEM1\n");

  /* Create and Start test task. */
  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '1', ' ' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "task create" );

  cpu_num = rtems_scheduler_get_processor();
  locked_printf(" CPU %d start task TA1\n", cpu_num );
  status = rtems_task_start( id, Test_task, 1 );
  directive_failed( status, "task start" );

  /* Create and start TSR */
  locked_printf(" CPU %d create and start timer\n", cpu_num );
  status = rtems_timer_create( rtems_build_name( 'T', 'M', 'R', '1' ), &Timer);
  directive_failed( status, "rtems_timer_create" );

  per_second = rtems_clock_get_ticks_per_second();
  status = rtems_timer_fire_after( Timer, 2 * per_second, TimerMethod, NULL );
  directive_failed( status, "rtems_timer_fire_after");

  /*
   *  Wait long enough that TSR should have fired.
   *
   *  Spin so CPU 0 is consumed.  This forces task to run on CPU 1.
   */
  then = rtems_clock_get_ticks_since_boot() + 4 * per_second;
  while (1) {
    if ( rtems_clock_get_ticks_since_boot() > then )
      break;
    if ( TSRFired && TaskRan )
      break;
  };

  /* Validate the timer fired and that the task ran */
  if ( !TSRFired )
    locked_printf( "*** ERROR TSR DID NOT FIRE ***" );

  if ( !TaskRan ) {
    locked_printf( "*** ERROR TASK DID NOT RUN ***" );
    rtems_test_exit(0);
  }

  /* End the program */
  success();
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS   2
#define CONFIGURE_MAXIMUM_TIMERS           1

#define CONFIGURE_MAXIMUM_TASKS            2
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_SEMAPHORES       2

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
