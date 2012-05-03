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
 *  COPYRIGHT (c) 1989-2009.
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
  rtems_status_code status;

  puts( "\n\n*** TEST 15 ***" );

  Task_name[ 1 ] =  rtems_build_name( 'T', 'A', '1', ' ' );

  Partition_name[ 1 ] =  rtems_build_name( 'P', 'T', '1', ' ' );
  Partition_name[ 2 ] =  rtems_build_name( 'P', 'T', '2', ' ' );

  status = rtems_task_create(
    Task_name[ 1 ],
    4u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  puts("INIT - rtems_partition_create - partition 1");
  status = rtems_partition_create(
    Partition_name[ 1 ],
    Area_1,
    4096,
    512,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Partition_id[ 1 ]
  );
  directive_failed( status, "rtems_partition_create of PT1" );

  puts("INIT - rtems_partition_create - partition 2");
  status = rtems_partition_create(
    Partition_name[ 2 ],
    Area_2,
    274,
    128,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Partition_id[ 2 ]
  );
  directive_failed( status, "rtems_partition_create of PT2" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
