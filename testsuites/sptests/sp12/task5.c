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
  exit( 0 );
}
