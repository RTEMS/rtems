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

const char rtems_test_name[] = "TIME TEST 25";

rtems_id Semaphore_id;

rtems_task High_tasks(
  rtems_task_argument argument
);

rtems_task Low_task(
  rtems_task_argument argument
);

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_id          task_id;
  uint32_t    index;
  rtems_status_code status;

  Print_Warning();

  TEST_BEGIN();

  status = rtems_semaphore_create(
    rtems_build_name( 'S', 'M', '1', ' ') ,
    0,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &Semaphore_id
  );
  directive_failed( status, "rtems_semaphore_create of SM1" );

  status = rtems_task_create(
    rtems_build_name( 'L', 'O', 'W', ' ' ),
    RTEMS_MAXIMUM_PRIORITY - 1u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  directive_failed( status, "rtems_task_create LOW" );

  status = rtems_task_start( task_id, Low_task, 0 );
  directive_failed( status, "rtems_task_start LOW" );

  for ( index=1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'T', 'I', 'M', 'E' ),
      (RTEMS_MAXIMUM_PRIORITY / 2u) + 1u,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &task_id
    );
    directive_failed( status, "rtems_task_create LOOP" );

    status = rtems_task_start( task_id, High_tasks, 0 );
    directive_failed( status, "rtems_task_start LOOP" );
  }

  rtems_task_exit();
}

rtems_task High_tasks(
  rtems_task_argument argument
)
{
  (void) rtems_semaphore_obtain(
    Semaphore_id,
    RTEMS_DEFAULT_OPTIONS,
    0xffffffff
  );
}

rtems_task Low_task(
  rtems_task_argument argument
)
{
  benchmark_timer_initialize();
    (void) rtems_clock_tick();
  end_time = benchmark_timer_read();

  put_time(
    "rtems_clock_tick: only case",
    end_time,
    1,
    0,
    0
  );

  TEST_END();
  rtems_test_exit( 0 );
}
