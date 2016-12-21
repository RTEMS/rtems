/**
 * @file sprmsched04/tasks.c
 *
 * @brief A heuristic example to demonstrate how the postponed jobs are handled.
 *
 * Given two tasks with implicit deadline under EDF policy.
 * Task 1 has (4, 5) and task 2 has (4, 6), where (execution time, period/deadline).
 * For the simplicity, we only execute the first task twice.
 * In the original implementation in v4.11, no matter how many periods are
 * expired, only one job will be released with a shifted deadline assignment.
 *
 * In this example, the first job of task 2 will be blocked by the second job
 * of task 1, so that there at least one following job is postponed.
 * Due to overhead/delay, the second following job will be postponed as well.
 *
 * If the overrun handling is correct, the period of task 2 changes back to
 * normal status at time 22.
 * Otherwise, the release time of job 3 is no longer periodic.
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

#include "system.h"
#include <rtems/score/watchdogimpl.h>

/* CPU usage and Rate monotonic manger statistics */
#include "rtems/cpuuse.h"
#include "rtems/counter.h"

uint32_t    Periods[ 3 ] = { 0, 5000, 6000 };
uint32_t    Iterations[ 3 ] = { 0, 4000, 4000 };
uint32_t    tsk_counter[ 3 ] = { 0, 0, 0 };

/**
 * @brief Task body
 */
rtems_task Task(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_id    RM_period;
  rtems_id    selfid=rtems_task_self();
  uint32_t    start, end, index, flag = 0;
  rtems_counter_ticks t0 = rtems_counter_nanoseconds_to_ticks( 1000000 );

  /*create period*/
  status = rtems_rate_monotonic_create( argument, &RM_period );
  directive_failed( status, "rtems_rate_monotonic_create" );

  switch ( argument ) {
    case 1:
    case 2:
      while ( FOREVER ) {
        status = rtems_rate_monotonic_period( RM_period, Periods[ argument ]);
        if ( flag == 0 && status == RTEMS_TIMEOUT ){
          printf( "System TIMEOUT \n" );
          flag = 1;
        }else if ( flag == 1 && status == RTEMS_SUCCESSFUL ){
          printf( "System SUCCESSFUL \n" );
          flag = 0;
        }

        start = rtems_clock_get_ticks_since_boot();
        printf( "Job %d Task %d starts at tick %d.\n", tsk_counter[ argument ]+1, argument, start );
        for( index = 0; index < Iterations[ argument ]; index++ ){
          rtems_counter_delay_ticks( t0 );
        }
        end = rtems_clock_get_ticks_since_boot();
        printf( "					Job %d Task %d ends at tick %d.\n", tsk_counter[ argument ]+1, argument, end );
        if( argument == 2 ){
          if( tsk_counter[ argument ] == testnumber ){
            TEST_END();
            status = rtems_rate_monotonic_delete( RM_period );
            directive_failed( status, "rtems_rate_monotonic_delete" );
            rtems_test_exit( 0 );
          }
        }
        tsk_counter[ argument ]+=1;
        if ( argument == 1 ){
          if( tsk_counter[ argument ] == 2 ){
				    status = rtems_rate_monotonic_delete( RM_period );
            directive_failed( status, "rtems_rate_monotonic_delete" );
            status = rtems_task_delete( selfid );
            directive_failed( status, "rtems_task_delete" );
          }
        }
      }
      break;
  }
}

