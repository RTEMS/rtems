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

rtems_name Period_name;

rtems_task Tasks(
  rtems_task_argument argument
);

rtems_task Low_task(
  rtems_task_argument argument
);

rtems_unsigned32 Task_count;

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_id          id;
  rtems_unsigned32  index;
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 29 ***" );

  Period_name = rtems_build_name( 'P', 'R', 'D', ' ' );

  Timer_initialize();
    (void) rtems_rate_monotonic_create( Period_name, &id );
  end_time = Read_timer();

  put_time(
    "rtems_rate_monotonic_create",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_RATE_MONOTONIC_CREATE
  );

  Timer_initialize();
    (void) rtems_rate_monotonic_period( id, 10 );
  end_time = Read_timer();

  put_time(
    "rtems_rate_monotonic_period: initiate period -- returns to caller",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_RATE_MONOTONIC_PERIOD
  );

  Timer_initialize();
    (void) rtems_rate_monotonic_period( id, RTEMS_PERIOD_STATUS );
  end_time = Read_timer();

  put_time(
    "rtems_rate_monotonic_period: obtain status",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_RATE_MONOTONIC_PERIOD
  );

  Timer_initialize();
    (void) rtems_rate_monotonic_cancel( id );
  end_time = Read_timer();

  put_time(
    "rtems_rate_monotonic_cancel",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_RATE_MONOTONIC_CANCEL
  );

  Timer_initialize();
    (void) rtems_rate_monotonic_delete( id );
  end_time = Read_timer();

  put_time(
    "rtems_rate_monotonic_delete: inactive",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_RATE_MONOTONIC_DELETE
  );

  status = rtems_rate_monotonic_create( Period_name, &id );
  directive_failed( status, "rtems_rate_monotonic_create" );

  status = rtems_rate_monotonic_period( id, 10 );
  directive_failed( status, "rtems_rate_monotonic_period" );

  Timer_initialize();
    rtems_rate_monotonic_delete( id );
  end_time = Read_timer();

  put_time(
    "rtems_rate_monotonic_delete: active",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_RATE_MONOTONIC_DELETE
  );

  for ( index=1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'T', 'E', 'S', 'T' ),
      128,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "rtems_task_create LOOP" );

    status = rtems_task_start( id, Tasks, 0 );
    directive_failed( status, "rtems_task_start LOOP" );
  }

  status = rtems_task_create(
    rtems_build_name( 'L', 'O', 'W', ' ' ),
    200,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create LOW" );

  status = rtems_task_start( id, Low_task, 0 );
  directive_failed( status, "rtems_task_start LOW" );

  Task_count = 0;

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task Tasks(
  rtems_task_argument argument
)
{
  rtems_id          id;
  rtems_status_code status;

  status = rtems_rate_monotonic_create( 1, &id );
  directive_failed( status, "rtems_rate_monotonic_create" );

  status = rtems_rate_monotonic_period( id, 100 );
  directive_failed( status, "rtems_rate_monotonic_period" );

  /*
   *  Give up the processor to allow all tasks to actually
   *  create and start their period timer before the benchmark
   *  timer is initialized.
   */

  (void) rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );

  Task_count++;

  if ( Task_count == 1 )
    Timer_initialize();

  (void) rtems_rate_monotonic_period( id, 100 );
}

rtems_task Low_task(
  rtems_task_argument argument
)
{
  rtems_unsigned32 index;

  end_time = Read_timer();

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) Empty_function();
  overhead = Read_timer();

  put_time(
    "rtems_rate_monotonic_period: conclude periods -- caller blocks",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_RATE_MONOTONIC_PERIOD
  );

  puts( "*** END OF TEST 29 ***" );
  rtems_test_exit( 0 );
}
