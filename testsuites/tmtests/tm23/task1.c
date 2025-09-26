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

const char rtems_test_name[] = "TIME TEST 23";

rtems_id          Timer_id[ OPERATION_COUNT+1 ];

rtems_time_of_day time_of_day;

void null_delay(
  rtems_id  ignored_id,
  void     *ignored_address
);

rtems_task Low_task(
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
  (void) argument;

  rtems_task_priority priority;
  int                 index;
  rtems_id            id;
  rtems_task_entry    task_entry;
  rtems_status_code   status;

  Print_Warning();

  TEST_BEGIN();

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  priority = 2;
  if ( OPERATION_COUNT > RTEMS_MAXIMUM_PRIORITY - 2 )
    operation_count =  RTEMS_MAXIMUM_PRIORITY - 2;

  for( index=1 ; index <= operation_count ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'T', 'I', 'M', 'E' ),
      priority,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "rtems_task_create LOOP" );

    if ( index == 1 )                    task_entry = High_task;
    else if ( index == operation_count ) task_entry = Low_task;
    else                                 task_entry = Middle_tasks;

    status = rtems_task_start( id, task_entry, 0 );
    directive_failed( status, "rtems_task_start LOOP" );

    priority++;
  }

  rtems_task_exit();
}

void null_delay(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  (void) ignored_id;
  (void) ignored_address;
}

rtems_task High_task(
  rtems_task_argument argument
)
{
  (void) argument;

  uint32_t    index;
  rtems_status_code status;
  int i;

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_create( index, &Timer_id[ index ] );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_timer_create: only case",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_fire_after( Timer_id[ index ], 500, null_delay, NULL );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_timer_fire_after: inactive",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_fire_after( Timer_id[ index ], 500, null_delay, NULL );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_timer_fire_after: active",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_cancel( Timer_id[ index ] );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_timer_cancel: active",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  for ( benchmark_timer_initialize(), i=0 ; i<OPERATION_COUNT ; i++ )
  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_cancel( Timer_id[ index ] );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_timer_cancel: inactive",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  for ( benchmark_timer_initialize(), i=0 ; i<OPERATION_COUNT ; i++ )
  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_reset( Timer_id[ index ] );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_timer_reset: inactive",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_reset( Timer_id[ index ] );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_timer_reset: active",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  for ( index=1 ; index <= OPERATION_COUNT ; index++ )
    (void) rtems_timer_reset( Timer_id[ index ] );

  build_time( &time_of_day, 12, 31, 1988, 9, 0, 0, 0 );

  status = rtems_clock_set( &time_of_day );
  directive_failed( status, "rtems_clock_set" );

  time_of_day.year = 1989;

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_fire_when(
         Timer_id[ index ], &time_of_day, null_delay, NULL );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_timer_fire_when: inactive",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_fire_when(
         Timer_id[ index ], &time_of_day, null_delay, NULL );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_timer_fire_when: active",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_delete( Timer_id[ index ] );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_timer_delete: active",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
  for ( index=1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_timer_create( index, &Timer_id[ index ] );
    directive_failed( status, "rtems_timer_create" );

    status = rtems_timer_fire_after( Timer_id[ index ], 500, null_delay, NULL );
    directive_failed( status, "rtems_timer_fire_after" );

    status = rtems_timer_cancel( Timer_id[ index ] );
    directive_failed( status, "rtems_timer_cancel" );
  }

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_delete( Timer_id[ index ] );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_timer_delete: inactive",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    (void) rtems_task_wake_when( &time_of_day );
}

rtems_task Middle_tasks(
  rtems_task_argument argument
)
{
  (void) argument;

  (void) rtems_task_wake_when( &time_of_day );
}

rtems_task Low_task(
  rtems_task_argument argument
)
{
  (void) argument;

  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_wake_when: only case",
    end_time,
    operation_count,
    0,
    0
  );

  TEST_END();
  rtems_test_exit( 0 );
}
