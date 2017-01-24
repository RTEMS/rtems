/**
 * @brief A heuristic example to demonstrate how the postponed jobs are handled.
 *
 * Given two tasks with implicit deadline under fixed-priority scheudling.
 * Task 1 has (6, 10) and task 2 has (1, 2), where (execution time, deadline/period).
 * To force deadline misses, we reverse the rate-monotonic priority assignment
 * and only execute the highest priority task twice.
 *
 * In the original implementation in v4.11, no matter how many periods are
 * expired, RMS manager only releases a job with a shifted deadline assignment
 * in the watchdog. As the results written in sprmsched01.scn, we can see that
 * the timeout of task 2 period will be detected right after Job3 of Task2 is finished.
 * If the overrun handling is correct, the status of task 2 period will return back to
 * RTEMS_SUCCESSFUL after periodically releasing those postponed jobs (the last one is Job 9).
 *
 * Otherwise, we can see that the release time of Job 4 is no longer periodic,
 * and the RTEMS returns back to RTEMS_SUCCESSFUL right after Job 4 is finished
 * without releasing all the other postponed jobs.
 *
 */

/*
 *  COPYRIGHT (c) 2016 Kuan-Hsun Chen.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/cpuuse.h>
#include <rtems/counter.h>

#include <stdio.h>
#include <inttypes.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPRMSCHED 1";

static const uint32_t Periods[] = { 10000, 2000 };
static const uint32_t Iterations[] = { 6000, 1000 };
static const rtems_name Task_name[] = {
  rtems_build_name( 'T', 'A', '1', ' ' ),
  rtems_build_name( 'T', 'A', '2', ' ' )
};
static const rtems_task_priority Prio[3] = { 2, 5 };
static const uint32_t testnumber = 11; /* stop condition */

static uint32_t tsk_counter[] = { 0, 0 };
static rtems_id   Task_id[ 2 ];

/**
 * @brief Task body
 */
static rtems_task Task(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_id    RM_period;
  rtems_id    selfid=rtems_task_self();
  uint32_t    start, end, flag=0, index;
  rtems_counter_ticks t0;

  t0 = rtems_counter_nanoseconds_to_ticks( 1000000 ); //1ms ticks counter
  /*create period*/
  status = rtems_rate_monotonic_create( Task_name[ argument ], &RM_period );
  directive_failed( status, "rtems_rate_monotonic_create" );

  while ( FOREVER ) {
    status = rtems_rate_monotonic_period( RM_period, Periods[ argument ] );
    //directive_failed( status, "rtems_rate_monotonic_period" ); let TIMEOUT pass
    if( argument == 1 && flag == 0 && status == RTEMS_TIMEOUT ){
      flag = 1;
      printf( "RTEMS_TIMEOUT\n" );
    } else if ( flag == 1 && status == RTEMS_SUCCESSFUL ) {
      flag = 0;
      printf( "RTEMS_SUCCESSFUL\n" );
    }

    start = rtems_clock_get_ticks_since_boot();
    if ( argument == 1 )
      printf( "Job %" PRIu32 " Task %" PRIuPTR " starts at tick %" PRIu32 ".\n", tsk_counter[ argument ]+1, argument, start );
    else
      printf( "Task %" PRIuPTR " starts at tick %" PRIu32 ".\n", argument, start );
    for( index = 0; index < Iterations[ argument ]; index++ ){
      rtems_counter_delay_ticks( t0 );
    }
    end = rtems_clock_get_ticks_since_boot();
    printf( "					Job %" PRIu32" Task %" PRIuPTR " ends at tick %" PRIu32".\n", tsk_counter[ argument ]+1, argument, end );
    if( argument == 1 ){
      if( tsk_counter[ argument ] == testnumber ){
        TEST_END();
        status = rtems_rate_monotonic_delete( RM_period );
        directive_failed( status, "rtems_rate_monotonic_delete" );
        rtems_test_exit( 0 );
      }
    }

    tsk_counter[ argument ]+=1;
    if ( argument == 0 ){
      if( tsk_counter[ argument ] == 2 ){
        status = rtems_rate_monotonic_delete( RM_period );
        directive_failed( status, "rtems_rate_monotonic_delete" );
        status = rtems_task_delete( selfid );
        directive_failed( status, "rtems_task_delete" );
      }
    }
  }
}

static rtems_task Init(
	rtems_task_argument argument
)
{
  uint32_t     index;
  rtems_status_code status;

  TEST_BEGIN();

  printf( "\nTicks per second in your system: %" PRIu32 "\n", rtems_clock_get_ticks_per_second() );

  /* Create two tasks */
  for ( index = 0; index < RTEMS_ARRAY_SIZE(Task_name); ++index ){
    status = rtems_task_create(
      Task_name[ index ], Prio[index], RTEMS_MINIMUM_STACK_SIZE, RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ index ]
    );
    directive_failed( status, "rtems_task_create loop" );
  }


  /* After creating the periods for tasks, start to run them sequencially. */
  for ( index = 0; index < RTEMS_ARRAY_SIZE(Task_name); ++index ){
    status = rtems_task_start( Task_id[ index ], Task, index);
    directive_failed( status, "rtems_task_start loop");
  }
  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MICROSECONDS_PER_TICK     1000
#define CONFIGURE_MAXIMUM_TASKS             3
#define CONFIGURE_MAXIMUM_PERIODS           2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
