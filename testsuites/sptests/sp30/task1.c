/*  Task_1_through_3
 *
 *  This task is a cyclic version of test1 to asssure that the times
 *  displayed are not skewed as in test1.  "TA1" is printed once every
 *  5 seconds, "TA2" is printed once every 10 seconds, and "TA3" is
 *  printed once every 15 seconds.  The times displayed should be
 *  in multiples of 5, 10, and 15 for TA1, TA2, and TA3 respectively.
 *  If the times are skewed from these values, then the calendar time
 *  does not correspond correctly with the number of ticks.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

rtems_task Task_1_through_3(
  rtems_task_argument argument
)
{
  rtems_id          tid;
  rtems_time_of_day time;
  rtems_status_code status;

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  directive_failed( status, "rtems_task_ident of self" );

  while ( FOREVER )  {
    status = rtems_timer_server_fire_after(
      Timer_id[ argument ],
      (task_number( tid ) - 1) * 5 * rtems_clock_get_ticks_per_second(),
      Resume_task,
      (void *) &tid
    );
    directive_failed( status, "rtems_timer_server_fire_after failed" );

    status = rtems_clock_get_tod( &time );
    directive_failed( status, "rtems_clock_get_tod failed" );

    if ( time.second >= 35 ) {
      puts( "*** END OF TEST 30 ***" );
      rtems_test_exit( 0 );
    }

    put_name( Task_name[ task_number( tid ) - 1 ], FALSE );
    print_time( " - rtems_clock_get_tod - ", &time, "\n" );

    status = rtems_task_suspend( RTEMS_SELF );
    directive_failed( status, "rtems_task_suspend" );
  }
}
