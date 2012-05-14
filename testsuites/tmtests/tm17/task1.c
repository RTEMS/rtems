/*
 *  COPYRIGHT (c) 1989-2009.
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

uint32_t   Task_count;
rtems_task_priority Task_priority;

rtems_task First_task(
  rtems_task_argument argument
);

rtems_task Middle_tasks(
  rtems_task_argument argument
);

rtems_task Last_task(
  rtems_task_argument argument
);

int operation_count = OPERATION_COUNT;

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_task_entry  task_entry;
  int               index;
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 17 ***" );

  Task_priority = RTEMS_MAXIMUM_PRIORITY - 1u;
  if ( OPERATION_COUNT > RTEMS_MAXIMUM_PRIORITY - 2u )
    operation_count =  (int) (RTEMS_MAXIMUM_PRIORITY - 2u);

  for( index = 0; index < operation_count ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'T', 'I', 'M', 'E' ),
      Task_priority,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[ index ]
    );
    directive_failed( status, "rtems_task_create loop" );

    if ( index == operation_count-1 ) task_entry = Last_task;
    else if ( index == 0 )            task_entry = First_task;
    else                              task_entry = Middle_tasks;

    status = rtems_task_start( Task_id[ index ], task_entry, 0 );
    directive_failed( status, "rtems_task_start loop" );
  }

  Task_count = 1;
  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task First_task(
  rtems_task_argument argument
)
{
  rtems_task_priority previous_priority;

  benchmark_timer_initialize();

  Task_priority--;
  Task_count++;

  (void) rtems_task_set_priority(
           Task_id[ Task_count ],
           Task_priority,
           &previous_priority
         );
}

rtems_task Middle_tasks(
  rtems_task_argument argument
)
{
  rtems_task_priority previous_priority;

  Task_priority--;
  Task_count++;

  (void) rtems_task_set_priority(
           Task_id[ Task_count ],
           Task_priority,
           &previous_priority
         );
}

rtems_task Last_task(
  rtems_task_argument argument
)
{
  int index;

  end_time = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index < operation_count ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  put_time(
    "rtems_task_set_priority: preempts caller",
    end_time,
    operation_count - 1u,
    overhead,
    CALLING_OVERHEAD_TASK_SET_PRIORITY
  );

  puts( "*** END OF TEST 17 ***" );
  rtems_test_exit( 0 );
}
