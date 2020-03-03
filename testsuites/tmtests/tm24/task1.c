/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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

const char rtems_test_name[] = "TIME TEST 24";

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

  TEST_BEGIN();

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
  rtems_task_exit();
}

rtems_task High_task(
  rtems_task_argument argument
)
{
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
    "rtems_task_wake_after: yield returns to caller",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  Task_count = 0;

  rtems_task_exit();
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
      "rtems_task_wake_after: yields preempts caller",
      end_time,
      OPERATION_COUNT,
      overhead,
      0
    );

  TEST_END();
    rtems_test_exit( 0 );
  }
  (void) rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
}
