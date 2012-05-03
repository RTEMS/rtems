/*
 *  COPYRIGHT (c) 1989-2008.
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

rtems_id   Task_id[OPERATION_COUNT+1];
uint32_t   Task_index;

rtems_task High_task(
  rtems_task_argument argument
);

rtems_task Middle_tasks(
  rtems_task_argument argument
);

rtems_task Low_task(
  rtems_task_argument argument
);

extern void test_init(void);

int operation_count = OPERATION_COUNT;

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 5 ***" );

  test_init();

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

void test_init(void)
{
  rtems_status_code   status;
  rtems_task_entry    task_entry;
  rtems_task_priority priority;
  uint32_t      index;

  priority = RTEMS_MAXIMUM_PRIORITY - 1;

  if ( OPERATION_COUNT > RTEMS_MAXIMUM_PRIORITY - 2 )
    operation_count =  RTEMS_MAXIMUM_PRIORITY - 2;

  for( index = 0; index <= operation_count ; index++ ) {

    status = rtems_task_create(
      rtems_build_name( 'T', 'I', 'M', 'E' ),
      priority,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[ index ]
    );
    directive_failed( status, "rtems_task_create loop" );

    priority--;

    if ( index==0 )                    task_entry = Low_task;
    else if ( index==operation_count ) task_entry = High_task;
    else                               task_entry = Middle_tasks;

    status = rtems_task_start( Task_id[ index ], task_entry, 0 );
    directive_failed( status, "rtems_task_start loop" );
  }
}

rtems_task High_task(
  rtems_task_argument argument
)
{
  benchmark_timer_initialize();

  (void) rtems_task_suspend( RTEMS_SELF );

  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_resume: task readied -- preempts caller",
    end_time,
    operation_count,
    0,
    CALLING_OVERHEAD_TASK_RESUME
  );

  puts( "*** END OF TEST 5 ***" );
  rtems_test_exit( 0 );
}

rtems_task Middle_tasks(
  rtems_task_argument argument
)
{
  (void) rtems_task_suspend( RTEMS_SELF );

  Task_index++;
  (void) rtems_task_resume( Task_id[ Task_index ] );
}

rtems_task Low_task(
  rtems_task_argument argument
)
{

  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_suspend: calling task",
    end_time,
    operation_count,
    0,
    CALLING_OVERHEAD_TASK_SUSPEND
  );

  Task_index = 1;
  benchmark_timer_initialize();
  (void) rtems_task_resume( Task_id[ Task_index ] );
}
