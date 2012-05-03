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
  rtems_id          tid2;
  rtems_id          tid3;
  rtems_status_code status;
  rtems_name        tid2_name;
  uint32_t    previous_priority;

  puts( "TA1 - rtems_task_wake_after - sleep 1 second" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  status = rtems_task_ident( Task_name[ 2 ], RTEMS_SEARCH_ALL_NODES, &tid2 );
  directive_failed( status, "rtems_task_ident of TA2" );

  printf(
    "TA1 - rtems_task_ident - tid of TA2 (0x%.8" PRIxrtems_id ")\n", tid2
  );
  status = rtems_object_get_classic_name( tid2, &tid2_name );
  directive_failed( status, "rtems_object_get_classic_name of TA2" );

  printf( "TA1 - rtems_get_classic_name - id -> name of TA2 %sOK\n",
    (tid2_name != Task_name[2]) ? "NOT " : "" );

  status = rtems_task_ident( Task_name[ 3 ], RTEMS_SEARCH_ALL_NODES, &tid3 );
  directive_failed( status, "rtems_task_ident of TA3" );

  printf(
    "TA1 - rtems_task_ident - tid of TA3 (0x%.8" PRIxrtems_id ")\n", tid3
  );

  status = rtems_task_set_priority( tid3, 2, &previous_priority );
  directive_failed( status, "rtems_task_set_priority" );

  puts( "TA1 - rtems_task_set_priority - set TA3's priority to 2" );

  puts( "TA1 - rtems_task_suspend - suspend TA2" );
  status = rtems_task_suspend( tid2 );
  directive_failed( status, "rtems_task_suspend of TA2" );

  puts( "TA1 - rtems_task_delete - delete TA2" );
  status = rtems_task_delete( tid2 );
  directive_failed( status, "rtems_task_delete of TA2" );

  puts( "TA1 - rtems_task_wake_after - sleep for 5 seconds" );
  status = rtems_task_wake_after( 5 * rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "*** END OF TEST 2 ***" );
  rtems_test_exit( 0 );
}
