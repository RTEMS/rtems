/*  Print_time
 *
 *  This routine prints the name of Task_1 and the current time of day.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
