/*  Preempt_task
 *
 *  This routine serves as a test task.  It verifies the task manager.
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
 *
 *  $Id$
 */

#include "system.h"

rtems_task Preempt_task(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  puts( "PREEMPT - rtems_task_delete - deleting self" );
  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_PREEMPT" );
}
