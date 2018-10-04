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

#include <rtems/rtems/tasksimpl.h>

const char rtems_test_name[] = "TASK ERROR 04";

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code   status;
  
  TEST_BEGIN();

  Task_id[ 0 ] = rtems_task_self();

  Task_name[ 1 ]       =  rtems_build_name( 'T', 'A', '1', ' ' );

  /* priority of 0 error */
  status = rtems_task_create(
     Task_name[1],
     0,
     RTEMS_MINIMUM_STACK_SIZE,
     RTEMS_DEFAULT_MODES,
     RTEMS_DEFAULT_ATTRIBUTES,
     &Task_id[ 1 ]
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_PRIORITY,
    "rtems_task_create with illegal priority"
  );
  puts( "INIT - rtems_task_create - priority of 0 - RTEMS_INVALID_PRIORITY" );

  /* priority > 255 error */
  status = rtems_task_create(
     Task_name[1],
     UINT32_C(0x80000000),
     RTEMS_MINIMUM_STACK_SIZE,
     RTEMS_DEFAULT_MODES,
     RTEMS_DEFAULT_ATTRIBUTES,
     &Task_id[ 1 ]
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_PRIORITY,
    "rtems_task_create with illegal priority"
  );
  puts(
    "INIT - rtems_task_create - priority too high - RTEMS_INVALID_PRIORITY"
  );

  status = rtems_task_create(
    Task_name[ 1 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_restart( Task_id[ 1 ], 0 );
  fatal_directive_status(
    status,
    RTEMS_INCORRECT_STATE,
    "rtems_task_restart of DORMANT task"
  );
  puts( "INIT - rtems_task_restart - RTEMS_INCORRECT_STATE" );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  rtems_task_exit();
}
