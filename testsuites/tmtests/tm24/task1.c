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

rtems_unsigned32 Task_count;

rtems_task Tasks(
  rtems_task_argument argument
);

rtems_task High_task(
  rtems_task_argument argument
);

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_id          id;
  rtems_unsigned32  index;
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 24 ***" );

  status = rtems_task_create(
    rtems_build_name( 'H', 'I', 'G', 'H' ),
    10,
    1024,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create HIGH" );

  status = rtems_task_start( id, High_task, 0 );
  directive_failed( status, "rtems_task_create HIGH" );

  for ( index = 1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'R', 'E', 'S', 'T' ),
      128,
      1024,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "rtems_task_create LOOP" );

    status = rtems_task_start( id, Tasks, 0 );
    directive_failed( status, "rtems_task_start LOOP" );
  }
  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task High_task(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_unsigned32  index;

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) Empty_function();
  overhead = Read_timer();

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  end_time = Read_timer();

  put_time(
    "rtems_task_wake_after (no context switch)",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_WAKE_AFTER
  );

  Task_count = 0;

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task Tasks(
  rtems_task_argument argument
)
{
  Task_count++;

  if ( Task_count == 1 )
    Timer_initialize();
  else if ( Task_count == OPERATION_COUNT ) {
    end_time = Read_timer();

    put_time(
      "rtems_task_wake_after (context switch)",
      end_time,
      OPERATION_COUNT,
      overhead,
      CALLING_OVERHEAD_TASK_WAKE_AFTER
    );

  puts( "*** END OF TEST 24 ***" );
    exit( 0 );
  }
  (void) rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
}
