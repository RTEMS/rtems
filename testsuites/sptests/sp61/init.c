/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <tmacros.h>


rtems_task Init(
  rtems_task_argument ignored
)
{
  puts( "\n\n*** TEST 61 ***" );

  puts( "Init - Set current System State to Shutdown" );
  _System_state_Set( SYSTEM_STATE_SHUTDOWN );

  puts( "Init - rtems_shutdown_executive when already shutdown" );
  rtems_shutdown_executive( 0 );

  puts( "Init - restore System State and shutdown for real" );
  _System_state_Set( SYSTEM_STATE_UP );

  puts( "*** END OF TEST 61 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         2
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
