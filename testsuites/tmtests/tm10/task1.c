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

rtems_id Queue_id;
long Buffer[4];

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

  puts( "\n\n*** TIME TEST 10 ***" );

  test_init();

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

void test_init()
{
  rtems_unsigned32    index;
  rtems_unsigned32    size;
  rtems_task_entry    task_entry;
  rtems_status_code   status;
  rtems_task_priority priority;
  rtems_id            task_id;

  priority = 5;

  for( index = 0; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'T', 'I', 'M', 'E' ),
      priority,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &task_id
    );
    directive_failed( status, "rtems_task_create LOOP" );

    priority++;

    if ( index==0 )                    task_entry = High_task;
    else if ( index==OPERATION_COUNT ) task_entry = Low_task;
    else                               task_entry = Middle_tasks;

    status = rtems_task_start( task_id, task_entry, 0 );
    directive_failed( status, "rtems_task_start LOOP" );
  }

  status = rtems_message_queue_create(
    1,
    OPERATION_COUNT,
    16,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Queue_id
  );
  directive_failed( status, "rtems_message_queue_create" );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) Empty_function();
  overhead = Read_timer();

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_message_queue_receive(
               Queue_id,
               (long (*)[4]) Buffer,
               &size,
               RTEMS_NO_WAIT,
               RTEMS_NO_TIMEOUT
             );
  end_time = Read_timer();

  put_time(
    "rtems_message_queue_receive: not available -- NO_WAIT",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_MESSAGE_QUEUE_RECEIVE
  );

}

rtems_task High_task(
  rtems_task_argument argument
)
{
  rtems_unsigned32 size;
  
  Timer_initialize();
     (void) rtems_message_queue_receive(
              Queue_id,
              (long (*)[4]) Buffer,
              &size,
              RTEMS_DEFAULT_OPTIONS,
              RTEMS_NO_TIMEOUT
            );
}

rtems_task Middle_tasks(
  rtems_task_argument argument
)
{
  rtems_unsigned32 size;
  
  (void) rtems_message_queue_receive(
           Queue_id,
           (long (*)[4]) Buffer,
           &size,
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
    "rtems_message_queue_receive: not available -- caller blocks",
    end_time,
    OPERATION_COUNT,
    0,
    CALLING_OVERHEAD_MESSAGE_QUEUE_RECEIVE
  );

  puts( "*** END OF TEST 10 ***" );
  rtems_test_exit( 0 );
}
