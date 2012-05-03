/*  Task5
 *
 *  This routine serves as a test task.  It obtains semaphore 1 correctly
 *  once, then waits for semaphore 1 again.  Task 1 should delete the
 *  semaphore, thus waking this task up.
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

rtems_task Task5(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  puts( "TA5 - rtems_semaphore_obtain - wait forever on SM1" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 1 ],
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_semaphore_obtain on SM1" );
  puts( "TA5 - got SM1" );

  puts( "TA5 - rtems_semaphore_obtain - wait forever on SM1" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 1 ],
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  fatal_directive_status(
    status,
    RTEMS_OBJECT_WAS_DELETED,
    "rtems_semaphore_obtain on SM1"
  );
  puts( "TA5 - SM1 deleted by TA1" );

  puts( "*** END OF TEST 12 ***" );
  rtems_test_exit( 0 );
}
