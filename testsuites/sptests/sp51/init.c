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
  rtems_task_argument argument
)
{
  rtems_status_code sc;
  rtems_id          mutex;

  puts( "\n\n*** TEST 51 ***" );

  puts( "Create semaphore - priority ceiling locked - violate ceiling" );
  sc = rtems_semaphore_create(
    rtems_build_name( 'S', 'E', 'M', '1' ),
    0,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY_CEILING | RTEMS_PRIORITY,
    (RTEMS_MAXIMUM_PRIORITY - 4u),
    &mutex
  );
  fatal_directive_status(sc, RTEMS_INVALID_PRIORITY, "rtems_semaphore_create");

  puts( "Create semaphore - priority ceiling unlocked" );
  sc = rtems_semaphore_create(
    rtems_build_name( 'S', 'E', 'M', '1' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY_CEILING | RTEMS_PRIORITY,
    (RTEMS_MAXIMUM_PRIORITY - 4u),
    &mutex
  );
  directive_failed( sc, "rtems_semaphore_create" );

  puts( "Obtain semaphore -- violate ceiling" );
  sc = rtems_semaphore_obtain( mutex, RTEMS_DEFAULT_OPTIONS, 0 );
  fatal_directive_status(
    sc, RTEMS_INVALID_PRIORITY, "rtems_semaphore_obtain" );

  /* This returns successful because RTEMS eats the unneeded unlock */
  puts( "Release semaphore we did not obtain" );
  sc = rtems_semaphore_release( mutex );
  directive_failed( sc, "rtems_semaphore_release" );

  puts( "*** END OF TEST 51 ***" );
  rtems_test_exit( 0 );
}


/**************** START OF CONFIGURATION INFORMATION ****************/

#define CONFIGURE_INIT
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         1
#define CONFIGURE_MAXIMUM_SEMAPHORES    1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>

/****************  END OF CONFIGURATION INFORMATION  ****************/
