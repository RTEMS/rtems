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

rtems_task Init(
  rtems_task_argument argument
);

rtems_task Task_1(
  rtems_task_argument argument
);

rtems_task Task_2(
  rtems_task_argument argument
);

rtems_task Task_3(
  rtems_task_argument argument
);

void add_some(
  uint32_t  per_loop,
  uint32_t *sum,
  uint32_t *next
);

/* configuration information */
#define TASK_COUNT           30

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS     1
#define CONFIGURE_MAXIMUM_TASKS               (TASK_COUNT+2)
#define CONFIGURE_TICKS_PER_TIMESLICE         100

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>

/* global variables */

TEST_EXTERN rtems_id         Task_id[ TASK_COUNT ];         /* array of task ids */
TEST_EXTERN rtems_name       Task_name[ TASK_COUNT ];       /* array of task names */
TEST_EXTERN rtems_interval   TicksPerSecond;

/* end of include file */
