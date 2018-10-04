/*
 * Copyright (c) 2014 Daniel Ramirez. (javamonn@gmail.com)
 *
 * This file's license is 2-clause BSD as in this distribution's LICENSE file.
 */

#include <timesys.h>
#include <rtems/btimer.h>

const char rtems_test_name[] = "RHTASKPREEMPT";

#define BENCHMARKS 50000   /* Number of benchmarks to run and average over */

rtems_task Task02( rtems_task_argument ignored );
rtems_task Task01( rtems_task_argument ignored );
rtems_task Init( rtems_task_argument ignored );

rtems_id           Task_id[2];
rtems_name         Task_name[2];

uint32_t           telapsed;          /* total time elapsed during benchmark */
uint32_t           tloop_overhead;    /* overhead of loops */
uint32_t           tswitch_overhead;  /* overhead of time it takes to switch
                                       * from TA02 to TA01, includes rtems_suspend
                                       * overhead
                                       */
unsigned long      count1;
rtems_status_code  status;

rtems_task Task01( rtems_task_argument ignored )
{
  /* Start up TA02, get preempted */
  status = rtems_task_start( Task_id[1], Task02, 0);
  directive_failed( status, "rtems_task_start of TA02");

  tswitch_overhead = benchmark_timer_read();

  benchmark_timer_initialize();
  /* Benchmark code */
  for ( count1 = 0; count1 < BENCHMARKS; count1++ ) {
    rtems_task_resume( Task_id[1] );  /* Awaken TA02, preemption occurs */
  }

  /* Should never reach here */
  rtems_test_assert( false );
}

rtems_task Task02( rtems_task_argument ignored )
{
  /* Find overhead of task switch back to TA01 (not a preemption) */
  benchmark_timer_initialize();
  rtems_task_suspend( RTEMS_SELF );

  /* Benchmark code */
  for ( ; count1 < BENCHMARKS - 1; ) {
    rtems_task_suspend( RTEMS_SELF );
  }

  telapsed = benchmark_timer_read();
  put_time(
     "Rhealstone: Task Preempt",
     telapsed,                     /* Total time of all benchmarks */
     BENCHMARKS - 1,               /* BENCHMARKS - 1 total preemptions */
     tloop_overhead,               /* Overhead of loops */
     tswitch_overhead              /* Overhead of task switch back to TA01 */
  );

  TEST_END();
  rtems_test_exit( 0 );
}

rtems_task Init( rtems_task_argument ignored )
{
  Print_Warning();

  TEST_BEGIN();

  Task_name[0] = rtems_build_name( 'T','A','0','1' );
  status = rtems_task_create(
    Task_name[0],
    30,               /* TA01 is low priority task */
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[0]
  );
  directive_failed( status, "rtems_task_create of TA01");

  Task_name[1] = rtems_build_name( 'T','A','0','2' );
  status = rtems_task_create(
    Task_name[1],
    28,               /* TA02 is high priority task */
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[1]
  );
  directive_failed( status, "rtems_task_create of TA02");

  /* Find loop overhead */
  benchmark_timer_initialize();
  for ( count1 = 0; count1 < ( BENCHMARKS * 2 ) - 1; count1++ ) {
     /* no statement */ ;
  }
  tloop_overhead = benchmark_timer_read();

  status = rtems_task_start( Task_id[0], Task01, 0 );
  directive_failed( status, "rtems_task_start of TA01");

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
