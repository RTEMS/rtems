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

const char rtems_test_name[] = "TIME TEST";

rtems_task Test_task(
  rtems_task_argument argument
);

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code status;

  Print_Warning();

  TEST_BEGIN();

  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ] = rtems_build_name( 'T', 'A', '3', ' ' );

  status = rtems_task_create(
    Task_name[ 1 ],
    (RTEMS_MAXIMUM_PRIORITY / 2u) + 1u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_start( Task_id[ 1 ], Test_task, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  rtems_task_exit();
}

rtems_task Test_task(
  rtems_task_argument argument
)
{
  (void) argument;

  uint32_t    semaphore_obtain_time;
  uint32_t    semaphore_release_time;
  uint32_t    semaphore_obtain_no_wait_time;
  uint32_t    semaphore_obtain_loop_time;
  uint32_t    semaphore_release_loop_time;
  uint32_t    index;
  uint32_t    iterations;
  rtems_name        name;
  rtems_id          smid;
  rtems_status_code status;

  name = rtems_build_name( 'S', 'M', '1', ' ' );

  semaphore_obtain_time          = 0;
  semaphore_release_time         = 0;
  semaphore_obtain_no_wait_time  = 0;
  semaphore_obtain_loop_time     = 0;
  semaphore_release_loop_time    = 0;


  /* Time one invocation of rtems_semaphore_create */

  benchmark_timer_initialize();
    (void) rtems_semaphore_create(
      name,
      OPERATION_COUNT,
      RTEMS_DEFAULT_MODES,
      RTEMS_NO_PRIORITY,
      &smid
    );
  end_time = benchmark_timer_read();
  put_time(
    "rtems_semaphore_create: only case",
    end_time,
    1,
    0,
    0
  );

  /* Time one invocation of rtems_semaphore_delete */

  benchmark_timer_initialize();
    (void) rtems_semaphore_delete( smid );
  end_time = benchmark_timer_read();
  put_time(
    "rtems_semaphore_delete: only case",
    end_time,
    1,
    0,
    0
  );

  status = rtems_semaphore_create(
    name,
    OPERATION_COUNT,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &smid
  );
  directive_failed( status, "rtems_task_create of TA1" );

  for ( iterations=OPERATION_COUNT ; iterations ; iterations-- ) {

    benchmark_timer_initialize();
      for ( index = 1 ; index<=OPERATION_COUNT ; index++ )
        (void) benchmark_timer_empty_function();
    end_time = benchmark_timer_read();

    semaphore_obtain_loop_time  += end_time;
    semaphore_release_loop_time += end_time;

    /* rtems_semaphore_obtain (available) */

    benchmark_timer_initialize();
      for ( index = 1 ; index<=OPERATION_COUNT ; index++ )
        (void) rtems_semaphore_obtain(
          smid,
          RTEMS_DEFAULT_OPTIONS,
          RTEMS_NO_TIMEOUT
        );
    end_time = benchmark_timer_read();

    semaphore_obtain_time += end_time;

    /* rtems_semaphore_release */

    benchmark_timer_initialize();
      for ( index = 1 ; index<=OPERATION_COUNT ; index++ )
        (void) rtems_semaphore_release( smid );
    end_time = benchmark_timer_read();

    semaphore_release_time += end_time;

    /* semaphore obtain (RTEMS_NO_WAIT) */
    benchmark_timer_initialize();
      for ( index = 1 ; index<=OPERATION_COUNT ; index++ )
        rtems_semaphore_obtain( smid, RTEMS_NO_WAIT, RTEMS_NO_TIMEOUT );
    semaphore_obtain_no_wait_time += benchmark_timer_read();

    benchmark_timer_initialize();
      for ( index = 1 ; index<=OPERATION_COUNT ; index++ )
        rtems_semaphore_release( smid );
    end_time = benchmark_timer_read();

    semaphore_release_time += end_time;
  }

  put_time(
    "rtems_semaphore_obtain: available",
    semaphore_obtain_time,
    OPERATION_COUNT * OPERATION_COUNT,
    semaphore_obtain_loop_time,
    0
  );

  put_time(
    "rtems_semaphore_obtain: not available NO_WAIT",
    semaphore_obtain_no_wait_time,
    OPERATION_COUNT * OPERATION_COUNT,
    semaphore_obtain_loop_time,
    0
  );

  put_time(
    "rtems_semaphore_release: no waiting tasks",
    semaphore_release_time,
    OPERATION_COUNT * OPERATION_COUNT * 2,
    semaphore_release_loop_time * 2,
    0
  );

  TEST_END();
  rtems_test_exit( 0 );
}
