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
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define TEST_INIT
#include "system.h"

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_time_of_day time;
  rtems_status_code status;

  puts( "\n\n*** MALLOC TEST ***" );

  build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ] = rtems_build_name( 'T', 'A', '3', ' ' );
  Task_name[ 4 ] = rtems_build_name( 'T', 'A', '4', ' ' );
  Task_name[ 5 ] = rtems_build_name( 'T', 'A', '5', ' ' );

  status = rtems_task_create(
     Task_name[ 1 ],
     1,
     TASK_STACK_SIZE,
     RTEMS_DEFAULT_MODES,
     RTEMS_FLOATING_POINT,
     &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_create(
     Task_name[ 2 ],
     1,
     TASK_STACK_SIZE,
     RTEMS_DEFAULT_MODES,
     RTEMS_FLOATING_POINT,
     &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TA2" );

  status = rtems_task_create(
     Task_name[ 3 ],
     1,
     TASK_STACK_SIZE,
     RTEMS_DEFAULT_MODES,
     RTEMS_FLOATING_POINT,
     &Task_id[ 3 ]
  );
  directive_failed( status, "rtems_task_create of TA3" );

  status = rtems_task_create(
     Task_name[ 4 ],
     1,
     TASK_STACK_SIZE,
     RTEMS_DEFAULT_MODES,
     RTEMS_FLOATING_POINT,
     &Task_id[ 4 ]
  );
  directive_failed( status, "rtems_task_create of TA4" );

  status = rtems_task_create(
     Task_name[ 5 ],
     1,
     TASK_STACK_SIZE,
     RTEMS_DEFAULT_MODES,
     RTEMS_FLOATING_POINT,
     &Task_id[ 5 ]
  );
  directive_failed( status, "rtems_task_create of TA5" );

  status = rtems_task_start( Task_id[ 1 ], Task_1_through_5, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  status = rtems_task_start( Task_id[ 2 ], Task_1_through_5, 0 );
  directive_failed( status, "rtems_task_start of TA2" );

  status = rtems_task_start( Task_id[ 3 ], Task_1_through_5, 0 );
  directive_failed( status, "rtems_task_start of TA3" );

  status = rtems_task_start( Task_id[ 4 ], Task_1_through_5, 0 );
  directive_failed( status, "rtems_task_start of TA4" );

  status = rtems_task_start( Task_id[ 5 ], Task_1_through_5, 0 );
  directive_failed( status, "rtems_task_start of TA5" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
