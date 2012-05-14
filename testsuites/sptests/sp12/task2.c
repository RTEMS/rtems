/*  Task_2
 *
 *  This routine serves as a test task.  It simply obtains semaphores
 *  1 and 2, the later when it is a high priority task.
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

#include "system.h"

rtems_task Task_2(
  rtems_task_argument argument
)
{
  rtems_status_code   status;
  rtems_task_priority previous_priority;

  puts( "TA2 - rtems_semaphore_obtain - wait forever on SM1" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 1 ],
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  puts( "TA2 - got SM1" );
  directive_failed( status, "rtems_semaphore_obtain on SM1" );

  puts( "TA2 - rtems_semaphore_release - release SM1" );
  status = rtems_semaphore_release( Semaphore_id[ 1 ] );
  directive_failed( status, "rtems_semaphore_release on SM1" );

  puts( "TA2 - rtems_task_set_priority - make self highest priority task" );
  status = rtems_task_set_priority( RTEMS_SELF, 3, &previous_priority );
  directive_failed( status, "rtems_task_set_priority on TA2" );

  puts( "TA2 - rtems_semaphore_obtain - wait forever on SM2" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 2 ],
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  puts( "TA2 - got SM2" );
  directive_failed( status, "rtems_semaphore_obtain on SM2" );

  puts( "TA2 - rtems_semaphore_release - release SM2" );
  status = rtems_semaphore_release( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_release on SM2" );

  puts( "TA2 - rtems_task_delete - delete self" );
  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of TA2" );
}
