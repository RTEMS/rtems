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
 *  init.c,v 1.6 1995/12/19 20:21:09 joel Exp
 */

#define TEST_INIT
#include "system.h"

#include "rtmonuse.h"

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_unsigned32  index;
  rtems_status_code status;

  puts( "\n\n*** RATE MONOTONIC PERIOD STATISTICS TEST ***" );
  Period_usage_Initialize();

  Task_name[ 1 ] =  rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] =  rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ] =  rtems_build_name( 'T', 'A', '3', ' ' );
  Task_name[ 4 ] =  rtems_build_name( 'T', 'A', '4', ' ' );
  Task_name[ 5 ] =  rtems_build_name( 'T', 'A', '5', ' ' );

  for ( index = 1 ; index <= 5 ; index++ ) {
    status = rtems_task_create(
      Task_name[ index ],
      Priorities[ index ],
      RTEMS_MINIMUM_STACK_SIZE * 4,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[ index ]
    );
    directive_failed( status, "rtems_task_create loop" );
  }

  for ( index = 1 ; index <= 5 ; index++ ) {
    status = rtems_task_start( Task_id[ index ], Task_1_through_5, index );
    directive_failed( status, "rtems_task_start loop" );
  }

  Count.count[ 1 ] = 0;
  Count.count[ 2 ] = 0;
  Count.count[ 3 ] = 0;
  Count.count[ 4 ] = 0;
  Count.count[ 5 ] = 0;

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
