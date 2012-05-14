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

rtems_id Task_id[ OPERATION_COUNT+1 ], task_index;

rtems_task High_task(
  rtems_task_argument argument
);

rtems_task Middle_tasks(
  rtems_task_argument argument
);

rtems_task Low_task(
  rtems_task_argument argument
);

void test_init(void);

int operation_count = OPERATION_COUNT;

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 7 ***" );

  test_init();

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

void test_init(void)
{
  rtems_status_code   status;
  rtems_task_priority priority;
  rtems_task_entry    task_entry;
  int                 index;

  priority = RTEMS_MAXIMUM_PRIORITY - 1u;

  if ( OPERATION_COUNT > RTEMS_MAXIMUM_PRIORITY - 2u )
    operation_count =  (int) (RTEMS_MAXIMUM_PRIORITY - 2u);

  for( index=0 ; index <= operation_count ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'T', 'I', 'M', 'E' ),
      priority,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[index]
    );
    directive_failed( status, "rtems_task_create" );
    priority--;

    if      ( index == 0 )               task_entry = Low_task;
    else if ( index == operation_count ) task_entry = High_task;
    else                                 task_entry = Middle_tasks;

    status = rtems_task_start( Task_id[index], task_entry, 0 );
    directive_failed( status, "rtems_task_start" );
  }
}

rtems_task High_task(
  rtems_task_argument argument
)
{
  if ( argument != 0 ) {
    end_time = benchmark_timer_read();

    put_time(
      "rtems_task_restart: suspended task -- preempts caller",
      end_time,
      operation_count,
      0,
      CALLING_OVERHEAD_TASK_RESTART
    );
  } else
    (void) rtems_task_suspend( RTEMS_SELF );

  puts( "*** END OF TEST 7 ***" );
  rtems_test_exit( 0 );
}

rtems_task Middle_tasks(
  rtems_task_argument argument
)
{
  task_index++;

  if ( argument != 0 )
    (void) rtems_task_restart( Task_id[ task_index ], 0xffffffff );
  else
    (void) rtems_task_suspend( RTEMS_SELF );
}

rtems_task Low_task(
  rtems_task_argument argument
)
{
  task_index = 1;

  benchmark_timer_initialize();
  (void) rtems_task_restart( Task_id[ task_index ], 0xffffffff );
}
