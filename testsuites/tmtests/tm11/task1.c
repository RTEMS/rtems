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

rtems_task Middle_tasks(
  rtems_task_argument argument
);

rtems_task High_task(
  rtems_task_argument argument
);

int operation_count = OPERATION_COUNT;

void Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_id id;

  Print_Warning();

  puts( "\n\n*** TIME TEST 11 ***" );

  status = rtems_task_create(
    1,
    RTEMS_MAXIMUM_PRIORITY - 1u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create" );

  status = rtems_task_start( id, test_init, 0 );
  directive_failed( status, "rtems_task_start" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

#define MESSAGE_SIZE (sizeof(long) * 4)

rtems_task test_init(
  rtems_task_argument argument
)
{
  int                 index;
  rtems_task_entry    task_entry;
  rtems_task_priority priority;
  rtems_id            task_id;
  rtems_status_code   status;

/*  As each task is started, it preempts this task and
 *  performs a blocking rtems_message_queue_receive.  Upon completion of
 *  this loop all created tasks are blocked.
 */

  status = rtems_message_queue_create(
    rtems_build_name( 'M', 'Q', '1', ' '  ),
    OPERATION_COUNT,
    MESSAGE_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Queue_id
  );
  directive_failed( status, "rtems_message_queue_create" );

  priority = RTEMS_MAXIMUM_PRIORITY - 2u;

  if ( OPERATION_COUNT > RTEMS_MAXIMUM_PRIORITY - 2u )
    operation_count =  RTEMS_MAXIMUM_PRIORITY - 2u;
  for( index = 0; index < operation_count ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'T', 'I', 'M', 'E'  ),
      priority,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &task_id
    );
    directive_failed( status, "rtems_task_create LOOP" );

    priority--;

    if ( index==operation_count-1 ) task_entry = High_task;
    else                            task_entry = Middle_tasks;

    status = rtems_task_start( task_id, task_entry, 0 );
    directive_failed( status, "rtems_task_start LOOP" );
  }

  benchmark_timer_initialize();
    (void) rtems_message_queue_send( Queue_id, Buffer, MESSAGE_SIZE );
}

rtems_task Middle_tasks(
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

  (void) rtems_message_queue_send( Queue_id, (long (*)[4]) Buffer, size );
}

rtems_task High_task(
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

  end_time = benchmark_timer_read();

  put_time(
    "rtems_message_queue_send: task readied -- preempts caller",
    end_time,
    operation_count,
    0,
    CALLING_OVERHEAD_MESSAGE_QUEUE_SEND
  );

  puts( "*** END OF TEST 11 ***" );
  rtems_test_exit( 0 );
}
