/*  Print_time
 *
 *  This routine prints the name of Task_1 and the current time of day.
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

void Print_time( void )
{
  rtems_time_of_day time;
  rtems_status_code status;

  status = rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time );
  directive_failed( status, "rtems_clock_get" );

  put_name( Task_name[ 1 ], FALSE );
  print_time( "- rtems_clock_get - ", &time, "\n" );
}
