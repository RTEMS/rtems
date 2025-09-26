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

const char rtems_test_name[] = "TIME TEST 10";

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

void test_init(void);

int operation_count = OPERATION_COUNT;

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  Print_Warning();

  TEST_BEGIN();

  test_init();

  rtems_task_exit();
}

void test_init()
{
  int                 index;
  size_t              size;
  rtems_task_entry    task_entry;
  rtems_status_code   status;
  rtems_task_priority priority;
  rtems_id            task_id;

  priority = 2;

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

    priority++;

    if ( index==0 )                    task_entry = High_task;
    else if ( index==operation_count-1 ) task_entry = Low_task;
    else                               task_entry = Middle_tasks;

    status = rtems_task_start( task_id, task_entry, 0 );
    directive_failed( status, "rtems_task_start LOOP" );
  }

  status = rtems_message_queue_create(
    1,
    (uint32_t)operation_count,
    16,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Queue_id
  );
  directive_failed( status, "rtems_message_queue_create" );

  benchmark_timer_initialize();
    for ( index=1 ; index < operation_count ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index < operation_count ; index++ )
      (void) rtems_message_queue_receive(
               Queue_id,
               (long (*)[4]) Buffer,
               &size,
               RTEMS_NO_WAIT,
               RTEMS_NO_TIMEOUT
             );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_message_queue_receive: not available NO_WAIT",
    end_time,
    operation_count,
    overhead,
    0
  );

}

rtems_task High_task(
  rtems_task_argument argument
)
{
  (void) argument;

  size_t  size;

  benchmark_timer_initialize();
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
  (void) argument;

  size_t  size;

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
  (void) argument;

  end_time = benchmark_timer_read();

  put_time(
    "rtems_message_queue_receive: not available caller blocks",
    end_time,
    operation_count - 1,
    0,
    0
  );

  TEST_END();
  rtems_test_exit( 0 );
}
