/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *
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

const char rtems_test_name[] = "TIME TEST 22";

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

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_id          id;
  rtems_status_code status;

  Print_Warning();

  TEST_BEGIN();

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

  rtems_task_exit();
}

rtems_task High_task(
  rtems_task_argument argument
)
{
  uint32_t    count;
  rtems_status_code status;

  benchmark_timer_initialize();
    (void) rtems_message_queue_broadcast(
             Queue_id,
             Buffer,
             MESSAGE_SIZE,
             &count
           );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_message_queue_broadcast: task readied returns to caller",
    end_time,
    1,
    0,
    0
  );

  status = rtems_task_suspend(RTEMS_SELF);
  directive_failed( status, "rtems_task_suspend" );
}

rtems_task Low_task(
  rtems_task_argument argument
)
{
  rtems_id          id;
  uint32_t          index;
  uint32_t          count;
  size_t            size;
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

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_message_queue_broadcast(
               Queue_id,
               Buffer,
               MESSAGE_SIZE,
               &count
             );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_message_queue_broadcast: no waiting tasks",
    end_time,
    OPERATION_COUNT,
    1,
    0
  );

  (void) rtems_message_queue_receive(
           Queue_id,
           (long (*)[4]) Buffer,
           &size,
           RTEMS_DEFAULT_OPTIONS,
           RTEMS_NO_TIMEOUT
         );

  /* should go to Preempt_task here */

  end_time = benchmark_timer_read();

  put_time(
    "rtems_message_queue_broadcast: task readied -- preempts caller",
    end_time,
    1,
    0,
    0
  );

  TEST_END();
  rtems_test_exit( 0 );
}

rtems_task Preempt_task(
  rtems_task_argument argument
)
{
  uint32_t    count;

  benchmark_timer_initialize();
    (void) rtems_message_queue_broadcast(
             Queue_id,
             Buffer,
             MESSAGE_SIZE,
             &count
           );

 /* should be preempted by low task */
}
