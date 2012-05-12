/**
 *  @file
 *
 *  Extension create fails
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
bool task_create(Thread_Control *executing, Thread_Control *created);

bool task_create(
  Thread_Control *executing,
  Thread_Control *created
)
{
  return false;
}

rtems_extensions_table Extensions = {
  task_create,               /* task create user extension */
  NULL,                      /* task start user extension */
  NULL,                      /* task restart user extension */
  NULL,                      /* task delete user extension */
  NULL,                      /* task switch user extension */
  NULL,                      /* task begin user extension */
  NULL,                      /* task exitted user extension */
  NULL                       /* fatal error user extension */
};

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code    status;
  rtems_id             extension;
  rtems_id             task_id;

  puts( "\n\n*** TEST 56 ***" );

  puts( "Init - rtems_extension_create - OK" );
  status = rtems_extension_create(
    rtems_build_name( 'E', 'X', 'T', ' ' ),
    &Extensions,
    &extension
  );
  directive_failed( status, "rtems_extension_create" );

  puts( "Init - rtems_task_create - create extension fails - UNSATISFIED" );
  status = rtems_task_create(
     rtems_build_name( 'T', 'A', '1', ' ' ),
     1,
     RTEMS_MINIMUM_STACK_SIZE,
     RTEMS_TIMESLICE,
     RTEMS_FLOATING_POINT,
     &task_id
  );
  fatal_directive_status( status, RTEMS_UNSATISFIED, "rtems_task_create" );

  puts( "Init - rtems_extension_delete - OK" );
  status = rtems_extension_delete( extension );
  directive_failed( status, "rtems_extension_delete" );
  puts( "*** END OF TEST 56 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             2
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS   1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
