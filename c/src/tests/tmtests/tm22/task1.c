/*
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#define TEST_INIT
#include "system.h"

rtems_id Queue_id;

long Buffer[4];

rtems_task Low_task(
  rtems_task_argument argument
);

rtems_task High_task(
  rtems_task_argument argument
);

rtems_task Preempt_task(
  rtems_task_argument argument
);

#define MESSAGE_SIZE (sizeof(long) * 4)

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_id          id;
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 22 ***" );

  status = rtems_message_queue_create(
    rtems_build_name( 'M', 'Q', '1', ' '),
    100,
    MESSAGE_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Queue_id
  );
  directive_failed( status, "rtems_message_queue_create" );

  status = rtems_task_create(
    rtems_build_name( 'L', 'O', 'W', ' ' ),
    10,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_NO_PREEMPT,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create" );

  status = rtems_task_start( id, Low_task, 0 );
  directive_failed( status, "rtems_task_start LOW" );

  status = rtems_task_create(
    1,
    11,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create RTEMS_PREEMPT" );

  status = rtems_task_start( id, Preempt_task, 0 );
  directive_failed( status, "rtems_task_start RTEMS_PREEMPT" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task High_task(
  rtems_task_argument argument
)
{
  rtems_unsigned32  count;
  rtems_status_code status;

  Timer_initialize();
    (void) rtems_message_queue_broadcast(
             Queue_id,
             Buffer,
             MESSAGE_SIZE,
             &count
           );
  end_time = Read_timer();

  put_time(
    "rtems_message_queue_broadcast: task readied -- returns to caller",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_MESSAGE_QUEUE_BROADCAST
  );

  status = rtems_task_suspend(RTEMS_SELF);
  directive_failed( status, "rtems_task_suspend" );
}

rtems_task Low_task(
  rtems_task_argument argument
)
{
  rtems_id          id;
  rtems_unsigned32  index;
  rtems_unsigned32  count;
  rtems_unsigned32  size;
  rtems_status_code status;

  status = rtems_task_create(
    rtems_build_name( 'H', 'I', 'G', 'H' ),
    5,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_NO_PREEMPT,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create" );

  status = rtems_task_start( id, High_task, 0 );
  directive_failed( status, "rtems_task_start HIGH" );

  status = rtems_message_queue_receive(
    Queue_id,
    (long (*)[4]) Buffer,
    &size,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "message_queu_receive" );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_message_queue_broadcast(
               Queue_id,
               Buffer,
               MESSAGE_SIZE,
               &count
             );
  end_time = Read_timer();

  put_time(
    "rtems_message_queue_broadcast: no waiting tasks",
    end_time,
    OPERATION_COUNT,
    1,
    CALLING_OVERHEAD_MESSAGE_QUEUE_BROADCAST
  );

  (void) rtems_message_queue_receive(
           Queue_id,
           (long (*)[4]) Buffer,
           &size,
           RTEMS_DEFAULT_OPTIONS,
           RTEMS_NO_TIMEOUT
         );

  /* should go to Preempt_task here */

  end_time = Read_timer();

  put_time(
    "rtems_message_queue_broadcast: task readied -- preempts caller",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_MESSAGE_QUEUE_BROADCAST
  );

  puts( "*** END OF TEST 22 ***" );
  rtems_test_exit( 0 );
}

rtems_task Preempt_task(
  rtems_task_argument argument
)
{
  rtems_unsigned32  count;

  Timer_initialize();
    (void) rtems_message_queue_broadcast(
             Queue_id,
             Buffer,
             MESSAGE_SIZE,
             &count
           );

 /* should be preempted by low task */
}
