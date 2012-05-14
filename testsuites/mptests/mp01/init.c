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
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_time_of_day time;
  char              c;

  printf(
    "\n\n*** TEST 1 -- NODE %" PRIu32 " ***\n",
    Multiprocessing_configuration.node
  );

  if ( Multiprocessing_configuration.node != 1 ) c = 'S';
  else                                           c = 'M';

  Task_name[ 1 ] = rtems_build_name( c, 'A', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( c, 'A', '2', ' ' );
  Task_name[ 3 ] = rtems_build_name( c, 'A', '3', ' ' );

  build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  puts( "Creating task 1 (Global)" );
  status = rtems_task_create(
    Task_name[ 1 ],
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_GLOBAL,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of Task 1" );

  puts( "Creating task 2 (Global)" );
  status = rtems_task_create(
    Task_name[ 2 ],
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_TIMESLICE,
    RTEMS_GLOBAL,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of Task 2" );

  puts( "Creating task 3 (Local)" );
  status = rtems_task_create(
    Task_name[ 3 ],
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 3 ]
  );
  directive_failed( status, "rtems_task_create of Task 3" );

  status = rtems_task_start( Task_id[ 1 ], Test_task, 0 );
  directive_failed( status, "rtems_task_start of Task 1" );

  status = rtems_task_start( Task_id[ 2 ], Test_task, 0 );
  directive_failed( status, "rtems_task_start of Task 2" );

  status = rtems_task_start( Task_id[ 3 ], Test_task, 0 );
  directive_failed( status, "rtems_task_start of Task 3" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
