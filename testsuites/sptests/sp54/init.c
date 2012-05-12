/**
 *  @@file
 *
 *  Odd Id Cases where API configured but No Threads
 *    + Possibly Valid Id passed to directive
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

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code    status;
  rtems_task_priority  pri;
  rtems_id             id;

  /*
   *  It is possible that since this thread prints and there is no idle
   *  task, that the system could fail miserably. :(
   */
  puts( "\n\n*** TEST 54 ***" );

  puts( "Init - use valid id of API class with no objects" );
  status = rtems_task_set_priority(
    rtems_build_id(0x2,0x1,0x01,0x0001) /* 0xa010001 */,
    RTEMS_CURRENT_PRIORITY,
    &pri
  );
  fatal_directive_status( status, RTEMS_INVALID_ID, "rtems_task_set_priority" );

  puts( "Init - lookup name within API class with no objects" );
  status = rtems_task_ident(
    rtems_build_id( 0, 0, 0x12, 0x3456) /* 0x123456 */,
    RTEMS_SEARCH_ALL_NODES,
    &id
  );
  fatal_directive_status( status, RTEMS_INVALID_NAME, "rtems_task_ident" );

  puts( "*** END OF TEST 54 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

/*
 *  In this application, the initialization task performs the system
 *  initialization and then transforms itself into the idle task.
 */
#define CONFIGURE_IDLE_TASK_BODY Init
#define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION

/*
 *  Another odd case to hit.  Since we use the Init task is Idle task
 *  configuration, we can dummy up the initialization task configuration
 *  to have a non-NULL pointer and 0 tasks.
 */

#define CONFIGURE_HAS_OWN_INIT_TASK_TABLE 1

rtems_initialization_tasks_table Initialization_tasks[1] =
  { { 0, }};

#define CONFIGURE_INIT_TASK_TABLE      Initialization_tasks
#define CONFIGURE_INIT_TASK_TABLE_SIZE 0
#define CONFIGURE_INIT_TASK_STACK_SIZE 0

/*
 *  Ensure we test the case where memory is zero.  Not
 *  all BSPs will set this to TRUE by default.
 */
#define CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY TRUE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
