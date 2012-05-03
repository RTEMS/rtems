/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

uint32_t   Task_count;

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
  uint32_t    index;
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 24 ***" );

  status = rtems_task_create(
    rtems_build_name( 'H', 'I', 'G', 'H' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
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
      (RTEMS_MAXIMUM_PRIORITY / 2u) + 1u,
      RTEMS_MINIMUM_STACK_SIZE,
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
  uint32_t    index;

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_wake_after: yield -- returns to caller",
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
    benchmark_timer_initialize();
  else if ( Task_count == OPERATION_COUNT ) {
    end_time = benchmark_timer_read();

    put_time(
      "rtems_task_wake_after: yields -- preempts caller",
      end_time,
      OPERATION_COUNT,
      overhead,
      CALLING_OVERHEAD_TASK_WAKE_AFTER
    );

  puts( "*** END OF TEST 24 ***" );
    rtems_test_exit( 0 );
  }
  (void) rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
}
