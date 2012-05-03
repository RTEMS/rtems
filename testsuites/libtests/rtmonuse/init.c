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
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
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

rtems_task Init(
  rtems_task_argument argument
)
{
  uint32_t          index;
  rtems_status_code status;
  rtems_id          rmid;
  rtems_name        period;

  puts( "\n\n*** RATE MONOTONIC PERIOD STATISTICS TEST ***" );

  period =  rtems_build_name( 'I', 'G', 'N', 'R' );
  status = rtems_rate_monotonic_create( period, &rmid );
  directive_failed( status, "rtems_rate_monotonic_create" );
  printf(
    "INIT - rtems_rate_monotonic_create id = 0x%08" PRIxrtems_id " (stays inactive)\n",
    rmid
  );


  Task_name[ 1 ] =  rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] =  rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ] =  rtems_build_name( 'T', 'A', '3', ' ' );
  Task_name[ 4 ] =  rtems_build_name( 'T', 'A', '4', ' ' );
  Task_name[ 5 ] =  rtems_build_name( 'T', 'A', '5', ' ' );

  for ( index = 1 ; index <= 5 ; index++ ) {
    status = rtems_task_create(
      Task_name[ index ],
      Priorities[ index ],
      RTEMS_MINIMUM_STACK_SIZE * 4,
      RTEMS_DEFAULT_MODES,
      (index == 5) ? RTEMS_FLOATING_POINT : RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[ index ]
    );
    directive_failed( status, "rtems_task_create loop" );
  }

  for ( index = 1 ; index <= 5 ; index++ ) {
    status = rtems_task_start( Task_id[ index ], Task_1_through_5, index );
    directive_failed( status, "rtems_task_start loop" );
  }

  Count.count[ 1 ] = 0;
  Count.count[ 2 ] = 0;
  Count.count[ 3 ] = 0;
  Count.count[ 4 ] = 0;
  Count.count[ 5 ] = 0;

  status = rtems_task_suspend( RTEMS_SELF );
  directive_failed( status, "rtems_task_suspend of RTEMS_SELF" );
}
