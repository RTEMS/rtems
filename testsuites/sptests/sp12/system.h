/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <tmacros.h>

#define PRIORITY_INHERIT_BASE_PRIORITY 5

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

void Priority_test_driver(
  rtems_task_priority priority_base
);

rtems_task Priority_task(
  rtems_task_argument its_index
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

rtems_task Task_4(
  rtems_task_argument argument
);

rtems_task Task5(
  rtems_task_argument argument
);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INIT_TASK_STACK_SIZE      (RTEMS_MINIMUM_STACK_SIZE * 2)
#define CONFIGURE_MAXIMUM_TASKS               8
#define CONFIGURE_MAXIMUM_SEMAPHORES         10
#define CONFIGURE_TICKS_PER_TIMESLICE       100

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (6 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>

/* global variables */


TEST_EXTERN rtems_id   Task_id[ 6 ];             /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 6 ];           /* array of task names */

TEST_EXTERN rtems_id   Priority_task_id[ 6 ];    /* array of task ids */
TEST_EXTERN rtems_name Priority_task_name[ 6 ];  /* array of task names */

TEST_EXTERN rtems_task_priority Task_priority[ 6 ];

TEST_EXTERN rtems_id   Semaphore_id[ 4 ];        /* array of semaphore ids */
TEST_EXTERN rtems_name Semaphore_name[ 4 ];      /* array of semaphore names */

/* end of include file */
