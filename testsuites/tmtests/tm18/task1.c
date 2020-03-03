/*
 *  COPYRIGHT (c) 1989-2009.
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

const char rtems_test_name[] = "TIME TEST 18";

uint32_t   taskcount;
rtems_task_priority taskpri;

extern void test_init(void);

rtems_task First_task(
  rtems_task_argument argument
);

rtems_task Middle_tasks(
  rtems_task_argument argument
);

rtems_task Last_task(
  rtems_task_argument argument
);


rtems_task Init(
  rtems_task_argument argument
)
{
  Print_Warning();

  TEST_BEGIN();

  test_init();

  rtems_task_exit();
}

void test_init(void)
{
  rtems_id          id;
  rtems_task_entry  task_entry;
  uint32_t          index;
  rtems_status_code status;

  for ( index = 0 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'T', 'I', 'M', 'E' ),
      (RTEMS_MAXIMUM_PRIORITY / 2u) + 1u,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "rtems_task_create loop" );

    if ( index == OPERATION_COUNT ) task_entry = Last_task;
    else if ( index == 0 )          task_entry = First_task;
    else                            task_entry = Middle_tasks;


    status = rtems_task_start( id, task_entry, 0 );
    directive_failed( status, "rtems_task_start loop" );
  }

}

rtems_task First_task(
  rtems_task_argument argument
)
{
  benchmark_timer_initialize();

  rtems_task_exit();
}

rtems_task Middle_tasks(
  rtems_task_argument argument
)
{
  rtems_task_exit();
}

rtems_task Last_task(
  rtems_task_argument argument
)
{
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_delete: calling task",
    end_time,
    OPERATION_COUNT,
    0,
    0
  );

  TEST_END();
  rtems_test_exit( 0 );
}
