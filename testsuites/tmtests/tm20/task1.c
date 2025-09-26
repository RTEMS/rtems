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

const char rtems_test_name[] = "TIME TEST 20";

rtems_device_major_number _STUB_major = 1;

rtems_id         Region_id;
rtems_name       Region_name;
uint8_t    Region_area[ 2048 ] CPU_STRUCTURE_ALIGNMENT;

#define PARTITION_SIZE         2048
#define PARTITION_ELEMENT_SIZE  128
#define PARTITION_BUFFER_POINTERS \
    ((PARTITION_SIZE / PARTITION_ELEMENT_SIZE) + 2)

rtems_id         Partition_id;
rtems_name       Partition_name;
uint8_t    Partition_area[ PARTITION_SIZE ] CPU_STRUCTURE_ALIGNMENT;

void  *Buffer_address_1;
void  *Buffer_address_2;
void  *Buffer_address_3;
void  *Buffer_address_4;

uint32_t   buffer_count;

void  *Buffer_addresses[ PARTITION_BUFFER_POINTERS ];

rtems_task Task_1(
  rtems_task_argument argument
);

rtems_task Task_2(
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

  status = rtems_task_create(
    rtems_build_name( 'T', 'I', 'M', '1' ),
    (RTEMS_MAXIMUM_PRIORITY / 2u) + 1u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TASK1" );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TASK1" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'I', 'M', '2' ),
    (RTEMS_MAXIMUM_PRIORITY / 2u) + 2u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TASK2" );

  status = rtems_task_start( Task_id[ 2 ], Task_2, 0 );
  directive_failed( status, "rtems_task_start of TASK2" );

  rtems_task_exit();
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  (void) argument;

  uint32_t      index;
  rtems_mode          previous_mode;
  rtems_task_priority previous_priority;
  rtems_status_code   status;

  Partition_name = rtems_build_name( 'P', 'A', 'R', 'T' );

  benchmark_timer_initialize();
    rtems_partition_create(
      Partition_name,
      Partition_area,
      PARTITION_SIZE,
      128,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Partition_id
    );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_partition_create: only case",
    end_time,
    1,
    0,
    0
  );

  Region_name = rtems_build_name( 'R', 'E', 'G', 'N' );

  benchmark_timer_initialize();
    rtems_region_create(
      Region_name,
      Region_area,
      2048,
      16,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Region_id
    );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_region_create: only case",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
    (void) rtems_partition_get_buffer( Partition_id, &Buffer_address_1 );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_partition_get_buffer: available",
    end_time,
    1,
    0,
    0
  );

  buffer_count = 0;
  while ( FOREVER ) {

    status = rtems_partition_get_buffer(
               Partition_id,
               &Buffer_addresses[ buffer_count ]
            );

    if ( status == RTEMS_UNSATISFIED ) break;

    buffer_count++;

    rtems_test_assert( buffer_count < PARTITION_BUFFER_POINTERS );
  }

  benchmark_timer_initialize();
    (void) rtems_partition_get_buffer( Partition_id, &Buffer_address_2 );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_partition_get_buffer: not available",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
    (void) rtems_partition_return_buffer( Partition_id, Buffer_address_1 );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_partition_return_buffer: only case",
    end_time,
    1,
    0,
    0
  );

  for ( index = 0 ; index < buffer_count ; index++ ) {

    status = rtems_partition_return_buffer(
               Partition_id,
               Buffer_addresses[ index ]
             );
    directive_failed( status, "rtems_partition_return_buffer" );

  }

  benchmark_timer_initialize();
    (void) rtems_partition_delete( Partition_id );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_partition_delete: only case",
    end_time,
    1,
    0,
    0
  );

  status = rtems_region_get_segment(
             Region_id,
             400,
             RTEMS_DEFAULT_OPTIONS,
             RTEMS_NO_TIMEOUT,
             &Buffer_address_2
           );
  directive_failed( status, "region_get_segment" );

  benchmark_timer_initialize();
    (void) rtems_region_get_segment(
      Region_id,
      400,
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT,
      &Buffer_address_3
    );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_region_get_segment: available",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
    (void) rtems_region_get_segment(
      Region_id,
      1700,
      RTEMS_NO_WAIT,
      RTEMS_NO_TIMEOUT,
      &Buffer_address_4
    );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_region_get_segment: not available NO_WAIT",
    end_time,
    1,
    0,
    0
  );

  status = rtems_region_return_segment( Region_id, Buffer_address_3 );
  directive_failed( status, "rtems_region_return_segment" );

  benchmark_timer_initialize();
    (void) rtems_region_return_segment( Region_id, Buffer_address_2 );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_region_return_segment: no waiting tasks",
    end_time,
    1,
    0,
    0
  );

  status = rtems_region_get_segment(
    Region_id,
    400,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &Buffer_address_1
  );
  directive_failed( status, "rtems_region_get_segment" );

  benchmark_timer_initialize();
    (void) rtems_region_get_segment(
      Region_id,
      1700,
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT,
      &Buffer_address_2
    );

  /* execute Task_2 */

  end_time = benchmark_timer_read();

  put_time(
    "rtems_region_return_segment: task readied preempts caller",
    end_time,
    1,
    0,
    0
  );

  status = rtems_region_return_segment( Region_id, Buffer_address_2 );
  directive_failed( status, "rtems_region_return_segment" );

  status = rtems_task_mode(
    RTEMS_NO_PREEMPT,
    RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );

  status = rtems_task_set_priority(
    RTEMS_SELF, RTEMS_MAXIMUM_PRIORITY - 1u, &previous_priority );
  directive_failed( status, "rtems_task_set_priority" );

  status = rtems_region_get_segment(
    Region_id,
    400,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &Buffer_address_1
  );
  directive_failed( status, "rtems_region_return_segment" );

  status = rtems_region_get_segment(
    Region_id,
    1700,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &Buffer_address_2
  );
  directive_failed( status, "rtems_region_get_segment" );

  /* execute Task_2 */

  status = rtems_region_return_segment( Region_id, Buffer_address_2 );
  directive_failed( status, "rtems_region_return_segment" );

  benchmark_timer_initialize();
    (void) rtems_region_delete( Region_id );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_region_delete: only case",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_io_initialize( _STUB_major, 0, NULL );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_io_initialize: only case",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_io_open( _STUB_major, 0, NULL );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_io_open: only case",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_io_close( _STUB_major, 0, NULL );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_io_close: only case",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_io_read( _STUB_major, 0, NULL );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_io_read: only case",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_io_write( _STUB_major, 0, NULL );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_io_write: only case",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_io_control( _STUB_major, 0, NULL );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_io_control: only case",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  TEST_END();
  rtems_test_exit( 0 );
}

rtems_task Task_2(
  rtems_task_argument argument
)
{
  (void) argument;

  end_time = benchmark_timer_read();

  put_time(
    "rtems_region_get_segment: not available caller blocks",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
    (void) rtems_region_return_segment( Region_id, Buffer_address_1 );

  /* preempt back to Task_1 */

  benchmark_timer_initialize();
    (void) rtems_region_return_segment( Region_id, Buffer_address_1 );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_region_return_segment: task readied returns to caller",
    end_time,
    1,
    0,
    0
  );

  rtems_task_exit();
}
