/*  Task_switch
 *
 *  This routine is the tswitch user extension.  It determines which
 *  task is being switched to and displays a message indicating the
 *  time and date that it gained control.
 *
 *  Input parameters:
 *    unused  - pointer to currently running TCB
 *    heir    - pointer to heir TCB
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
 *  tswitch.c,v 1.2 1995/05/09 20:32:43 joel Exp
 */

#include "system.h"

rtems_extension Task_switch( 
  rtems_tcb *unused,
  rtems_tcb *heir
)
{
  rtems_unsigned32  index;
  rtems_time_of_day time;
  rtems_status_code status;

  index = task_number( heir->Object.id );

  switch( index ) {
    case 1:
    case 2:
    case 3:
      Run_count[ index ] += 1;

      status = rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time );
      directive_failed( status, "rtems_clock_get" );

      put_name( Task_name[ index ], FALSE );
      print_time( "- ", &time, "\n" );

      if ( time.second >= 16 ) {
        puts( "*** END OF TEST 4 ***" );
        exit( 0 );
      }
      break;

    case 0:
    default:
      break;
  }
}
