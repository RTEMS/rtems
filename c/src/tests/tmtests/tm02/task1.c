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

rtems_id High_id;
rtems_id Low_id;
rtems_id Semaphore_id;

rtems_task High_task(
  rtems_task_argument argument
);

rtems_task Middle_tasks(
  rtems_task_argument argument
);

rtems_task Low_task(
  rtems_task_argument argument
);


void test_init();

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 2 ***" );

  test_init();

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete" );
}

void test_init()
{
  rtems_status_code   status;
  rtems_unsigned32    index;
  rtems_task_priority priority;

  priority = 5;

  status = rtems_task_create(
    rtems_build_name( 'H', 'I', 'G', 'H' ),
    priority,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &High_id
  );
  directive_failed( status, "rtems_task_create of high task" );

  priority++;

  status = rtems_task_start( High_id, High_task, 0 );
  directive_failed( status, "rtems_task_start of high task" );

  for ( index=2 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'M', 'I', 'D', ' ' ),
      priority,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Low_id
    );
    directive_failed( status, "rtems_task_create middle" );

    priority++;

    status = rtems_task_start( Low_id, Middle_tasks, 0 );
    directive_failed( status, "rtems_task_start middle" );
  }

  status = rtems_task_create(
    rtems_build_name( 'L', 'O', 'W', ' ' ),
    priority,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Low_id
  );
  directive_failed( status, "rtems_task_create low" );

  status = rtems_task_start( Low_id, Low_task, 0 );
  directive_failed( status, "rtems_task_start low" );

  status = rtems_semaphore_create(
    rtems_build_name( 'S', 'M', '1', ' '),
    0,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &Semaphore_id
  );
  directive_failed( status, "rtems_semaphore_create of SM1" );
}

rtems_task High_task(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Timer_initialize();           /* start blocking rtems_semaphore_obtain time */
  status = rtems_semaphore_obtain(
    Semaphore_id,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
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
}

rtems_task Low_task(
  rtems_task_argument argument
)
{
  end_time = Read_timer();

  put_time(
    "rtems_semaphore_obtain: not available -- caller blocks",
    end_time,
    OPERATION_COUNT,
    0,
    CALLING_OVERHEAD_SEMAPHORE_OBTAIN
  );

  puts( "*** END OF TEST 2 ***" );
  rtems_test_exit( 0 );
}
