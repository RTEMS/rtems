/*
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#define TEST_INIT
#include "system.h"

rtems_device_major_number _STUB_major = 1;

rtems_id         Region_id;
rtems_name       Region_name;
rtems_unsigned8  Region_area[ 2048 ] CPU_STRUCTURE_ALIGNMENT;

rtems_id         Partition_id;
rtems_name       Partition_name;
rtems_unsigned8  Partition_area[ 2048 ] CPU_STRUCTURE_ALIGNMENT;

void            *Buffer_address_1;
void            *Buffer_address_2;
void            *Buffer_address_3;
void            *Buffer_address_4;

rtems_unsigned32 buffer_count;

void            *Buffer_addresses[ OPERATION_COUNT+1 ];

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
    2048,
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
    2048,
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
      2048,
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
    "rtems_partition_get_buffer (available)",
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
  }

  Timer_initialize();
    (void) rtems_partition_get_buffer( Partition_id, &Buffer_address_2 );
  end_time = Read_timer();

  put_time(
    "rtems_partition_get_buffer (not available)",
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
  directive_failed( status, "region_get_semgent" );

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
    "rtems_region_get_segment (available)",
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
    "rtems_region_get_segment (RTEMS_NO_WAIT)",
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
    "rtems_region_return_segment (no tasks waiting)",
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
    "rtems_region_return_segment (preempt)",
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
      (void) rtems_io_initialize( 0, 0, NULL );
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
      (void) rtems_io_open( 0, 0, NULL );
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
      (void) rtems_io_close( 0, 0, NULL );
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
      (void) rtems_io_read( 0, 0, NULL );
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
      (void) rtems_io_write( 0, 0, NULL );
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
      (void) rtems_io_control( 0, 0, NULL );
  end_time = Read_timer();

  put_time(
    "rtems_io_control",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_IO_CONTROL
  );

  puts( "*** END OF TEST 20 ***" );
  exit( 0 );
}

rtems_task Task_2(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  end_time = Read_timer();

  put_time(
    "rtems_region_get_segment (blocking)",
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
    "rtems_region_return_segment (ready -- return)",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_REGION_RETURN_SEGMENT
  );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
