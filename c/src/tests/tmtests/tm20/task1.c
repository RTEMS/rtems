/*
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define TEST_INIT
#include "system.h"
#include <assert.h>

rtems_device_major_number _STUB_major = 1;

rtems_id         Region_id;
rtems_name       Region_name;
rtems_unsigned8  Region_area[ 2048 ] CPU_STRUCTURE_ALIGNMENT;

#define PARTITION_SIZE         2048
#define PARTITION_ELEMENT_SIZE  128
#define PARTITION_BUFFER_POINTERS \
    ((PARTITION_SIZE / PARTITION_ELEMENT_SIZE) + 2)

rtems_id         Partition_id;
rtems_name       Partition_name;
rtems_unsigned8  Partition_area[ PARTITION_SIZE ] CPU_STRUCTURE_ALIGNMENT;

void  *Buffer_address_1;
void  *Buffer_address_2;
void  *Buffer_address_3;
void  *Buffer_address_4;

rtems_unsigned32 buffer_count;

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
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 20 ***" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'I', 'M', '1' ),
    128,
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
    129,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TASK2" );

  status = rtems_task_start( Task_id[ 2 ], Task_2, 0 );
  directive_failed( status, "rtems_task_start of TASK2" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_unsigned32    index;
  rtems_mode          previous_mode;
  rtems_task_priority previous_priority;
  rtems_status_code   status;

  Partition_name = rtems_build_name( 'P', 'A', 'R', 'T' );

  Timer_initialize();
    rtems_partition_create(
      Partition_name,
      Partition_area,
      PARTITION_SIZE,
      128,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Partition_id
    );
  end_time = Read_timer();

  put_time(
    "rtems_partition_create",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_PARTITION_CREATE
  );

  Region_name = rtems_build_name( 'R', 'E', 'G', 'N' );

  Timer_initialize();
    rtems_region_create(
      Region_name,
      Region_area,
      2048,
      16,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Region_id
    );
  end_time = Read_timer();

  put_time(
    "rtems_region_create",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_REGION_CREATE
  );

  Timer_initialize();
    (void) rtems_partition_get_buffer( Partition_id, &Buffer_address_1 );
  end_time = Read_timer();

  put_time(
    "rtems_partition_get_buffer: available",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_PARTITION_GET_BUFFER
  );

  buffer_count = 0;
  while ( FOREVER ) {

    status = rtems_partition_get_buffer(
               Partition_id,
               &Buffer_addresses[ buffer_count ]
            );

    if ( status == RTEMS_UNSATISFIED ) break;

    buffer_count++;

    assert( buffer_count < PARTITION_BUFFER_POINTERS );
  }

  Timer_initialize();
    (void) rtems_partition_get_buffer( Partition_id, &Buffer_address_2 );
  end_time = Read_timer();

  put_time(
    "rtems_partition_get_buffer: not available",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_PARTITION_GET_BUFFER
  );

  Timer_initialize();
    (void) rtems_partition_return_buffer( Partition_id, Buffer_address_1 );
  end_time = Read_timer();

  put_time(
    "rtems_partition_return_buffer",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_PARTITION_RETURN_BUFFER
  );

  for ( index = 0 ; index < buffer_count ; index++ ) {

    status = rtems_partition_return_buffer(
               Partition_id,
               Buffer_addresses[ index ]
             );
    directive_failed( status, "rtems_partition_return_buffer" );

  }

  Timer_initialize();
    (void) rtems_partition_delete( Partition_id );
  end_time = Read_timer();

  put_time(
    "rtems_partition_delete",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_PARTITION_DELETE
  );

  status = rtems_region_get_segment(
             Region_id,
             400,
             RTEMS_DEFAULT_OPTIONS,
             RTEMS_NO_TIMEOUT,
             &Buffer_address_2
           );
  directive_failed( status, "region_get_segment" );

  Timer_initialize();
    (void) rtems_region_get_segment(
      Region_id,
      400,
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT,
      &Buffer_address_3
    );
  end_time = Read_timer();

  put_time(
    "rtems_region_get_segment: available",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_REGION_GET_SEGMENT
  );

  Timer_initialize();
    (void) rtems_region_get_segment(
      Region_id,
      1998,
      RTEMS_NO_WAIT,
      RTEMS_NO_TIMEOUT,
      &Buffer_address_4
    );
  end_time = Read_timer();

  put_time(
    "rtems_region_get_segment: not available -- NO_WAIT",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_REGION_GET_SEGMENT
  );

  status = rtems_region_return_segment( Region_id, Buffer_address_3 );
  directive_failed( status, "rtems_region_return_segment" );

  Timer_initialize();
    (void) rtems_region_return_segment( Region_id, Buffer_address_2 );
  end_time = Read_timer();

  put_time(
    "rtems_region_return_segment: no waiting tasks",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_REGION_RETURN_SEGMENT
  );

  status = rtems_region_get_segment(
    Region_id,
    400,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &Buffer_address_1
  );
  directive_failed( status, "rtems_region_get_segment" );

  Timer_initialize();
    (void) rtems_region_get_segment(
      Region_id,
      1998,
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT,
      &Buffer_address_2
    );

  /* execute Task_2 */

  end_time = Read_timer();

  put_time(
    "rtems_region_return_segment: task readied -- preempts caller",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_REGION_RETURN_SEGMENT
  );

  status = rtems_region_return_segment( Region_id, Buffer_address_2 );
  directive_failed( status, "rtems_region_return_segment" );

  status = rtems_task_mode(
    RTEMS_NO_PREEMPT,
    RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );

  status = rtems_task_set_priority( RTEMS_SELF, 254, &previous_priority );
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
    1998,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &Buffer_address_2
  );
  directive_failed( status, "rtems_region_get_segment" );

  /* execute Task_2 */

  status = rtems_region_return_segment( Region_id, Buffer_address_2 );
  directive_failed( status, "rtems_region_return_segment" );

  Timer_initialize();
    (void) rtems_region_delete( Region_id );
  end_time = Read_timer();

  put_time(
    "rtems_region_delete",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_REGION_DELETE
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) Empty_function();
  overhead = Read_timer();

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_io_initialize( _STUB_major, 0, NULL );
  end_time = Read_timer();

  put_time(
    "rtems_io_initialize",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_IO_INITIALIZE
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_io_open( _STUB_major, 0, NULL );
  end_time = Read_timer();

  put_time(
    "rtems_io_open",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_IO_OPEN
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_io_close( _STUB_major, 0, NULL );
  end_time = Read_timer();

  put_time(
    "rtems_io_close",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_IO_CLOSE
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_io_read( _STUB_major, 0, NULL );
  end_time = Read_timer();

  put_time(
    "rtems_io_read",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_IO_READ
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_io_write( _STUB_major, 0, NULL );
  end_time = Read_timer();

  put_time(
    "rtems_io_write",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_IO_WRITE
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_io_control( _STUB_major, 0, NULL );
  end_time = Read_timer();

  put_time(
    "rtems_io_control",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_IO_CONTROL
  );

  puts( "*** END OF TEST 20 ***" );
  rtems_test_exit( 0 );
}

rtems_task Task_2(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  end_time = Read_timer();

  put_time(
    "rtems_region_get_segment: not available -- caller blocks",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_REGION_GET_SEGMENT
  );

  Timer_initialize();
    (void) rtems_region_return_segment( Region_id, Buffer_address_1 );

  /* preempt back to Task_1 */

  Timer_initialize();
    (void) rtems_region_return_segment( Region_id, Buffer_address_1 );
  end_time = Read_timer();

  put_time(
    "rtems_region_return_segment: task readied -- returns to caller",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_REGION_RETURN_SEGMENT
  );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
