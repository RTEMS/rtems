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

rtems_id Queue_id;

long Buffer[4];

rtems_task test_init(
  rtems_task_argument argument
);

rtems_task High_task(
  rtems_task_argument argument
);

rtems_task Low_tasks(
  rtems_task_argument argument
);

#define MESSAGE_SIZE (sizeof(long) * 4)

int operation_count = OPERATION_COUNT;

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_id          task_id;
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 12 ***" );

  status = rtems_task_create(
    1,
    RTEMS_MAXIMUM_PRIORITY - 1u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  directive_failed( status, "rtems_task_create" );

  status = rtems_task_start( task_id, test_init, 0 );
  directive_failed( status, "rtems_task_start" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task test_init(
  rtems_task_argument argument
)
{
  int                  index;
  rtems_task_entry     task_entry;
  rtems_task_priority  priority;
  rtems_id             task_id;
  rtems_status_code    status;


  status = rtems_message_queue_create(
    rtems_build_name( 'M', 'Q', '1', ' ' ),
    (uint32_t) operation_count,
    MESSAGE_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Queue_id
  );
  directive_failed( status, "rtems_message_queue_create" );

  priority = RTEMS_MAXIMUM_PRIORITY - 1u;

  if ( OPERATION_COUNT > RTEMS_MAXIMUM_PRIORITY - 2 )
    operation_count =  RTEMS_MAXIMUM_PRIORITY - 2;

  for( index = 0; index < operation_count ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'T', 'I', 'M', 'E' ),
      priority,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &task_id
    );
    directive_failed( status, "rtems_task_create LOOP" );

    priority--;

    if ( index == operation_count-1 ) task_entry = High_task;
    else                              task_entry = Low_tasks;

    status = rtems_task_start( task_id, task_entry, 0 );
    directive_failed( status, "rtems_task_start LOOP" );
  }
}

rtems_task High_task(
  rtems_task_argument argument
)
{
  int  index;

  benchmark_timer_initialize();
    for ( index=1 ; index < operation_count ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index < operation_count ; index++ )
      (void) rtems_message_queue_send( Queue_id, Buffer, MESSAGE_SIZE );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_message_queue_send: task readied -- returns to caller",
    end_time,
    operation_count - 1,
    overhead,
    CALLING_OVERHEAD_MESSAGE_QUEUE_SEND
  );

  puts( "*** END OF TEST 12 ***" );
  rtems_test_exit( 0 );
}

rtems_task Low_tasks(
  rtems_task_argument argument
)
{
  size_t  size;

  (void) rtems_message_queue_receive(
           Queue_id,
           (long (*)[4]) Buffer,
           &size,
           RTEMS_DEFAULT_OPTIONS,
           RTEMS_NO_TIMEOUT
         );
}
