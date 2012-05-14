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

rtems_timer_service_routine TA1_send_18_to_self_5_seconds(
  rtems_id  ignored_id,
  void     *ignored_address
);

rtems_timer_service_routine TA1_send_8_to_self_60_seconds(
  rtems_id  ignored_id,
  void     *ignored_address
);

rtems_timer_service_routine TA1_send_9_to_self_60_seconds(
  rtems_id  ignored_id,
  void     *ignored_address
);

rtems_timer_service_routine TA1_send_10_to_self(
  rtems_id  ignored_id,
  void     *ignored_address
);

rtems_timer_service_routine TA1_send_1_to_self_every_second(
  rtems_id  ignored_id,
  void     *ignored_address
);

rtems_timer_service_routine TA1_send_11_to_self(
  rtems_id  ignored_id,
  void     *ignored_address
);

rtems_timer_service_routine TA2_send_10_to_self(
  rtems_id  ignored_id,
  void     *ignored_address
);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS               3
#define CONFIGURE_MAXIMUM_TIMERS              6
#define CONFIGURE_TICKS_PER_TIMESLICE       100

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (2 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 4 ];         /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 4 ];       /* array of task names */

TEST_EXTERN rtems_id   Timer_id[ 7 ];     /* array of timer ids */
TEST_EXTERN rtems_name Timer_name[ 7 ];   /* array of timer names */

/* end of include file */
