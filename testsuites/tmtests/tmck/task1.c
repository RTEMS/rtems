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

#if !defined(MAXIMUM_DISTRIBUTION)
#define MAXIMUM_DISTRIBUTION 1000
#endif

const char rtems_test_name[] = "TIME CHECKER";

uint32_t Distribution[ MAXIMUM_DISTRIBUTION + 1 ];

rtems_task Task_1(
  rtems_task_argument argument
);

void check_read_timer( void );

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_id          id;
  rtems_status_code status;

  /*
   *  Tell the Timer Driver what we are doing
   */

  benchmark_timer_disable_subtracting_average_overhead( 1 );

  Print_Warning();

  TEST_BEGIN();

  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' ),

  status = rtems_task_create(
    1,
    5,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_start( id, Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  rtems_task_exit();
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  (void) argument;

  uint32_t   index;

  check_read_timer();

  benchmark_timer_initialize();
  end_time = benchmark_timer_read();

  put_time(
    "Time Check: NULL timer stopped at",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
  for ( index = 1 ; index <= 1000 ; index++ )
    (void) benchmark_timer_empty_function();
  end_time = benchmark_timer_read();

  put_time(
    "Time Check: LOOP (1000) timer stopped at",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
  for ( index = 1 ; index <= 10000 ; index++ )
    (void) benchmark_timer_empty_function();
  end_time = benchmark_timer_read();

  put_time(
    "Time Check: LOOP (10000) timer stopped at",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
  for ( index = 1 ; index <= 50000 ; index++ )
    (void) benchmark_timer_empty_function();
  end_time = benchmark_timer_read();

  put_time(
    "Time Check: LOOP (50000) timer stopped at",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
  for ( index = 1 ; index <= 100000 ; index++ )
    (void) benchmark_timer_empty_function();
  end_time = benchmark_timer_read();

  put_time(
    "Time Check: LOOP (100000) timer stopped at",
    end_time,
    1,
    0,
    0
  );

  TEST_END();
  rtems_test_exit( 0 );
}

void check_read_timer()
{
  uint32_t   index;
  uint32_t   time;

  for ( index = 1 ; index <= MAXIMUM_DISTRIBUTION ; index++ )
    Distribution[ index ] = 0;

  for ( index = 1 ; index <= OPERATION_COUNT ; ) {
    benchmark_timer_initialize();
    end_time = benchmark_timer_read();
    if ( end_time > MAXIMUM_DISTRIBUTION ) {
      /*
       *  Under UNIX a simple process swap takes longer than we
       *  consider valid for our testing purposes.
       */
      printf( "TOO LONG (%" PRIu32 ") at index %" PRIu32 "!!!\n", end_time, index );
      continue;
    }
    Distribution[ end_time ]++;
    index++;
  }

  printf( "Units may not be in microseconds for this test!!!\n" );
  time = 0;
  for ( index = 0 ; index <= MAXIMUM_DISTRIBUTION ; index++ ) {
    time += (Distribution[ index ] * index);
    if ( Distribution[ index ] != 0 )
      printf( "%" PRId32 " %" PRId32 "\n", index, Distribution[ index ] );
  }
  printf( "Total time = %" PRId32 "\n", time );
  printf( "Average time = %" PRId32 "\n", time / OPERATION_COUNT );
}
