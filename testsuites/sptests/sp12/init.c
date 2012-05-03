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
 *  COPYRIGHT (c) 1989-2009.
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

  puts( "\n\n*** TEST 12 ***" );

  Task_name[ 1 ]          = rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ]          = rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ]          = rtems_build_name( 'T', 'A', '3', ' ' );
  Task_name[ 4 ]          = rtems_build_name( 'T', 'A', '4', ' ' );
  Task_name[ 5 ]          = rtems_build_name( 'T', 'A', '5', ' ' );

  Priority_task_name[ 1 ] = rtems_build_name( 'P', 'R', 'I', '1' );
  Priority_task_name[ 2 ] = rtems_build_name( 'P', 'R', 'I', '2' );
  Priority_task_name[ 3 ] = rtems_build_name( 'P', 'R', 'I', '3' );
  Priority_task_name[ 4 ] = rtems_build_name( 'P', 'R', 'I', '4' );
  Priority_task_name[ 5 ] = rtems_build_name( 'P', 'R', 'I', '5' );

  Semaphore_name[ 1 ]     = rtems_build_name( 'S', 'M', '1', ' ' );
  Semaphore_name[ 2 ]     = rtems_build_name( 'S', 'M', '2', ' ' );
  Semaphore_name[ 3 ]     = rtems_build_name( 'S', 'M', '3', ' ' );

  status = rtems_semaphore_create(
    Semaphore_name[ 1 ],
    1,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &Semaphore_id[ 1 ]
  );
  directive_failed( status, "rtems_semaphore_create of SM1" );

  status = rtems_semaphore_create(
    Semaphore_name[ 2 ],
    0,
    RTEMS_PRIORITY,
    RTEMS_NO_PRIORITY,
    &Semaphore_id[ 2 ]
  );
  directive_failed( status, "rtems_semaphore_create of SM2" );

  status = rtems_semaphore_create(
    Semaphore_name[ 3 ],
    1,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &Semaphore_id[ 3 ]
  );
  directive_failed( status, "rtems_semaphore_create of SM3" );

  puts( "INIT - Forward priority queue test" );
  Priority_test_driver( 0 );

  puts( "INIT - Backward priority queue test" );
  Priority_test_driver( (RTEMS_MAXIMUM_PRIORITY / 2u) + 1u );

rtems_test_pause();

  puts( "INIT - Binary Semaphore and Priority Inheritance Test" );

  status = rtems_semaphore_delete( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_delete of SM2 #1" );

  puts( "INIT - rtems_semaphore_create - allocated binary semaphore" );
  status = rtems_semaphore_create(
    Semaphore_name[ 2 ],
    0,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    RTEMS_NO_PRIORITY,
    &Semaphore_id[ 2 ]
  );
  directive_failed( status, "rtems_semaphore_create of priority inherit SM2" );

  puts( "INIT - rtems_semaphore_release - allocated binary semaphore" );
  status = rtems_semaphore_release( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_release of SM2" );

  puts( "INIT - rtems_semaphore_delete - allocated binary semaphore" );
  status = rtems_semaphore_delete( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_delete of SM2 #2" );

  status = rtems_semaphore_create(
    Semaphore_name[ 2 ],
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    RTEMS_NO_PRIORITY,
    &Semaphore_id[ 2 ]
  );
  directive_failed( status, "rtems_semaphore_create of priority inherit SM2" );

  Priority_test_driver( PRIORITY_INHERIT_BASE_PRIORITY );

rtems_test_pause();

  status = rtems_semaphore_delete( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_delete of SM2 #3" );

  status = rtems_semaphore_create(
    Semaphore_name[ 2 ],
    0,
    RTEMS_PRIORITY,
    RTEMS_NO_PRIORITY,
    &Semaphore_id[ 2 ]
  );
  directive_failed( status, "rtems_semaphore_create of priority SM2" );

  status = rtems_semaphore_release( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_release of SM2" );

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
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TA2" );

  status = rtems_task_create(
    Task_name[ 3 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 3 ]
  );
  directive_failed( status, "rtems_task_create of TA3" );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  status = rtems_task_start( Task_id[ 2 ], Task_2, 0 );
  directive_failed( status, "rtems_task_start of TA2" );

  status = rtems_task_start( Task_id[ 3 ], Task_3, 0 );
  directive_failed( status, "rtems_task_start of TA3" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
