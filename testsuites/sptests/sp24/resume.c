/*  Resume_task
 *
 *  This subprogram is scheduled as a timer service routine.  When
 *  it fires it resumes the task which is mapped to this timer.
 *
 *  Input parameters:  NONE
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

rtems_timer_service_routine Resume_task(
  rtems_id  timer_id,
  void     *ignored_address
)
{
  rtems_id          task_to_resume;
  rtems_status_code status;

  task_to_resume = Task_id[ rtems_get_index( timer_id ) ];
  status = rtems_task_resume( task_to_resume );
  directive_failed( status, "rtems_task_resume" );
}
