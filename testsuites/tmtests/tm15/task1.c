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

const char rtems_test_name[] = "TIME TEST 15";

bool     time_set;
uint32_t eventout;

rtems_task High_tasks(
  rtems_task_argument argument
);

rtems_task Low_task(
  rtems_task_argument argument
);

void test_init(void);

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

void test_init(void)
{
  rtems_id          id;
  uint32_t    index;
  rtems_event_set   event_out;
  rtems_status_code status;

  time_set = false;

  status = rtems_task_create(
    rtems_build_name( 'L', 'O', 'W', ' ' ),
    10,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_NO_PREEMPT,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create LOW" );

  status = rtems_task_start( id, Low_task, 0 );
  directive_failed( status, "rtems_task_start LOW" );

  for ( index = 1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'H', 'I', 'G', 'H' ),
      5,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[ index ]
    );
    directive_failed( status, "rtems_task_create LOOP" );

    status = rtems_task_start( Task_id[ index ], High_tasks, 0 );
    directive_failed( status, "rtems_task_start LOOP" );
  }

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
    {
        (void) rtems_event_receive(
                 RTEMS_PENDING_EVENTS,
                 RTEMS_DEFAULT_OPTIONS,
                 RTEMS_NO_TIMEOUT,
                 &event_out
               );
    }

  end_time = benchmark_timer_read();

  put_time(
    "rtems_event_receive: obtain current events",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );


  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
    {
      (void) rtems_event_receive(
               RTEMS_ALL_EVENTS,
               RTEMS_NO_WAIT,
               RTEMS_NO_TIMEOUT,
               &event_out
             );
    }
  end_time = benchmark_timer_read();

  put_time(
    "rtems_event_receive: not available NO_WAIT",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );
}

rtems_task Low_task(
  rtems_task_argument argument
)
{
  (void) argument;

  uint32_t    index;
  rtems_event_set   event_out;

  end_time = benchmark_timer_read();

  put_time(
    "rtems_event_receive: not available caller blocks",
    end_time,
    OPERATION_COUNT,
    0,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_event_send( RTEMS_SELF, RTEMS_EVENT_16 );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_event_send: no task readied",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    (void) rtems_event_receive(
             RTEMS_EVENT_16,
             RTEMS_DEFAULT_OPTIONS,
             RTEMS_NO_TIMEOUT,
             &event_out
           );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_event_receive: available",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_event_send( Task_id[ index ], RTEMS_EVENT_16 );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_event_send: task readied returns to caller",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  TEST_END();
  rtems_test_exit( 0 );
}

rtems_task High_tasks(
  rtems_task_argument argument
)
{
  (void) argument;

  if ( time_set )
    (void) rtems_event_receive(
      RTEMS_EVENT_16,
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT,
      &eventout
    );
  else {
    time_set = true;
    /* start blocking rtems_event_receive time */
    benchmark_timer_initialize();
    (void) rtems_event_receive(
      RTEMS_EVENT_16,
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT,
      &eventout
    );
  }
}
