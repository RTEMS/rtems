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

const char rtems_test_name[] = "TIME TEST 6";

rtems_id Task_id[ OPERATION_COUNT + 1 ];

uint32_t   Task_restarted;

rtems_task null_task(
  rtems_task_argument argument
);

rtems_task Task_1(
  rtems_task_argument argument
);

void test_init( void );

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

void test_init( void )
{
  rtems_status_code status;
  rtems_id          id;

  Task_restarted = OPERATION_COUNT;

  status = rtems_task_create(
    rtems_build_name( 'T', 'I', 'M', 'E' ),
    (RTEMS_MAXIMUM_PRIORITY / 2u) + 1u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create" );

  status = rtems_task_start( id, Task_1, 0 );
  directive_failed( status, "rtems_task_start" );
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code status;
  uint32_t    index;

  if ( Task_restarted == OPERATION_COUNT )
     benchmark_timer_initialize();

  Task_restarted--;

  if ( Task_restarted != 0 )
    (void) rtems_task_restart( RTEMS_SELF, 0 );

  end_time = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  put_time(
    "rtems_task_restart: calling task",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  for ( index=1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'T', 'I', 'M', 'E' ),
      RTEMS_MAXIMUM_PRIORITY - 1u,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[ index ]
    );
    directive_failed( status, "rtems_task_create loop" );

    status = rtems_task_start( Task_id[ index ], null_task, 0 );
    directive_failed( status, "rtems_task_start loop" );
  }

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_suspend( Task_id[ index ] );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_suspend: returns to caller",
    end_time,
    OPERATION_COUNT,
    0,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_resume( Task_id[ index ] );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_resume: task readied returns to caller",
    end_time,
    OPERATION_COUNT,
    0,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_delete( Task_id[ index ] );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_delete: ready task",
    end_time,
    OPERATION_COUNT,
    0,
    0
  );

  TEST_END();
  rtems_test_exit( 0 );
}

rtems_task null_task(
  rtems_task_argument argument
)
{
  (void) argument;

  while ( FOREVER )
    ;
}
