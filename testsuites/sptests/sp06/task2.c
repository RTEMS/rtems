/*  Task_2
 *
 *  This routine serves as a test task.  It simply runs forever so
 *  another task can restart it.
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

rtems_task Task_2( argument )
rtems_task_argument argument;
{
  rtems_status_code status;

  puts( "TA2 - is beginning to run" );
  if ( argument == Argument ) {
    status = rtems_task_wake_after( 2 * TICKS_PER_SECOND );
    directive_failed( status, "rtems_task_wake_after" );
  }

  while( FOREVER ) {
    puts( "TA2 - rtems_task_wake_after - sleep 1/2 second" );
    status = rtems_task_wake_after( TICKS_PER_SECOND / 2 );
    directive_failed( status, "rtems_task_wake_after" );
  }
}
