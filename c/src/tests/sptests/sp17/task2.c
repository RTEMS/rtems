/*  Task_2
 *
 *  This task initializes the signal catcher, sends the first signal
 *  if running on the first node, and loops while waiting for signals.
 *
 *  NOTE: The signal catcher is not reentrant and hence RTEMS_NO_ASR must
 *        be a part of its execution mode.
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

rtems_task Task_2(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Task_2_preempted = FALSE;

  puts( "TA2 - Suspending self" );
  status = rtems_task_suspend( RTEMS_SELF );
  directive_failed( status, "rtems_task_suspend of TA2" );

  puts( "TA2 - signal_return preempted correctly" );

  Task_2_preempted = TRUE;

  status = rtems_task_suspend( RTEMS_SELF );
  directive_failed( status, "rtems_task_suspend of TA2" );
}
