/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is a user initialization task and has the responsibility
 *  of invoking the test routine
 *
 *  Input parameters:
 *    not_used
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

/* Includes */
#include <bsp.h>
#include <tmacros.h>

void test_main(void);

rtems_task Init(
  rtems_task_argument not_used
)
{
  test_main();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 6

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_PIPES_ENABLED
#define CONFIGURE_INIT
#define CONFIGURE_MAXIMUM_BARRIERS               2

#include <rtems/confdefs.h>

/* end of file */
