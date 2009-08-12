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

#include <tmacros.h>

void* Task_1(
  void *argument
);

rtems_task Init(
  rtems_task_argument ignored
)
{
  int                  status, ceiling, old_ceiling;
  rtems_id             Mutex_id;
  
  puts( "\n\n*** TEST 65 ***" );

  /*
   *  Create binary semaphore (a.k.a. Mutex) with Priority Ceiling
   *  attribute.
   */
  status = rtems_semaphore_create(
    rtems_build_name( 's','e','m','1' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_PRIORITY_CEILING,
    1,
    &Mutex_id
  );
  directive_failed( status, "rtems_semaphore_create" );

  puts( "*** END OF TEST 65 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         1
#define CONFIGURE_MAXIMUM_SEMAPHORES    1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
