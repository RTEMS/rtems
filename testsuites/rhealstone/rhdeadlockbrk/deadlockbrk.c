/*
 * Copyright (c) 2014 Daniel Ramirez. (javamonn@gmail.com)
 *
 * This file's license is 2-clause BSD as in this distribution's LICENSE file.
 */

#include <timesys.h>
#include <rtems/btimer.h>

const char rtems_test_name[] = "RHDEADLOCKBRK";

#define BENCHMARKS 20000

rtems_task Task01( rtems_task_argument ignored );
rtems_task Task02( rtems_task_argument ignored );
rtems_task Task03( rtems_task_argument ignored );
rtems_task Init( rtems_task_argument ignored );

rtems_id           Task_id[3];
rtems_name         Task_name[3];
rtems_id           sem_id;
rtems_name         sem_name;
rtems_status_code  status;

uint32_t count;
uint32_t telapsed;
uint32_t tswitch_overhead;
uint32_t tobtain_overhead;
uint32_t sem_exe;

rtems_task Init( rtems_task_argument ignored )
{
  rtems_attribute      sem_attr;
  rtems_task_priority  pri;
  rtems_mode           prev_mode;

  Print_Warning();

  TEST_BEGIN();

  sem_attr = RTEMS_INHERIT_PRIORITY | RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY;

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
    26,  /* High priority task */
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[0]
  );
  directive_failed( status, "rtems_task_create of TA01" );

  Task_name[1] = rtems_build_name( 'T','A','0','2' );
  status = rtems_task_create(
    Task_name[1],
    28,  /* Mid priority task */
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[1]
  );
  directive_failed( status, "rtems_task_create of TA02" );

  Task_name[2] = rtems_build_name( 'T','A','0','3' );
  status = rtems_task_create(
    Task_name[2],
    30,  /* Low priority task */
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[2]
  );
  directive_failed( status, "rtems_task_create of TA03" );

  /* find overhead of obtaining semaphore */
  benchmark_timer_initialize();
  rtems_semaphore_obtain( sem_id, RTEMS_WAIT, 0 );
  tobtain_overhead = benchmark_timer_read();
  rtems_semaphore_release( sem_id );

  rtems_task_mode( RTEMS_PREEMPT, RTEMS_PREEMPT_MASK, &prev_mode );
  /* Lower own priority so tasks can start up and run */
  rtems_task_set_priority( RTEMS_SELF, 40, &pri );

  /* Get time of benchmark with no semaphores involved, i.e. find overhead */
  sem_exe = 0;
  status = rtems_task_start( Task_id[2], Task03, 0 );
  directive_failed( status, "rtems_task_start of TA03" );

  /* Get time of benchmark with semaphores */
  sem_exe = 1;
  status = rtems_task_restart( Task_id[2], 0 );
  directive_failed( status, "rtems_task_start of TA03" );

  /* Should never reach here */
  rtems_test_assert( false );
}

rtems_task Task01( rtems_task_argument ignored )
{
  /* All tasks have had time to start up once TA01 is running */

  /* Benchmark code */
  benchmark_timer_initialize();
  for ( count = 0; count < BENCHMARKS; count++ ) {
    if ( sem_exe == 1 ) {
      /* Block on call */
      rtems_semaphore_obtain( sem_id, RTEMS_WAIT, 0 );
    }

    if ( sem_exe == 1 ) {
      /* Release semaphore immediately after obtaining it */
      rtems_semaphore_release( sem_id );
    }

    /* Suspend self, go to TA02 */
    rtems_task_suspend( RTEMS_SELF );
  }
  telapsed = benchmark_timer_read();

  /* Check which run this was */
  if (sem_exe == 0) {
    tswitch_overhead = telapsed;
    rtems_task_suspend( Task_id[1] );
    rtems_task_suspend( Task_id[2] );
    rtems_task_suspend( RTEMS_SELF );
  } else {
    put_time(
       "Rhealstone: Deadlock Break",
       telapsed,
       BENCHMARKS,              /* Total number of times deadlock broken*/
       tswitch_overhead,        /* Overhead of loop and task switches */
       tobtain_overhead
    );
    TEST_END();
    rtems_test_exit( 0 );
  }

}

rtems_task Task02( rtems_task_argument ignored )
{
  /* Start up TA01, get preempted */
  if ( sem_exe == 1) {
    status = rtems_task_restart( Task_id[0], 0);
    directive_failed( status, "rtems_task_start of TA01");
  } else {
    status = rtems_task_start( Task_id[0], Task01, 0);
    directive_failed( status, "rtems_task_start of TA01");
  }

  /* Benchmark code */
  for ( ; count < BENCHMARKS ; ) {
    /* Suspend self, go to TA01 */
    rtems_task_suspend( RTEMS_SELF );

    /* Wake up TA01, get preempted */
    rtems_task_resume( Task_id[0] );
  }
}

rtems_task Task03( rtems_task_argument ignored )
{
  if (sem_exe == 1) {
    /* Low priority task holds mutex */
    rtems_semaphore_obtain( sem_id, RTEMS_WAIT, 0 );
  }

  /* Start up TA02, get preempted */
  if ( sem_exe == 1) {
    status = rtems_task_restart( Task_id[1], 0);
    directive_failed( status, "rtems_task_start of TA02");
  } else {
    status = rtems_task_start( Task_id[1], Task02, 0);
    directive_failed( status, "rtems_task_start of TA02");
  }

  /* Benchmark code */
  for ( ; count < BENCHMARKS ; ) {
    if ( sem_exe == 1 ) {
      /* Preempted by TA01 upon release */
      rtems_semaphore_release( sem_id );
    }

    if ( sem_exe == 1 ) {
      /* Prepare for next Benchmark */
      rtems_semaphore_obtain( sem_id, RTEMS_WAIT, 0 );
    }
    /* Wake up TA02, get preempted */
    rtems_task_resume( Task_id[1] );
  }
}

/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_TICKS_PER_TIMESLICE        0
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_SEMAPHORES 1
#define CONFIGURE_MAXIMUM_TASKS 4

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
