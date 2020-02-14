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
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

const char rtems_test_name[] = "SP 54";

static void *Init( uintptr_t ignored )
{
  rtems_status_code                    status;
  rtems_task_priority                  pri;
  rtems_id                             id;
  const rtems_api_configuration_table *config;

  /*
   *  It is possible that since this thread prints and there is no idle
   *  task, that the system could fail miserably. :(
   */
  TEST_BEGIN();

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

  rtems_test_assert( rtems_configuration_get_do_zero_of_workspace() );

  config = rtems_configuration_get_rtems_api_configuration();
  rtems_test_assert( config->number_of_initialization_tasks == 0 );
  rtems_test_assert( config->User_initialization_tasks_table == NULL );

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

/*
 *  In this application, the initialization task performs the system
 *  initialization and then transforms itself into the idle task.
 */
#define CONFIGURE_IDLE_TASK_BODY Init
#define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION

#define CONFIGURE_DIRTY_MEMORY

/*
 *  Ensure we test the case where memory is zero.
 */
#define CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
