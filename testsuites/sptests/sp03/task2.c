/*  Task_2
 *
 *  This routine serves as a test task.  It repeatedly gets the system time
 *  and date, converts it to printable form, and print it.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
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

rtems_task Task_2(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_time_of_day time;

  while( FOREVER ) {
    status = rtems_task_wake_after( 1*TICKS_PER_SECOND );
    directive_failed( status, "rtems_task_wake_after" );

    status = rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time );
    directive_failed( status, "rtems_clock_get" );

    if ( time.second >= 17 ) {
      puts( "*** END OF TEST 3 *** " );
      exit( 0 );
    }

    put_name( Task_name[ 2 ], FALSE );
    print_time( "  ", &time, "\n" );
  }
}
