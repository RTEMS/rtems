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

rtems_task test_init(
  rtems_task_argument argument
);

rtems_task Middle_tasks(
  rtems_task_argument argument
);

rtems_task High_task(
  rtems_task_argument argument
);

int operation_count = OPERATION_COUNT;

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_id          id;
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 16 ***" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'E', 'S', 'T' ),
    RTEMS_MAXIMUM_PRIORITY - 1u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create of test_init" );

  status = rtems_task_start( id, test_init, 0 );
  directive_failed( status, "rtems_task_start of test_init" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task test_init(
  rtems_task_argument argument
)
{
  rtems_task_priority priority;
  rtems_status_code   status;
  int                 index;
  rtems_task_entry    task_entry;

/*  As each task is started, it preempts this task and
 *  performs a blocking rtems_event_receive.  Upon completion of
 *  this loop all created tasks are blocked.
 */

  priority = RTEMS_MAXIMUM_PRIORITY - 2u;
  if ( OPERATION_COUNT > RTEMS_MAXIMUM_PRIORITY - 2u )
    operation_count =  (int) (RTEMS_MAXIMUM_PRIORITY - 2u);

  for( index = 0 ; index < operation_count ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'M', 'I', 'D', ' ' ),
      priority,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[ index ]
    );
    directive_failed( status, "rtems_task_create LOOP" );

    if (  index == operation_count-1 ) task_entry = High_task;
    else                               task_entry = Middle_tasks;

    status = rtems_task_start( Task_id[ index ], task_entry, 0 );
    directive_failed( status, "rtems_task_start LOOP" );

    priority--;
  }

  Task_count = 0;

  benchmark_timer_initialize();
    (void) rtems_event_send( Task_id[ Task_count ], RTEMS_EVENT_16 );
  /* preempts task */
}

rtems_task Middle_tasks(
  rtems_task_argument argument
)
{
  rtems_event_set event_out;

  (void) rtems_event_receive(              /* task blocks */
           RTEMS_EVENT_16,
           RTEMS_DEFAULT_OPTIONS,
           RTEMS_NO_TIMEOUT,
           &event_out
         );

  Task_count++;

  (void) rtems_event_send(               /* preempts task */
    Task_id[ Task_count ],
    RTEMS_EVENT_16
  );
}

rtems_task High_task(
  rtems_task_argument argument
)
{
  rtems_event_set event_out;

  (void) rtems_event_receive(                /* task blocks */
            RTEMS_EVENT_16,
            RTEMS_DEFAULT_OPTIONS,
            RTEMS_NO_TIMEOUT,
            &event_out
          );

  end_time = benchmark_timer_read();

  put_time(
    "rtems_event_send: task readied -- preempts caller",
    end_time,
    operation_count - 1u,
    0u,
    CALLING_OVERHEAD_EVENT_SEND
  );

  puts( "*** END OF TEST 16 ***" );
  rtems_test_exit( 0 );
}
