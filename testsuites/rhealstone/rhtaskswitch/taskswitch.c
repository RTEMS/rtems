/*
 * Copyright (c) 2014 Daniel Ramirez. (javamonn@gmail.com)
 *
 * This file's license is 2-clause BSD as in this distribution's LICENSE file.
 */

#include <timesys.h>
#include <rtems/btimer.h>

#define BENCHMARKS 50000

const char rtems_test_name[] = "RHTASKSWITCH";

rtems_task Task01( rtems_task_argument ignored );
rtems_task Task02( rtems_task_argument ignored );
rtems_task Init( rtems_task_argument ignored );

rtems_id           Task_id[2];
rtems_name         Task_name[2];
uint32_t           loop_overhead;
uint32_t           dir_overhead;
unsigned long      count1, count2;
rtems_status_code  status;

rtems_task Task02( rtems_task_argument ignored )
{
  uint32_t telapsed;

  /* All overhead accounted for now, we can begin benchmark */
  benchmark_timer_initialize();

  for ( count1 = 0; count1 < BENCHMARKS - 1; count1++ ) {
    rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  }

  telapsed = benchmark_timer_read();
  put_time(
     "Rhealstone: Task switch",
     telapsed,
     ( BENCHMARKS * 2 ) - 1,   /* ( BENCHMARKS * 2 ) - 1 total benchmarks */
     loop_overhead,            /* Overhead of loop */
     dir_overhead              /* Overhead of rtems_task_wake_after directive */
  );

  TEST_END();
  rtems_test_exit( 0 );
}

rtems_task Task01( rtems_task_argument ignored )
{
  status = rtems_task_start( Task_id[1], Task02, 0 );
  directive_failed( status, "rtems_task_start of TA02" );

  /* Yield processor so second task can startup and run */
  rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );

  for ( count2 = 0; count2 < BENCHMARKS; count2++ ) {
    rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  }

  /* Should never reach here */
  rtems_test_assert( false );

}

rtems_task Init( rtems_task_argument ignored )
{
  Print_Warning();

  TEST_BEGIN();

  Task_name[0] = rtems_build_name( 'T','A','0','1' );
  status = rtems_task_create(
    Task_name[0],
    30,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[0]
  );
  directive_failed( status, "rtems_task_create of TA01" );

  Task_name[1] = rtems_build_name( 'T','A','0','2' );
  status = rtems_task_create(
    Task_name[1],
    30,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[1]
  );
  directive_failed( status, "rtems_task_create of TA02" );

  /* find overhead of routine (no task switches) */
  benchmark_timer_initialize();
  for ( count1 = 0; count1 < BENCHMARKS - 1; count1++ ) {
    /* rtems_task_wake_after( RTEMS_YIELD_PROCESSOR ) */
  }
  for ( count2 = 0; count2 < BENCHMARKS; count2++ ) {
    /* rtems_task_wake_after( RTEMS_YIELD_PROCESSOR ) */
  }
  loop_overhead = benchmark_timer_read();

  /* find overhead of rtems_task_wake_after call (no task switches) */
  benchmark_timer_initialize();
  rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  dir_overhead = benchmark_timer_read();

  status = rtems_task_start( Task_id[0], Task01, 0);
  directive_failed( status, "rtems_task_start of TA01" );

  rtems_task_exit();
}

/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER
#define CONFIGURE_TICKS_PER_TIMESLICE        0
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 3
#define CONFIGURE_INIT
#include <rtems/confdefs.h>
