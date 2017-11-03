/*
 * Copyright (c) 2014 Daniel Ramirez. (javamonn@gmail.com)
 *
 * This file's license is 2-clause BSD as in this distribution's LICENSE file.
 */

#include <timesys.h>
#include <rtems/btimer.h>

const char rtems_test_name[] = "RHSEMSHUFFLE";

#define BENCHMARKS 50000

rtems_task Task01( rtems_task_argument ignored );
rtems_task Task02( rtems_task_argument ignored );
rtems_task Init( rtems_task_argument ignored );

rtems_id   Task_id[2];
rtems_name Task_name[2];
rtems_id    sem_id;
rtems_name  sem_name;

uint32_t    telapsed;
uint32_t    tswitch_overhead;
uint32_t    count;
uint32_t    sem_exe;

rtems_task Init( rtems_task_argument ignored )
{
  rtems_status_code    status;
  rtems_attribute      sem_attr;
  rtems_task_priority  pri;
  rtems_mode           prev_mode;

  Print_Warning();

  TEST_BEGIN();

  sem_attr =  RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY;

  sem_name = rtems_build_name( 'S','0',' ',' ' );
  status = rtems_semaphore_create(
    sem_name,
    1,
    sem_attr,
    0,
    &sem_id
  );
  directive_failed( status, "rtems_semaphore_create of S0" );

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
  directive_failed( status , "rtems_task_create of TA02\n" );

  rtems_task_mode( RTEMS_PREEMPT, RTEMS_PREEMPT_MASK, &prev_mode );
  /* Lower own priority so TA01 can start up and run */
  rtems_task_set_priority( RTEMS_SELF, 40, &pri);

  /* Get time of benchmark with no semaphore shuffling */
  sem_exe = 0;
  status = rtems_task_start( Task_id[0], Task01, 0 );
  directive_failed( status, "rtems_task_start of TA01" );

  /* Get time of benchmark with semaphore shuffling */
  sem_exe = 1;
  status = rtems_task_restart( Task_id[0], 0 );
  directive_failed( status, "rtems_task_restart of TA01" );

  /* Should never reach here */
  rtems_test_assert( false );
}

rtems_task Task01( rtems_task_argument ignored )
{
  rtems_status_code status;

  /* Start up TA02, yield so it can run */
  if ( sem_exe == 0 ) {
    status = rtems_task_start( Task_id[1], Task02, 0 );
    directive_failed( status, "rtems_task_start of TA02" );
  } else {
    status = rtems_task_restart( Task_id[1], 0 );
    directive_failed( status, "rtems_task_restart of TA02" );
  }
  rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );

  /* Benchmark code */
  for ( ; count < BENCHMARKS ; ) {
    if ( sem_exe == 1 ) {
      rtems_semaphore_obtain( sem_id, RTEMS_WAIT, 0 );
    }
    rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );

    if ( sem_exe == 1 ) {
      rtems_semaphore_release( sem_id );
    }
    rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  }

  /* Should never reach here */
  rtems_test_assert( false );
}

rtems_task Task02( rtems_task_argument ignored )
{

  /* Benchmark code */
  benchmark_timer_initialize();
  for ( count = 0; count < BENCHMARKS; count++ ) {
    if ( sem_exe == 1 ) {
      rtems_semaphore_obtain( sem_id, RTEMS_WAIT, 0 );
    }
    rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );

    if ( sem_exe == 1 ) {
      rtems_semaphore_release( sem_id );
    }
    rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  }
  telapsed = benchmark_timer_read();

  /* Check which run this was */
  if (sem_exe == 0) {
    tswitch_overhead = telapsed;
    rtems_task_suspend( Task_id[0] );
    rtems_task_suspend( RTEMS_SELF );
  } else {
    put_time(
       "Rhealstone: Semaphore Shuffle",
       telapsed,
       (BENCHMARKS * 2),        /* Total number of semaphore-shuffles*/
       tswitch_overhead,        /* Overhead of loop and task switches */
       0
    );
    TEST_END();
    rtems_test_exit( 0 );
  }
}

/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 3
#define CONFIGURE_MAXIMUM_SEMAPHORES 1
#define CONFIGURE_INIT
#include <rtems/confdefs.h>
