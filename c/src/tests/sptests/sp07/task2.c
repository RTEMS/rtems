/*  Task_2
 *
 *  This routine serves as a test task.  Plays with priorities to verify
 *  that the highest priority task is always executed.
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
  rtems_status_code   status;
  rtems_task_priority the_priority;
  rtems_task_priority previous_priority;

  while( FOREVER ) {

    status = rtems_task_get_note( RTEMS_SELF, RTEMS_NOTEPAD_8, &the_priority );
    directive_failed( status, "rtems_task_get_note" );
    printf(
"TA2 - rtems_task_get_note - get RTEMS_NOTEPAD_8 - current priority: %02d\n",
      the_priority
    );

    if ( --the_priority == 0 ) {
      puts( "TA2 - rtems_task_suspend - suspend TA1" );
      status = rtems_task_suspend( Task_id[ 1 ] );
      directive_failed( status, "rtems_task_suspend" );

      puts( "TA2 - rtems_task_set_priority - set priority of TA1 ( blocked )" );
      status = rtems_task_set_priority( Task_id[ 1 ], 5, &previous_priority );
      directive_failed( status, "rtems_task_set_priority" );

      status = rtems_task_delete( Task_id[ 1 ] );          /* TA1 is blocked */
      directive_failed( status, "rtems_task_delete of TA1" );

      status = rtems_task_delete( Task_id[ 3 ] );          /* TA3 is ready   */
      directive_failed( status, "rtems_task_delete of TA3" );

      status = rtems_task_delete( RTEMS_SELF );
      directive_failed( status, "rtems_task_delete of SELD" );

    } else {

      printf( "TA2 - rtems_task_set_note - set TA1's RTEMS_NOTEPAD_8: %02d\n",
              the_priority
      );
      status = rtems_task_set_note(Task_id[ 1 ], RTEMS_NOTEPAD_8, the_priority);
      directive_failed( status, "rtems_task_set_note" );

      printf( "TA2 - rtems_task_set_priority - set TA1's priority: %02d\n",
              the_priority
      );
      status = rtems_task_set_priority(
                 Task_id[ 1 ],
                 the_priority,
                 &previous_priority
      );
      directive_failed( status, "rtems_task_set_priority" );
    }
  }
}
