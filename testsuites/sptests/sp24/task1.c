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
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

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
    status = rtems_timer_fire_after(
      Timer_id[ argument ],
      task_number( tid ) * 5 * TICKS_PER_SECOND,
      Resume_task,
      NULL
    );
    directive_failed( status, "tm_fire_after failed" );

    status = rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time );
    directive_failed( status, "rtems_clock_get failed" );

    if ( time.second >= 35 ) {
      puts( "*** END OF TEST 24 ***" );
      exit( 0 );
    }

    put_name( Task_name[ task_number( tid ) ], FALSE );
    print_time( " - rtems_clock_get - ", &time, "\n" );

    status = rtems_task_suspend( RTEMS_SELF );
    directive_failed( status, "rtems_task_suspend" );
  }
}
