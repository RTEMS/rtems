/**
 * @brief A heuristic example to demonstrate how the postponed jobs are handled in EDF.
 *
 * Given two tasks with implicit deadline under EDF policy.
 * Task 1 has (400, 500) and task 2 has (450, 550), where (required ticks, period/deadline).
 * For the simplicity, we only execute the first task three times.
 * In the original implementation in v4.11, no matter how many periods are
 * expired, only one job will be released with a shifted deadline assignment.
 *
 * In this example, task 2 will be blocked by the second and third jobs
 * of task 1, so that there are two jobs are postponed.
 * Due to the domino effects, the following jobs of task 2 will be postponed until Job 9.
 *
 * If the overrun handling is correct, the period of task 2 changes back to
 * normal status at Job 9.
 * Otherwise, the release time of job 3 is no longer periodic
 * and there is no more postponed jobs.
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

const char rtems_test_name[] = "SPEDFSCHED 4";

static const uint32_t Periods[] = { 500, 550 };
static const uint32_t Iterations[] = { 400, 450 };
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
  rtems_status_code                         status;
  rtems_id                                  RM_period;
  rtems_id                                  selfid=rtems_task_self();
  rtems_rate_monotonic_period_status        period_status;
  uint32_t                                  flag=0;

  /* create period */
  status = rtems_rate_monotonic_create( Task_name[ argument ], &RM_period );
  directive_failed( status, "rtems_rate_monotonic_create" );

  rtems_test_spin_until_next_tick();

  while ( FOREVER ) {
    status = rtems_rate_monotonic_period( RM_period, Periods[ argument ] );

    /* Do some work */
    rtems_test_spin_for_ticks( Iterations[ argument ] );

    if( argument == 1 ){
      if( status == RTEMS_TIMEOUT ){
        if( flag == 0 ){
          puts( "First time RTEMS_TIMEOUT" );
          puts( "Task 2 should have 2 postponed job due to preemption." );
          rtems_test_assert( period_status.postponed_jobs_count == 2 );
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
      if( tsk_counter[ argument ] == 3 ){
        puts("Task 1 has released 3 jobs and finished.");
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

#define CONFIGURE_SCHEDULER_EDF

#define CONFIGURE_INITIAL_EXTENSIONS \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
