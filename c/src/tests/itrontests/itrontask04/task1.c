/*  Task_1
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

/*
 * XXX - This test requires tasks to suspend self, yet ITRON defines
 *       this as an error.
 */

void Task_1()
{
  rtems_status_code status;

  while (1) {

    puts( "TA2 - rtems_task_suspend RTEMS_SELF" );
    status = rtems_task_suspend( RTEMS_SELF );
    directive_failed( status, "TA2 rtems_task_suspend RTEMS_SELF" );

  }
}






