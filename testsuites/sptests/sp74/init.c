/* 
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

/*
 *  Keep the names and IDs in global variables so another task can use them.
 */
rtems_id   Task_id[ 3 ];         /* array of task ids */
rtems_name Task_name[ 3 ];       /* array of task names */

rtems_task Test_task(
  rtems_task_argument index
)
{
  puts( "Successfully yielded it to higher priority task" );

  puts( "*** END OF SP74 TEST ***" );
  rtems_test_exit( 0 );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code   	status;

  puts( "\n\n*** SP74 (YIELD) TEST ***" );

  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( 'T', 'A', '2', ' ' );

  puts( "Create TA1 at higher priority task" );
  status = rtems_task_create(
    Task_name[ 1 ],
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "create 1" );

  puts( "Create TA2 at equal priority task" );
  status = rtems_task_create(
    Task_name[ 2 ],
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "create 2" );

  status = rtems_task_start( Task_id[ 1 ], Test_task, 1 );
  directive_failed( status, "start 1" );

  status = rtems_task_start( Task_id[ 2 ], Test_task, 2 );
  directive_failed( status, "start 2" );

  puts( "Yield to TA1" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "yield" );

  puts( "*** should now get here ***" );
}

/* configuration information */
#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS           3
#define CONFIGURE_INIT_TASK_PRIORITY      2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
