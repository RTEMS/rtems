/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is a user initialization task and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
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
#undef EXTERN
#define EXTERN
#include "conftbl.h"
#include "gvar.h"

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_time_of_day time;
  rtems_unsigned32  index;
  rtems_status_code status;

  puts( "\n\n*** TEST 24 ***" );

  build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );

  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  Task_name[ 1 ] =  rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] =  rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ] =  rtems_build_name( 'T', 'A', '3', ' ' );

  Timer_name[ 1 ] =  rtems_build_name( 'T', 'M', '1', ' ' );
  Timer_name[ 2 ] =  rtems_build_name( 'T', 'M', '2', ' ' );
  Timer_name[ 3 ] =  rtems_build_name( 'T', 'M', '3', ' ' );

  for ( index = 1 ; index <= 3 ; index++ ) {
    status = rtems_task_create(
      Task_name[ index ],
      1,
      2048,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[ index ]
    );
    directive_failed( status, "rtems_task_create loop" );

    status = rtems_timer_create(
      Timer_name[ index ],
      &Timer_id[ index ]
    );
    directive_failed( status, "rtems_timer_create loop" );
  }

  for ( index = 1 ; index <= 3 ; index++ ) {
    status = rtems_task_start( Task_id[ index ], Task_1_through_3, index );
    directive_failed( status, "rtems_task_start loop" );
  }

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
