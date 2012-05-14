/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
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

int operation_count = OPERATION_COUNT;

void test_init(void);

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

void test_init(void)
{
  rtems_status_code   status;
  int                 index;
  rtems_task_priority priority;

  priority = 2;

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

  if ( OPERATION_COUNT > RTEMS_MAXIMUM_PRIORITY - 2u )
    operation_count = (int) (RTEMS_MAXIMUM_PRIORITY - 2u);
  for ( index=2 ; index < operation_count ; index++ ) {
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
  /* start blocking rtems_semaphore_obtain time */
  benchmark_timer_initialize();

  (void) rtems_semaphore_obtain(
    Semaphore_id,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
}

rtems_task Middle_tasks(
  rtems_task_argument argument
)
{
  (void) rtems_semaphore_obtain(
    Semaphore_id,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
}

rtems_task Low_task(
  rtems_task_argument argument
)
{
  end_time = benchmark_timer_read();

  put_time(
    "rtems_semaphore_obtain: not available -- caller blocks",
    end_time,
    operation_count - 1,
    0,
    CALLING_OVERHEAD_SEMAPHORE_OBTAIN
  );

  puts( "*** END OF TEST 2 ***" );
  rtems_test_exit( 0 );
}
