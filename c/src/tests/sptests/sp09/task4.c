/*  Task_4
 *
 *  This routine serves as a test task.  Its only purpose in life is to
 *  generate the error where a rate monotonic period is accessed by a
 *  task other than its creator.
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

  status = rtems_rate_monotonic_cancel( Period_id[ 1 ] );
  fatal_directive_status(
    status,
    RTEMS_NOT_OWNER_OF_RESOURCE,
    "rtems_rate_monotonic_cancel not the owner"
  );
  puts( "TA4 - rtems_rate_monotonic_cancel - RTEMS_NOT_OWNER_OF_RESOURCE" );

  status = rtems_rate_monotonic_period( Period_id[ 1 ], 5 );
  fatal_directive_status(
    status,
    RTEMS_NOT_OWNER_OF_RESOURCE,
    "rtems_rate_monotonic_period not the owner"
  );
  puts( "TA4 - rtems_rate_monotonic_period - RTEMS_NOT_OWNER_OF_RESOURCE" );

  puts( "TA4 - rtems_task_delete - delete self - RTEMS_SUCCESSFUL" );
  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of TA4" );
}
