/**
 * @file sprmsched01/tasks.c
 *
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

#include "system.h"

/* CPU usage and Rate monotonic manger statistics */
#include "rtems/cpuuse.h"
#include "rtems/counter.h"

/* Periods for the various tasks [ticks] */
uint32_t Periods[3] = { 0, 10000, 2000 };
uint32_t Iterations[3] = { 0, 6000, 1000 };
uint32_t tsk_counter[3] = { 0, 0, 0 };

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
  uint32_t    start, end, flag=0, index;
  rtems_counter_ticks t0;

  t0 = rtems_counter_nanoseconds_to_ticks( 1000000 ); //1ms ticks counter
  /*create period*/
  status = rtems_rate_monotonic_create( argument, &RM_period );
  directive_failed( status, "rtems_rate_monotonic_create" );

  switch ( argument ) {
    case 1:
    case 2:
      while ( FOREVER ) {
        status = rtems_rate_monotonic_period( RM_period, Periods[ argument ] );
        //directive_failed( status, "rtems_rate_monotonic_period" ); let TIMEOUT pass
        if( argument == 2 && flag == 0 && status == RTEMS_TIMEOUT ){
          flag = 1;
          printf( "RTEMS_TIMEOUT\n" );
        } else if ( flag == 1 && status == RTEMS_SUCCESSFUL ) {
          flag = 0;
          printf( "RTEMS_SUCCESSFUL\n" );
        }

        start = rtems_clock_get_ticks_since_boot();
        if ( argument == 2 )
          printf( "Job %d Task %d starts at tick %d.\n", tsk_counter[ argument ]+1, argument, start );
        else
          printf( "Task %d starts at tick %d.\n", argument, start );
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

