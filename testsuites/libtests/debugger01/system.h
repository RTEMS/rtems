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

#include <bsp.h>
#include <tmacros.h>

/* functions */

rtems_task Init(rtems_task_argument argument);

int rtems_debugger_register_test_remote(void);
void test_wake(void);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS     1
#define CONFIGURE_MAXIMUM_TASKS               10
#define CONFIGURE_MAXIMUM_SEMAPHORES          10

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>

/* end of include file */
