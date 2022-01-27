/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <tmacros.h>

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

rtems_task Task_1_through_3(
  rtems_task_argument argument
);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

/*
 * Avoid a dependency on errno which might be a thread-local object.  This test
 * assumes that no thread-local storage object is present.
 */
#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (3 * RTEMS_MINIMUM_STACK_SIZE)
#define CONFIGURE_MAXIMUM_TASKS             4
#define CONFIGURE_MINIMUM_TASKS_WITH_USER_PROVIDED_STORAGE 1

#include <rtems/confdefs.h>

/* global variables */

/* end of include file */
