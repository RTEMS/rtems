/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/test.h>

/* functions */

rtems_task main_task(
  rtems_task_argument argument
);

/* configuration information */

#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS           1
#define CONFIGURE_MAXIMUM_SEMAPHORES      5

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT_TASK_ENTRY_POINT   main_task
#define CONFIGURE_INIT_TASK_NAME          rtems_build_name( 'C', 'T', 'O', 'R' )
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

/* On some platforms _Unwind_RaiseException() needs a lot of stack space */
#define CONFIGURE_INIT_TASK_STACK_SIZE (4 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#include <rtems/confdefs.h>

/* end of include file */
