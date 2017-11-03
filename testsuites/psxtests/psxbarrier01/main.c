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

#include <rtems/test.h>
#include <bsp.h>
#include <pmacros.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

void test_main( void );

rtems_task Init(
  rtems_task_argument ignored
)
{
  test_main();
  rtems_test_exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS 10

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT_TASK_STACK_SIZE    (RTEMS_MINIMUM_STACK_SIZE * 2)

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/* end of file */
