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

const char rtems_test_name[] = "TIME TEST 16";

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

  TEST_BEGIN();

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

  rtems_task_exit();
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
    "rtems_event_send: task readied preempts caller",
    end_time,
    operation_count - 1u,
    0u,
    0
  );

  TEST_END();
  rtems_test_exit( 0 );
}
