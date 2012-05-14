/*  Tasks_Aperiodic
 *
 *  This routine serves as a test task for the EDF scheduler
 *  implementation. This is for aperiodic task execution.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

rtems_task Tasks_Aperiodic(
  rtems_task_argument argument
)
{
  rtems_status_code   status;
  int                 start;
  int                 stop;
  int                 now;

  put_name( Task_name[ argument ], FALSE );

  status = rtems_task_wake_after( 2 + Phases[argument] );
  directive_failed( status, "rtems_task_wake_after" );

  if ( argument == 6 ) {
    rtems_task_suspend( Task_id[5] );
    rtems_task_resume( Task_id[5] );
  }

  rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &start);
  printf("AT%" PRIdPTR "-S ticks:%d\n", argument, start);
  /* active computing */

  while(FOREVER) {
    rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
    if (now >= start + Execution[argument]) break;
  }
  rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &stop);
  printf("P%" PRIdPTR "-F ticks:%d\n", argument, stop);

  /* delete SELF */
  fflush(stdout);
  printf( "Killing task %" PRIdPTR "\n", argument);
  status = rtems_task_delete(RTEMS_SELF);
  directive_failed(status, "rtems_task_delete of RTEMS_SELF");
}
