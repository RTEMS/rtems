/*  Test_task
 *
 *  This routine serves as a test task.  It verifies the basic task
 *  switching capabilities of the executive.
 *
 *  Input parameters:  NONE
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

rtems_task Test_task(
  rtems_task_argument unused
)
{
  rtems_id          tid;
  rtems_time_of_day time;
  rtems_unsigned32  task_index;
  rtems_status_code status;

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  task_index = task_number( tid );
  while( FOREVER ) {
    status = rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time );
    if ( time.second >= 35 ) {
      puts( "*** END OF CLOCK TICK TEST ***" );
      exit( 0 );
    }
    put_name( Task_name[ task_index ], FALSE );
    print_time( " - rtems_clock_get - ", &time, "\n" );
    status = rtems_task_wake_after( task_index * 5 * TICKS_PER_SECOND );
  }
}
