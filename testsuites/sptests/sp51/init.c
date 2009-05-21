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

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code sc;
  rtems_id          mutex;

  puts( "\n\n*** TEST 51 ***" );

  puts( "Create semaphore" );
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
  fatal_directive_status( sc, RTEMS_INTERNAL_ERROR, "rtems_semaphore_obtain" );

  puts( "Release semaphore we did not obtain-- violate ceiling" );
  sc = rtems_semaphore_release( mutex );
  fatal_directive_status(
    sc, RTEMS_NOT_OWNER_OF_RESOURCE, "rtems_semaphore_release" );

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
