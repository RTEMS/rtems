/*  Priority_task
 *
 *  This routine serves as a test task.  It verifies the semaphore manager.
 *
 *  Input parameters:
 *    its_index - priority index
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

#include "system.h"

rtems_task Priority_task(
  rtems_task_argument its_index
)
{
  rtems_interval      timeout;
  rtems_task_priority its_priority;
  rtems_task_priority current_priority;
  rtems_status_code   status;
  rtems_unsigned32    index;

  its_priority = Task_priority[ its_index ];

  if ( its_priority < 3 )
    timeout = 5 * TICKS_PER_SECOND;
  else
    timeout = RTEMS_NO_TIMEOUT;

  put_name( Priority_task_name[ its_index ], FALSE );
  puts( " - rtems_semaphore_obtain - wait forever on SM2" );

  status = rtems_semaphore_obtain(
    Semaphore_id[ 2 ],
    RTEMS_DEFAULT_OPTIONS,
    timeout
  );
  directive_failed( status, "rtems_semaphore_obtain of SM2" );

  if ( its_priority < 64 ) {
    printf( "PRI%d - WHY AM I HERE? (pri=%d)", its_index, its_priority );
    rtems_test_exit( 0 );
  }

  if ( its_index == 5 )
    puts( "PRI5 - rtems_task_suspend - until all priority tasks blocked" );
  status = rtems_task_suspend( RTEMS_SELF );
  directive_failed( status, "rtems_task_suspend" );

  puts( "PRI5 - rtems_task_delete - all tasks waiting on SM2" );
  for ( index = 1 ; index < 5 ; index++ ) {
    status = rtems_task_delete( Priority_task_id[ index ] );
    directive_failed( status, "rtems_task_delete loop" );
  }

  puts( "PRI5 - rtems_semaphore_obtain - nested" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 2 ],
    RTEMS_DEFAULT_OPTIONS,
    timeout
  );
  directive_failed( status, "rtems_semaphore_obtain nested" );

  puts( "PRI5 - rtems_semaphore_release - nested" );
  status = rtems_semaphore_release( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_release nested " );

  puts( "PRI5 - rtems_semaphore_release - restore priority" );
  status = rtems_semaphore_release( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_release" );

  status = rtems_task_set_priority(
    RTEMS_SELF,
    RTEMS_CURRENT_PRIORITY,
    &current_priority
  );
  directive_failed( status, "PRI5 rtems_task_set_priority CURRENT" );
  printf( "PRI5 - priority of PRI5 is %d\n", current_priority );

  (void) rtems_task_suspend( RTEMS_SELF );
}
