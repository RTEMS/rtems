/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is a user initialization task and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
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

#define CONFIGURE_INIT
#include "system.h"

#include "edfparams.h"

rtems_task Init(
  rtems_task_argument argument
)
{
  uint32_t    index;
  rtems_status_code status;

  puts( "\n\n*** TEST EDF SCHEDULER 3 ***" );

  build_task_name();

  for ( index = 1 ; index <= NUM_TASKS ; index++ ) {
    status = rtems_task_create(
      Task_name[ index ],
      Priorities[ index ],
      RTEMS_MINIMUM_STACK_SIZE * 4,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[ index ]
    );
    directive_failed( status, "rtems_task_create loop" );
  }

  for ( index = 1 ; index <= NUM_PERIODIC_TASKS ; index++ ) {
    status = rtems_task_start( Task_id[ index ], Tasks_Periodic, index );
    directive_failed( status, "rtems_task_start loop" );
  }

  for ( index = NUM_PERIODIC_TASKS+1 ; index <= NUM_TASKS ; index++ ) {
    status = rtems_task_start( Task_id[ index ], Tasks_Aperiodic, index );
    directive_failed( status, "rtems_task_start loop" );
  }

  rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
