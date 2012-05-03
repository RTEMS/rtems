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

rtems_task Test_task(
  rtems_task_argument argument
);
void queue_test(void);

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 9 ***" );

  status = rtems_task_create(
    1,
    (RTEMS_MAXIMUM_PRIORITY / 2u) + 1u,
    RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create" );

  status = rtems_task_start( Task_id[ 1 ], Test_task, 0 );
  directive_failed( status, "rtems_task_start" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task Test_task (
  rtems_task_argument argument
)
{
  benchmark_timer_initialize();
    rtems_message_queue_create(
      1,
      OPERATION_COUNT,
      MESSAGE_SIZE,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Queue_id
    );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_message_queue_create",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_MESSAGE_QUEUE_CREATE
  );

  queue_test();

  benchmark_timer_initialize();
    rtems_message_queue_delete( Queue_id );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_message_queue_delete",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_MESSAGE_QUEUE_DELETE
  );

  puts( "*** END OF TEST 9 ***" );
  rtems_test_exit( 0 );
}

void queue_test(void)
{
  uint32_t    send_loop_time;
  uint32_t    urgent_loop_time;
  uint32_t    receive_loop_time;
  uint32_t    send_time;
  uint32_t    urgent_time;
  uint32_t    receive_time;
  uint32_t    empty_flush_time;
  uint32_t    flush_time;
  uint32_t    empty_flush_count;
  uint32_t    flush_count;
  uint32_t    index;
  uint32_t    iterations;
  long        buffer[4];
  rtems_status_code status;
  size_t      size;

  send_loop_time    = 0;
  urgent_loop_time  = 0;
  receive_loop_time = 0;
  send_time         = 0;
  urgent_time       = 0;
  receive_time      = 0;
  empty_flush_time  = 0;
  flush_time        = 0;
  flush_count       = 0;
  empty_flush_count = 0;

  for ( iterations = 1 ; iterations <= OPERATION_COUNT ; iterations++ ) {

    benchmark_timer_initialize();
      for ( index=1 ; index <= OPERATION_COUNT ; index++ )
        (void) benchmark_timer_empty_function();
    send_loop_time += benchmark_timer_read();

    benchmark_timer_initialize();
      for ( index=1 ; index <= OPERATION_COUNT ; index++ )
        (void) benchmark_timer_empty_function();
    urgent_loop_time += benchmark_timer_read();

    benchmark_timer_initialize();
      for ( index=1 ; index <= OPERATION_COUNT ; index++ )
        (void) benchmark_timer_empty_function();
    receive_loop_time += benchmark_timer_read();

    benchmark_timer_initialize();
      for ( index=1 ; index <= OPERATION_COUNT ; index++ )
        (void) rtems_message_queue_send( Queue_id, buffer, MESSAGE_SIZE );
    send_time += benchmark_timer_read();

    benchmark_timer_initialize();
      for ( index=1 ; index <= OPERATION_COUNT ; index++ )
        (void) rtems_message_queue_receive(
                 Queue_id,
                 (long (*)[4])buffer,
                 &size,
                 RTEMS_DEFAULT_OPTIONS,
                 RTEMS_NO_TIMEOUT
               );
    receive_time += benchmark_timer_read();

    benchmark_timer_initialize();
      for ( index=1 ; index <= OPERATION_COUNT ; index++ )
        (void) rtems_message_queue_urgent( Queue_id, buffer, MESSAGE_SIZE );
    urgent_time += benchmark_timer_read();

    benchmark_timer_initialize();
      for ( index=1 ; index <= OPERATION_COUNT ; index++ )
        (void) rtems_message_queue_receive(
                 Queue_id,
                 (long (*)[4])buffer,
                 &size,
                 RTEMS_DEFAULT_OPTIONS,
                 RTEMS_NO_TIMEOUT
               );
    receive_time += benchmark_timer_read();

    benchmark_timer_initialize();
      rtems_message_queue_flush( Queue_id, &empty_flush_count );
    empty_flush_time += benchmark_timer_read();

    /* send one message to flush */
    status = rtems_message_queue_send(
       Queue_id,
       (long (*)[4])buffer,
       MESSAGE_SIZE
    );
    directive_failed( status, "rtems_message_queue_send" );

    benchmark_timer_initialize();
      rtems_message_queue_flush( Queue_id, &flush_count );
    flush_time += benchmark_timer_read();
  }

  put_time(
    "rtems_message_queue_send: no waiting tasks",
    send_time,
    OPERATION_COUNT * OPERATION_COUNT,
    send_loop_time,
    CALLING_OVERHEAD_MESSAGE_QUEUE_SEND
  );

  put_time(
    "rtems_message_queue_urgent: no waiting tasks",
    urgent_time,
    OPERATION_COUNT * OPERATION_COUNT,
    urgent_loop_time,
    CALLING_OVERHEAD_MESSAGE_QUEUE_URGENT
  );

  put_time(
    "rtems_message_queue_receive: available",
    receive_time,
    OPERATION_COUNT * OPERATION_COUNT * 2,
    receive_loop_time * 2,
    CALLING_OVERHEAD_MESSAGE_QUEUE_RECEIVE
  );

  put_time(
    "rtems_message_queue_flush: no messages flushed",
    empty_flush_time,
    OPERATION_COUNT,
    0,
    CALLING_OVERHEAD_MESSAGE_QUEUE_FLUSH
  );

  put_time(
    "rtems_message_queue_flush: messages flushed",
    flush_time,
    OPERATION_COUNT,
    0,
    CALLING_OVERHEAD_MESSAGE_QUEUE_FLUSH
  );

}
