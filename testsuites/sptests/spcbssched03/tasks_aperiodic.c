/*  Tasks_Aperiodic
 *
 *  This routine serves as a test task for the CBS scheduler
 *  implementation. This is for aperiodic task execution.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

rtems_task Tasks_Aperiodic(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  int start, stop, now;

  put_name( Task_name[ argument ], FALSE );

  status = rtems_task_wake_after( 2 + Phases[argument] );
  directive_failed( status, "rtems_task_wake_after" );

  start = rtems_clock_get_ticks_since_boot();
  printf("AT%" PRIdPTR "-S ticks:%d\n", argument, start);
  /* active computing */

  while(FOREVER) {
    now = rtems_clock_get_ticks_since_boot();
    if (now >= start + Execution[argument]) break;
  }
  stop = rtems_clock_get_ticks_since_boot();
  printf("P%" PRIdPTR "-F ticks:%d\n", argument, stop);

  /* delete SELF */
  fflush(stdout);
  printf( "Killing task %" PRIdPTR "\n", argument);
  rtems_task_exit();
}
