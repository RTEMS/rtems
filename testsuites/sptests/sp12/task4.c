/*  Task_4
 *
 *  This routine serves as a test task.  It waits forever attempting
 *  to obtain semaphore 1.  However, it should never get the semaphore!
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

rtems_task Task_4(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  puts( "TA4 - rtems_semaphore_obtain - wait forever on SM1" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 1 ],
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_semaphore_obtain on SM1" );
}
