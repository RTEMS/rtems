/*  Delayed_resume
 *
 *  This routine is scheduled to be fired as a timer service routine.
 *  When fired this subprogram resumes Task_1.
 *
 *  Input parameters:  NONE
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

rtems_timer_service_routine Delayed_resume(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code status;

  status = rtems_task_resume( Task_id[ 1 ] );
  directive_failed_with_level( status, "rtems_task_resume of self", 1 );
}
