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
