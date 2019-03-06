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

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "TIME TEST 29";

rtems_name Period_name;

rtems_task Tasks(
  rtems_task_argument argument
);

rtems_task Low_task(
  rtems_task_argument argument
);

uint32_t   Task_count;

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_id          id;
  uint32_t    index;
  rtems_status_code status;

  Print_Warning();

  TEST_BEGIN();

  Period_name = rtems_build_name( 'P', 'R', 'D', ' ' );

  benchmark_timer_initialize();
    (void) rtems_rate_monotonic_create( Period_name, &id );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_rate_monotonic_create: only case",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
    (void) rtems_rate_monotonic_period( id, 10 );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_rate_monotonic_period: initiate period returns to caller",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
    (void) rtems_rate_monotonic_period( id, RTEMS_PERIOD_STATUS );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_rate_monotonic_period: obtain status",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
    (void) rtems_rate_monotonic_cancel( id );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_rate_monotonic_cancel: only case",
    end_time,
    1,
    0,
    0
  );

  benchmark_timer_initialize();
    (void) rtems_rate_monotonic_delete( id );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_rate_monotonic_delete: inactive",
    end_time,
    1,
    0,
    0
  );

  status = rtems_rate_monotonic_create( Period_name, &id );
  directive_failed( status, "rtems_rate_monotonic_create" );

  status = rtems_rate_monotonic_period( id, 10 );
  directive_failed( status, "rtems_rate_monotonic_period" );

  benchmark_timer_initialize();
    rtems_rate_monotonic_delete( id );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_rate_monotonic_delete: active",
    end_time,
    1,
    0,
    0
  );

#define LOOP_TASK_PRIORITY ((RTEMS_MAXIMUM_PRIORITY / 2u) + 1u)
  for ( index=1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'T', 'E', 'S', 'T' ),
      LOOP_TASK_PRIORITY,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "rtems_task_create LOOP" );

    status = rtems_task_start( id, Tasks, 0 );
    directive_failed( status, "rtems_task_start LOOP" );
  }

#define MIDDLE_PRIORITY (RTEMS_MAXIMUM_PRIORITY - 2u)
  status = rtems_task_create(
    rtems_build_name( 'L', 'O', 'W', ' ' ),
    MIDDLE_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create LOW" );

  status = rtems_task_start( id, Low_task, 0 );
  directive_failed( status, "rtems_task_start LOW" );

  Task_count = 0;

  rtems_task_exit();
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
    benchmark_timer_initialize();

  (void) rtems_rate_monotonic_period( id, 100 );
}

rtems_task Low_task(
  rtems_task_argument argument
)
{
  uint32_t   index;

  end_time = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  put_time(
    "rtems_rate_monotonic_period: conclude periods caller blocks",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  TEST_END();
  rtems_test_exit( 0 );
}
