/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is a user initialization task and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include <tmacros.h>
#include <stdio.h>
#include <stdlib.h>

rtems_task Init(
  rtems_task_argument argument
);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define TASK_STACK_SIZE (RTEMS_MINIMUM_STACK_SIZE*3)

#define CONFIGURE_MAXIMUM_TASKS               1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>

rtems_task Init(
  rtems_task_argument argument
)
{
  puts( "\n\n*** PUTENV/GETENV TEST ***" );

  puts( "putenv(\"FOO=BAR\") - expected to work" );
  putenv ("FOO=BAR");
  printf ("getenv(\"FOO\") ==> \"%s\"\n", getenv ("FOO"));

  puts( "*** END OF PUTENV/GETENV TEST ***" );
  rtems_test_exit(0);
}
