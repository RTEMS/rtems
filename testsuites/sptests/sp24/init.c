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
  rtems_time_of_day time;
  uint32_t    index;
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
      RTEMS_MINIMUM_STACK_SIZE * 2,
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
