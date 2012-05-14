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

  puts( "\n\n*** TEST 11 ***" );

  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( 'T', 'A', '2', ' ' );

  status = rtems_task_create(
    Task_name[ 1 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_create(
    Task_name[ 2 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TA2" );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  status = rtems_task_start( Task_id[ 2 ], Task_2, 0 );
  directive_failed( status, "rtems_task_start of TA2" );

  Timer_name[ 1 ] = rtems_build_name( 'T', 'M', '1', ' ' );
  Timer_name[ 2 ] = rtems_build_name( 'T', 'M', '2', ' ' );
  Timer_name[ 3 ] = rtems_build_name( 'T', 'M', '3', ' ' );
  Timer_name[ 4 ] = rtems_build_name( 'T', 'M', '4', ' ' );
  Timer_name[ 5 ] = rtems_build_name( 'T', 'M', '5', ' ' );
  Timer_name[ 6 ] = rtems_build_name( 'T', 'M', '6', ' ' );

  status = rtems_timer_create( Timer_name[ 1 ], &Timer_id[ 1 ] );
  directive_failed( status, "rtems_timer_create of TM1" );

  status = rtems_timer_create( Timer_name[ 2 ], &Timer_id[ 2 ] );
  directive_failed( status, "rtems_timer_create of TM2" );

  status = rtems_timer_create( Timer_name[ 3 ], &Timer_id[ 3 ] );
  directive_failed( status, "rtems_timer_create of TM3" );

  status = rtems_timer_create( Timer_name[ 4 ], &Timer_id[ 4 ] );
  directive_failed( status, "rtems_timer_create of TM4" );

  status = rtems_timer_create( Timer_name[ 5 ], &Timer_id[ 5 ] );
  directive_failed( status, "rtems_timer_create of TM5" );

  status = rtems_timer_create( Timer_name[ 6 ], &Timer_id[ 6 ] );
  directive_failed( status, "rtems_timer_create of TM6" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
