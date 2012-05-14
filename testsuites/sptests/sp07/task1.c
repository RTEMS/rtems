/*
 *  COPYRIGHT (c) 1989-2011.
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

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_status_code   status;
  rtems_task_priority the_priority;
  rtems_task_priority previous_priority;
  rtems_id            my_id;

  my_id = rtems_task_self();

  rtems_test_pause();

  status = rtems_task_set_priority(
    RTEMS_SELF,
    RTEMS_CURRENT_PRIORITY,
    &the_priority
  );
  directive_failed( status, "rtems_task_set_priority" );
  printf(
    "TA1 - rtems_task_set_priority - get initial "
       "priority of self: %02" PRIdrtems_task_priority "\n",
    the_priority
  );

  while( FOREVER ) {
    status = rtems_task_get_note( my_id, RTEMS_NOTEPAD_8, &the_priority );
    directive_failed( status, "rtems_task_get_note" );
    printf(
      "TA1 - rtems_task_get_note - get RTEMS_NOTEPAD_8 - "
         "current priority: %02" PRIdrtems_task_priority "\n",
      the_priority
    );

    if ( --the_priority == 0 ) {
      puts( "TA1 - rtems_task_suspend - suspend TA2" );
      status = rtems_task_suspend( Task_id[ 2 ] );
      directive_failed( status, "rtems_task_suspend" );

      puts( "TA1 - rtems_task_set_priority - set priority of TA2 ( blocked )" );
      status = rtems_task_set_priority( Task_id[ 2 ], 5, &previous_priority );
      directive_failed( status, "rtems_task_set_priority" );

      status = rtems_task_delete( Task_id[ 2 ] );
      directive_failed( status, "rtems_task_delete of TA2" );

      status = rtems_task_delete( RTEMS_SELF );
      directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
    }

    printf(
      "TA1 - rtems_task_set_note - set TA2's RTEMS_NOTEPAD_8: "
          "%02" PRIdrtems_task_priority "\n",
      the_priority
    );
    status = rtems_task_set_note( Task_id[ 2 ], RTEMS_NOTEPAD_8, the_priority );
    directive_failed( status, "rtems_task_set_note" );

    printf(
      "TA1 - rtems_task_set_priority - set TA2's priority: "
          "%02" PRIdrtems_task_priority "\n",
      the_priority
    );
    status = rtems_task_set_priority(
               Task_id[ 2 ],
               the_priority,
               &previous_priority
    );
    directive_failed( status, "rtems_task_set_priority" );
  }
}
