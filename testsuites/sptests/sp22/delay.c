/*  Delayed_resume
 *
 *  This routine is scheduled to be fired as a timer service routine.
 *  When fired this subprogram resumes Task_1.
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

rtems_timer_service_routine Delayed_resume(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code status;

  status = rtems_task_resume( Task_id[ 1 ] );
  directive_failed( status, "rtems_task_resume of self" );
}
