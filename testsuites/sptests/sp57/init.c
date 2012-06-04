/*
 *  @file
 *
 *  Restart a task which is delaying
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_task Delay_task(rtems_task_argument ignored);

rtems_task Delay_task(
  rtems_task_argument ignored
)
{
  rtems_status_code    status;

  puts( "Delay - rtems_task_wake_after - OK" );
  status = rtems_task_wake_after( RTEMS_MILLISECONDS_TO_TICKS(2000) );
  directive_failed( status, "wake after" );

  puts( "ERROR - delay task woke up!!" );
  rtems_test_exit(0);
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code    status;
  rtems_id             task_id;

  puts( "\n\n*** TEST 57 ***" );

  puts( "Init - rtems_task_create - delay task - OK" );
  status = rtems_task_create(
     rtems_build_name( 'T', 'A', '1', ' ' ),
     1,
     RTEMS_MINIMUM_STACK_SIZE,
     RTEMS_DEFAULT_OPTIONS,
     RTEMS_DEFAULT_ATTRIBUTES,
     &task_id
  );
  directive_failed( status, "rtems_task_create" );

  puts( "Init - rtems_task_start - delay task - OK" );
  status = rtems_task_start( task_id, Delay_task, 0 );
  directive_failed( status, "rtems_task_start" );

  puts( "Init - rtems_task_wake_after - let delay task block - OK" );
  status = rtems_task_wake_after( RTEMS_MILLISECONDS_TO_TICKS(1000) );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "Init - rtems_task_restart - delay task - OK" );
  status = rtems_task_restart( task_id, 0 );
  directive_failed( status, "rtems_task_restart" );

  puts( "*** END OF TEST 57 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             2
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
