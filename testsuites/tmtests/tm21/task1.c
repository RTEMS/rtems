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

#include "system.h"
#undef EXTERN
#define EXTERN
#include "conftbl.h"
#include "gvar.h"

rtems_unsigned8 Region_area[ 2048 ] CPU_STRUCTURE_ALIGNMENT;
rtems_unsigned8 Partition_area[ 2048 ] CPU_STRUCTURE_ALIGNMENT;

rtems_task Task_1(
  rtems_task_argument argument
);

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_id          id;
  rtems_status_code status;

  puts( "\n\n*** TIME TEST 21 ***" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'I', 'M', 'E' ),
    250,
    1024,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create of TASK1" );

  status = rtems_task_start( id, Task_1, 0 );
  directive_failed( status, "rtems_task_start of TASK1" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_unsigned32  index;
  rtems_id          id;
  rtems_status_code status;

  for( index = 1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create (
      index,
      254,
      1024,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "rtems_task_create" );

    status = rtems_message_queue_create(
      index,
      OPERATION_COUNT,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "rtems_message_queue_create" );

    status = rtems_semaphore_create(
      index,
      OPERATION_COUNT,
      RTEMS_DEFAULT_ATTRIBUTES,
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

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) Empty_function();
  overhead = Read_timer();

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_ident( index, RTEMS_SEARCH_ALL_NODES, &id );
  end_time = Read_timer();

  put_time(
    "rtems_task_ident",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_IDENT
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_message_queue_ident( index, RTEMS_SEARCH_ALL_NODES, &id );
  end_time = Read_timer();

  put_time(
    "rtems_message_queue_ident",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_MESSAGE_QUEUE_IDENT
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_semaphore_ident( index, RTEMS_SEARCH_ALL_NODES, &id );
  end_time = Read_timer();

  put_time(
    "rtems_semaphore_ident",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_SEMAPHORE_IDENT
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_partition_ident( index, RTEMS_SEARCH_ALL_NODES, &id );
  end_time = Read_timer();

  put_time(
    "rtems_partition_ident",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_PARTITION_IDENT
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_region_ident( index, &id );
  end_time = Read_timer();

  put_time(
    "rtems_region_ident",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_REGION_IDENT
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_port_ident( index, &id );
  end_time = Read_timer();

  put_time(
    "rtems_port_ident",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_PORT_IDENT
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_ident( index, &id );
  end_time = Read_timer();

  put_time(
    "rtems_timer_ident",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TIMER_IDENT
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_rate_monotonic_ident( index, &id );
  end_time = Read_timer();

  put_time(
    "rtems_rate_monotonic_ident",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_RATE_MONOTONIC_IDENT
  );

  exit( 0 );
}
