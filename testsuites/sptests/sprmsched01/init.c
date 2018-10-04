/**
 * @brief A heuristic example to demonstrate how the postponed jobs are handled in RMS.
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
 *  COPYRIGHT (c) 2016-2017 Kuan-Hsun Chen.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/cpuuse.h>
#include <tmacros.h>
#include "test_support.h"

const char rtems_test_name[] = "SPRMSCHED 1";

static const uint32_t Periods[] = { 1000, 200 };
static const uint32_t Iterations[] = { 600, 100 };
static const rtems_name Task_name[] = {
  rtems_build_name( 'T', 'A', '1', ' ' ),
  rtems_build_name( 'T', 'A', '2', ' ' )
};
static const rtems_task_priority Prio[3] = { 2, 5 };
static const uint32_t testnumber = 9; /* stop condition */

static uint32_t tsk_counter[] = { 0, 0 };
static rtems_id   Task_id[ 2 ];

/**
 * @brief Task body
 */
static rtems_task Task(
  rtems_task_argument argument
)
{
  rtems_status_code                          status;
  rtems_id                                   RM_period;
  rtems_id                                   selfid=rtems_task_self();
  rtems_rate_monotonic_period_status         period_status;
  uint32_t                                   flag=0;

  /*create period*/
  status = rtems_rate_monotonic_create( Task_name[ argument ], &RM_period );
  directive_failed( status, "rtems_rate_monotonic_create" );

  while ( FOREVER ) {
    status = rtems_rate_monotonic_period( RM_period, Periods[ argument ] );

    /* Do some work */
    rtems_test_spin_for_ticks( Iterations[ argument ] );

    if( argument == 1 ){
      if( status == RTEMS_TIMEOUT ){
        if( flag == 0 ){
          puts( "First time RTEMS_TIMEOUT" );
          puts( "Task 2 should have 3 postponed jobs due to preemption." );
          rtems_test_assert( period_status.postponed_jobs_count == 3 );
          flag = 1;
        }
      } else if ( flag == 1 && status == RTEMS_SUCCESSFUL ) {
        puts( "RTEMS_SUCCESSFUL" );
        puts( "Overrun handling is finished, now Task 2 becomes normal." );
        rtems_test_assert( period_status.postponed_jobs_count == 0 );
        flag = 0;
      }

      /* Check the status */
      status = rtems_rate_monotonic_get_status( RM_period, &period_status );
      directive_failed( status, "rate_monotonic_get_status" );

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
        puts( "Task 1 has released two jobs" );
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
  rtems_task_exit();
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MICROSECONDS_PER_TICK     1000
#define CONFIGURE_MAXIMUM_TASKS             3
#define CONFIGURE_MAXIMUM_PERIODS           2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
