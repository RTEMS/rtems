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
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  tswitch.c,v 1.4 1995/12/19 20:18:44 joel Exp
 */

#include "system.h"

#include "cpuuse.h"

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
        CPU_usage_Dump();
        puts( "*** END OF CPU USAGE LIBRARY TEST ***" );
        exit( 0 );
      }
      break;

    case 0:
    default:
      break;
  }
}
