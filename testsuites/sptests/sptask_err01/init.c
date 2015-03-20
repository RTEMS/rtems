/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "SP TASK ERROR 01";

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  
  TEST_BEGIN();
   
  Task_name[ 2 ]       =  rtems_build_name( 'T', 'A', '2', ' ' );

  status = rtems_task_create(
    Task_name[ 2 ],
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TA2" );
  puts( "TA1 - rtems_task_create - TA2 created - RTEMS_SUCCESSFUL" );
  
  puts( "TA1 - rtems_task_start - start TA2 - RTEMS_SUCCESSFUL" );
  status = rtems_task_start( Task_id[ 2 ], Task_2, 0 );
  directive_failed( status, "rtems_task_start of TA2" ); 
  
  puts( "TA1 - rtems_task_wake_after - yield processor - RTEMS_SUCCESSFUL" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "rtems_task_wake_after (yield)" );

  status = rtems_task_delete( Task_id[ 2 ] );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_task_delete after the task has been deleted"
  );
  puts( "TA1 - rtems_task_delete TA2 - already deleted RTEMS_INVALID_ID" );

  TEST_END();
}
