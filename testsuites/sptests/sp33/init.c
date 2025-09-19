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

#include <tmacros.h>  /* includes bsp.h, stdio, etc... */

const char rtems_test_name[] = "SP 33";

/* prototype */
rtems_task Init (rtems_task_argument ignored);

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MAXIMUM_TASKS     5
#define CONFIGURE_MAXIMUM_BARRIERS  1

#define CONFIGURE_INIT

rtems_task Waiter(
  rtems_task_argument number
);

rtems_id    Barrier;

int SuccessfulCase;
int DeletedCase;

rtems_task Waiter(
  rtems_task_argument number
)
{
  rtems_status_code status;
  int               waiter = (int) number;

  printf( "Waiter %d waiting on barrier\n", waiter );
  status = rtems_barrier_wait( Barrier, RTEMS_NO_TIMEOUT );

  printf( "Waiter %d back from barrier\n", waiter );

  if ( SuccessfulCase == TRUE ) {
    directive_failed(status, "rtems_barrier_wait");
  } else if ( DeletedCase == TRUE ) {
    fatal_directive_status(
      status,
      RTEMS_OBJECT_WAS_DELETED,
      "rtems_barrier_wait did not get deleted"
    );
  }

  rtems_task_exit();
}

#include <rtems/confdefs.h>

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code status;
  rtems_name        name = rtems_build_name('B','A','R','1');
  uint32_t          released;
  uint32_t          waiting;
  rtems_id          testId;
  rtems_id          Tasks[CONFIGURE_MAXIMUM_TASKS-1];
  uint32_t          i;

  TEST_BEGIN();

  /* Check bad argument cases */
  puts( "rtems_barrier_delete - bad id - INVALID_ID" );
  status = rtems_barrier_delete( 100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_barrier_delete did not return RTEMS_INVALID_ID"
  );

  puts( "rtems_barrier_release - bad id - INVALID_ID" );
  status = rtems_barrier_release( 100, &released );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_barrier_release did not return RTEMS_INVALID_ID"
  );

  puts( "rtems_barrier_get_number_waiting - bad id - INVALID_ID" );
  status = rtems_barrier_get_number_waiting( 100, &waiting );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_barrier_get_number_waiting did not return RTEMS_INVALID_ID"
  );

  puts( "rtems_barrier_wait - bad id - INVALID_ID" );
  status = rtems_barrier_wait( 100, 10 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_barrier_wait did not return RTEMS_INVALID_ID"
  );

  /* Create barrier with automatic release and 0 maximum waiters */
  puts( "Create barrier with automatic release and 0 max waiters" );
  status = rtems_barrier_create(
    name, RTEMS_BARRIER_AUTOMATIC_RELEASE, 0, &Barrier
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_barrier_create did not return RTEMS_INVALID_NUMBER"
  );

  /* create barrier with bad name */
  puts( "rtems_barrier_create - bad name - INVALID_NAME" );
  status = rtems_barrier_create(
    0, RTEMS_BARRIER_AUTOMATIC_RELEASE, 1, &Barrier);
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_barrier_create did not return RTEMS_INVALID_NAME"
  );

  /* create barrier with bad id return address */
  puts( "rtems_barrier_create - NULL barrier ID - INVALID_ADDRESS" );
  status = rtems_barrier_create(name, RTEMS_BARRIER_AUTOMATIC_RELEASE, 1, NULL);
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_barrier_create did not return RTEMS_INVALID_ADDRESS"
  );

  /* Create barrier */
  puts( "rtems_barrier_create - OK" );
  status = rtems_barrier_create( name, RTEMS_DEFAULT_ATTRIBUTES, 0, &Barrier );
  directive_failed(status, "rtems_barrier_create");

  /* Check for creating too many */
  puts( "rtems_barrier_create - too many" );
  status = rtems_barrier_create( name, RTEMS_DEFAULT_ATTRIBUTES, 0, &Barrier );
  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_barrier_create did not return RTEMS_TOO_MANY"
  );

  puts( "Check barrier ident" );
  status = rtems_barrier_ident( name, &testId );
  directive_failed(status, "rtems_barrier_ident");
  if ( testId != Barrier ) {
    printf( "ERROR -- rtems_barrier_create -- did not get Id expected\n" );
    exit( 0 );
  }

  puts( "Wait on barrier w/timeout and TIMEOUT" );
  status = rtems_barrier_wait( Barrier, 25 );
  fatal_directive_status(
    status,
    RTEMS_TIMEOUT,
    "rtems_barrier_wait did not timeout"
  );

  /* Release with bad return pointer */
  puts( "rtems_barrier_release - NULL return count - INVALID_ADDRESS" );
  status = rtems_barrier_release( Barrier, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_barrier_release bad return pointer"
  );

  /* Release no tasks */
  status = rtems_barrier_release( Barrier, &released );
  directive_failed(status, "rtems_barrier_release");
  if ( released != 0 ) {
    printf(
      "ERROR -- rtems_barrier_release -- released != 0, = %" PRIu32,
      released
    );
    rtems_test_exit(0);
  }

  /* Get number waiting with bad return pointer */
  puts(
    "rtems_barrier_get_number_waiting - NULL return count - INVALID_ADDRESS"
  );
  status = rtems_barrier_get_number_waiting( Barrier, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_barrier_get_number_waiting bad return pointer"
  );

  /* number of waiting tasks must be 0 */
  status = rtems_barrier_get_number_waiting( Barrier, &waiting );
  directive_failed(status, "rtems_barrier_get_number_waiting");
  if ( waiting != 0 ) {
    printf(
      "ERROR -- rtems_barrier_get_number_waiting -- waiting != 0, = %" PRIu32,
      waiting
    );
    rtems_test_exit(0);
  }

  /*  Create some tasks to wait for the barrier */
  SuccessfulCase = TRUE;
  DeletedCase    = FALSE;
  puts( "\n*** Testing manual release of barrier ***" );
  for (i=0 ; i<(CONFIGURE_MAXIMUM_TASKS-1) ; i++) {
    status = rtems_task_create(
      rtems_build_name('W','A','I','T'),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Tasks[ i ]
    );
    directive_failed( status, "rtems_task_create of Waiter" );

    status = rtems_task_start( Tasks[ i ], Waiter, i );
    directive_failed( status, "rtems_task_start of Waiter" );
  }

  puts( "Delay to let Waiters block" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed(status, "rtems_task_wake_after");

  /* Gets the number of tasks that are waiting at the barrier */
  puts( "Getting the number of tasks that are waiting at the barrier" );
  status = rtems_barrier_get_number_waiting( Barrier, &waiting );
  printf("Number of tasks waiting at the barrier: %" PRIu32 "\n", waiting);
  directive_failed(status, "rtems_barrier_get_number_waiting" );
  if ( waiting != ( CONFIGURE_MAXIMUM_TASKS - 1 ) ) {
    printf(
      "ERROR -- rtems_barrier_get_number_waiting -- waiting != %d, = %" PRIu32,
      ( CONFIGURE_MAXIMUM_TASKS - 1 ),
      waiting
    );
    rtems_test_exit(0);
  }

  /* Release tasks which were waiting */
  puts( "Releasing tasks" );
  status = rtems_barrier_release( Barrier, &released );
  directive_failed(status, "rtems_barrier_release");
  if ( released != (CONFIGURE_MAXIMUM_TASKS-1) ) {
    printf(
      "ERROR -- rtems_barrier_release -- released != %d, = %" PRIu32,
      (CONFIGURE_MAXIMUM_TASKS-1),
      released
    );
    rtems_test_exit(0);
  }

  puts( "Delay to let Waiters print a message" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed(status, "rtems_task_wake_after");

  /* Gets the number of tasks that are waiting at the barrier */
  puts( "Getting the number of tasks that are waiting at the barrier" );
  status = rtems_barrier_get_number_waiting( Barrier, &waiting );
  printf("Number of tasks waiting at the barrier: %" PRIu32 "\n", waiting);
  directive_failed(status, "rtems_barrier_get_number_waiting" );
  if ( waiting != 0 ) {
    printf(
      "ERROR -- rtems_barrier_get_number_waiting -- waiting != %d, = %" PRIu32,
      0,
      waiting
    );
    rtems_test_exit(0);
  }


  /*  Create some tasks to wait for the barrier */
  SuccessfulCase = FALSE;
  DeletedCase    = TRUE;
  puts( "\n*** Testing Deletion of barrier ***" );
  for (i=0 ; i<(CONFIGURE_MAXIMUM_TASKS-1) ; i++) {
    status = rtems_task_create(
      rtems_build_name('W','A','I','T'),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Tasks[ i ]
    );
    directive_failed( status, "rtems_task_create of Waiter" );

    status = rtems_task_start( Tasks[ i ], Waiter, i );
    directive_failed( status, "rtems_task_start of Waiter" );
  }

  puts( "Delay to let Waiters block" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed(status, "rtems_task_wake_after");

  puts( "rtems_barrier_delete - OK" );
  status = rtems_barrier_delete( Barrier );
  directive_failed(status, "rtems_barrier_delete");

  puts( "Delay to let Waiters print a message" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed(status, "rtems_task_wake_after");

  /* Create barrier with automatic release */
  puts( "rtems_barrier_create - OK" );
  status = rtems_barrier_create(
    name, RTEMS_BARRIER_AUTOMATIC_RELEASE, CONFIGURE_MAXIMUM_TASKS-1, &Barrier
  );
  directive_failed(status, "rtems_barrier_create");

  /*  Create some tasks to wait for the barrier */
  SuccessfulCase = TRUE;
  DeletedCase    = FALSE;
  puts( "\n*** Testing automatic release of barrier ***" );
  for (i=0 ; i<(CONFIGURE_MAXIMUM_TASKS-1) ; i++) {
    status = rtems_task_create(
      rtems_build_name('W','A','I','T'),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Tasks[ i ]
    );
    directive_failed( status, "rtems_task_create of Waiter" );

    status = rtems_task_start( Tasks[ i ], Waiter, i );
    directive_failed( status, "rtems_task_start of Waiter" );
  }

  puts( "Delay to let task wait on barrier" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed(status, "rtems_task_wake_after");

  /* the end */
  TEST_END();
  rtems_test_exit(0);
}
