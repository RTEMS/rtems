/*  Task_1
 *
 *  This routine serves as a test task.  It verifies the basic task
 *  switching capabilities of the executive.
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
#include "inttest.h"

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_id          tid;
  rtems_time_of_day time;
  rtems_unsigned32  task_index;
  INTEGER_DECLARE;

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  directive_failed( status, "rtems_task_ident" );

  task_index = task_number( tid );

  INTEGER_LOAD( INTEGER_factors[ task_index ] );

  put_name( Task_name[ task_index ], FALSE );
  printf( " - integer base = (0x%x)\n", INTEGER_factors[ task_index ] );

  while( FOREVER ) {
    status = rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time );
    directive_failed( status, "rtems_clock_get" );

    put_name( Task_name[ task_number( tid ) ], FALSE );
    print_time( " - rtems_clock_get - ", &time, "\n" );

    INTEGER_CHECK( INTEGER_factors[ task_index ] );

    status = rtems_task_wake_after(
      ( task_number( tid ) ) * 5 * TICKS_PER_SECOND
    );
    directive_failed( status, "rtems_task_wake_after" );
  }
}
