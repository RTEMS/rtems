/*
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define TEST_INIT
#include "system.h"

rtems_id Semaphore_id;
rtems_task test_init(
  rtems_task_argument argument
);

rtems_task Middle_tasks(
  rtems_task_argument argument
);

rtems_task High_task(
  rtems_task_argument argument
);


rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_id          task_id;

  Print_Warning();

  puts( "\n\n*** TIME TEST 3 ***" );
  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '1', ' ' ),
    252,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  directive_failed( status, "rtems_task_create of test_init" );

  status = rtems_task_start( task_id, test_init, 0 );
  directive_failed( status, "rtems_task_start of test_init" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task test_init(
  rtems_task_argument argument
)
{
  rtems_status_code   status;
  rtems_unsigned32    index;
  rtems_id            task_id;
  rtems_task_priority priority;

  priority = 250;

  status = rtems_semaphore_create(
    rtems_build_name( 'S', 'M', '1', '\0'),
    0,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &Semaphore_id
  );
  directive_failed( status, "rtems_semaphore_create of SM1" );

  for ( index = 2 ; index <= OPERATION_COUNT ; index ++ ) {
    rtems_task_create(
      rtems_build_name( 'M', 'I', 'D', ' ' ),
      priority,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &task_id
    );
    directive_failed( status, "rtems_task_create middle" );

    priority--;

    rtems_task_start( task_id, Middle_tasks, 0 );
    directive_failed( status, "rtems_task_start middle" );
  }

  status = rtems_task_create(
    rtems_build_name( 'H', 'I', 'G', 'H' ),
    priority,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  directive_failed( status, "rtems_task_create of high task" );

  status = rtems_task_start( task_id, High_task, 0 );
  directive_failed( status, "rtems_task_start of high task" );

  Timer_initialize();                          /* start the timer */
  status = rtems_semaphore_release( Semaphore_id );
}

rtems_task Middle_tasks(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  status = rtems_semaphore_obtain(
    Semaphore_id,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );

  status = rtems_semaphore_release( Semaphore_id );
}

rtems_task High_task(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  status = rtems_semaphore_obtain(
    Semaphore_id,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );

  end_time = Read_timer();

  put_time(
    "rtems_semaphore_release: task readied -- preempts caller",
    end_time,
    OPERATION_COUNT,
    0,
    CALLING_OVERHEAD_SEMAPHORE_RELEASE
  );

  puts( "*** END OF TEST 3 ***" );
  rtems_test_exit( 0 );
}
