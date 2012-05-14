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
 *  http://www.rtems.com/license/LICENSE.
 */

#include <tmacros.h>

/* types */

struct counters {
  uint32_t   count[6];
};

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

rtems_task Task_1_through_5(
  rtems_task_argument argument
);

void Get_all_counters( void );

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS               7
#define CONFIGURE_MAXIMUM_PERIODS             10

#define CONFIGURE_INIT_TASK_PRIORITY          10
#define CONFIGURE_INIT_TASK_INITIAL_MODES     RTEMS_DEFAULT_MODES
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_EXTRA_TASK_STACKS \
  (6 * (3 * RTEMS_MINIMUM_STACK_SIZE)) /* our tasks */ + \
  (1 * RTEMS_MINIMUM_STACK_SIZE)       /* monitor tasks */

#include <rtems/confdefs.h>

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 6 ];     /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 6 ];   /* array of task names */

/* end of include file */
