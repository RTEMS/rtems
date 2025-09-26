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

const char rtems_test_name[] = "TIME TEST 21";

uint8_t   Region_area[ 2048 ] CPU_STRUCTURE_ALIGNMENT;
uint8_t   Partition_area[ 2048 ] CPU_STRUCTURE_ALIGNMENT;

rtems_task Task_1(
  rtems_task_argument argument
);

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_id          id;
  rtems_status_code status;

  Print_Warning();

  TEST_BEGIN();

  status = rtems_task_create(
    rtems_build_name( 'T', 'I', 'M', 'E' ),
    RTEMS_MAXIMUM_PRIORITY - 5u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create of TASK1" );

  status = rtems_task_start( id, Task_1, 0 );
  directive_failed( status, "rtems_task_start of TASK1" );

  rtems_task_exit();
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  (void) argument;

  uint32_t    index;
  rtems_id          id;
  rtems_status_code status;

  for( index = 1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create (
      index,
      RTEMS_MAXIMUM_PRIORITY - 1u,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "rtems_task_create" );

    status = rtems_message_queue_create(
      index,
      1,                       /* only going to ident this queue */
      MESSAGE_SIZE,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "rtems_message_queue_create" );

    status = rtems_semaphore_create(
      index,
      OPERATION_COUNT,
      RTEMS_DEFAULT_ATTRIBUTES,
      RTEMS_NO_PRIORITY,
      &id
    );
    directive_failed( status, "rtems_semaphore_create" );

    status = rtems_region_create(
      index,
      Region_area,
      2048,
      16,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "rtems_region_create" );

    status = rtems_partition_create(
      index,
      Partition_area,
      2048,
      128,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "rtems_partition_create" );

    status = rtems_port_create(
      index,
      Partition_area,
      Region_area,
      0xff,
      &id
    );
    directive_failed( status, "rtems_port_create" );

    status = rtems_timer_create( index, &id );
    directive_failed( status, "rtems_timer_create" );

    status = rtems_rate_monotonic_create( index, &id );
    directive_failed( status, "rtems_rate_monotonic_create" );
  }

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_ident( index, RTEMS_SEARCH_ALL_NODES, &id );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_ident: only case",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_message_queue_ident( index, RTEMS_SEARCH_ALL_NODES, &id );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_message_queue_ident: only case",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_semaphore_ident( index, RTEMS_SEARCH_ALL_NODES, &id );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_semaphore_ident: only case",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_partition_ident( index, RTEMS_SEARCH_ALL_NODES, &id );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_partition_ident: only case",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_region_ident( index, &id );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_region_ident: only case",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_port_ident( index, &id );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_port_ident: only case",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_ident( index, &id );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_timer_ident: only case",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_rate_monotonic_ident( index, &id );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_rate_monotonic_ident: only case",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  TEST_END();
  rtems_test_exit( 0 );
}
