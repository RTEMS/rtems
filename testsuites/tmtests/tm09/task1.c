/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#if !defined(OPERATION_COUNT)
#define OPERATION_COUNT 100
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/btimer.h>

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "TIME TEST 9";

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

  TEST_BEGIN();

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

  rtems_task_exit();
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
    "rtems_message_queue_create: only case",
    end_time,
    1,
    0,
    0
  );

  queue_test();

  benchmark_timer_initialize();
    rtems_message_queue_delete( Queue_id );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_message_queue_delete: only case",
    end_time,
    1,
    0,
    0
  );

  TEST_END();
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
    0
  );

  put_time(
    "rtems_message_queue_urgent: no waiting tasks",
    urgent_time,
    OPERATION_COUNT * OPERATION_COUNT,
    urgent_loop_time,
    0
  );

  put_time(
    "rtems_message_queue_receive: available",
    receive_time,
    OPERATION_COUNT * OPERATION_COUNT * 2,
    receive_loop_time * 2,
    0
  );

  put_time(
    "rtems_message_queue_flush: no messages flushed",
    empty_flush_time,
    OPERATION_COUNT,
    0,
    0
  );

  put_time(
    "rtems_message_queue_flush: messages flushed",
    flush_time,
    OPERATION_COUNT,
    0,
    0
  );

}
