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
#include <unistd.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_task Task_1(rtems_task_argument arg);

#if defined(INHERIT_CEILING)
  #define TEST_NAME                "66"
  #define TASK_PRIORITY            2
#else
  #define TEST_NAME                "65"
  #define TASK_PRIORITY            1
#endif

rtems_task Init(
  rtems_task_argument ignored
)
{
  int                  status;
  rtems_id             Mutex_id, Task_id;

  puts( "\n\n*** TEST " TEST_NAME " ***" );

  /*
   *  Create binary semaphore (a.k.a. Mutex) with Priority Ceiling
   *  attribute.
   */

  puts( "Creating semaphore" );
  status = rtems_semaphore_create(
    rtems_build_name( 's','e','m','1' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_PRIORITY_CEILING,
    1,
    &Mutex_id
  );
  directive_failed( status, "rtems_semaphore_create" );

  puts( "Calling rtems_semaphore_obtain" );
  status = rtems_semaphore_obtain( Mutex_id, RTEMS_DEFAULT_OPTIONS, 0 );
  directive_failed( status, "rtems_semaphore_obtain" );

  puts( "Calling rtems_task_create" );
  status = rtems_task_create( rtems_build_name( 'T', 'A', 'S', '1' ),
    TASK_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id
  );
  directive_failed( status, "rtems_task_create" );

  puts( "Calling rtems_task_start" );
  status = rtems_task_start( Task_id, Task_1, (rtems_task_argument)&Mutex_id );
  directive_failed( status, "rtems_task_start" );

  sleep(1);

  puts( "Calling semaphore release" );
  status = rtems_semaphore_release( Mutex_id );
  directive_failed( status, "rtems_semaphore_release" );

  puts( "*** END OF TEST 65 ***" );

  rtems_test_exit(0);
}

rtems_task Task_1(
  rtems_task_argument arg
)
{
  int status_in_task;
  rtems_id *Mutex_id = (rtems_id *)arg;

  puts( "Init Task_1: Obtaining semaphore" );
  status_in_task = rtems_semaphore_obtain(
    *Mutex_id,
    RTEMS_DEFAULT_OPTIONS,
    0
  );
  directive_failed( status_in_task, "Task_1 rtems_semaphore_obtain" );
  return;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         2
#define CONFIGURE_MAXIMUM_SEMAPHORES    1
#define CONFIGURE_INIT_TASK_PRIORITY    TASK_PRIORITY
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
