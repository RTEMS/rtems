/*  Task_3
 *
 *  This routine serves as a test task.  It simply obtains semaphore
 *  2 and waits forever attempting to obtain semaphore 3.
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

rtems_task Task_3(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  puts( "TA3 - rtems_semaphore_obtain - wait forever on SM2" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 2 ],
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_semaphore_obtain on SM2" );
  puts( "TA3 - got SM2" );

  puts( "TA3 - rtems_semaphore_release - release SM2" );
  status = rtems_semaphore_release( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_release on SM2" );

  puts( "TA3 - rtems_semaphore_obtain - wait forever on SM3" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 3 ],
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_semaphore_obtain on SM3" );
}
