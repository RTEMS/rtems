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

#include <tmacros.h>

/* macros */

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

rtems_task Task_1_through_3(
  rtems_task_argument argument
);

void blow_stack( void );

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define TASK_STACK_SIZE (RTEMS_MINIMUM_STACK_SIZE*3)

#define CONFIGURE_MAXIMUM_TASKS               4
#define CONFIGURE_EXTRA_TASK_STACKS (3 * (RTEMS_MINIMUM_STACK_SIZE * 2))

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_STACK_CHECKER_ENABLED

void Fatal_extension(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   error
);

#define CONFIGURE_INITIAL_EXTENSIONS \
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL, Fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#include <rtems/confdefs.h>

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 4 ];         /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 4 ];       /* array of task names */

/* end of include file */
