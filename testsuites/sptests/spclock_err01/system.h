/*
 *  This include file contains information that is included in every
 *  function in the test set.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
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

rtems_timer_service_routine Delayed_routine(
  rtems_id  ignored_id,
  void     *ignored_address
);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS              10
#define CONFIGURE_MAXIMUM_TIMERS              1
#define CONFIGURE_MAXIMUM_SEMAPHORES          2
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES      1
#define CONFIGURE_MAXIMUM_PERIODS             1
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS     0
#define CONFIGURE_TICKS_PER_TIMESLICE       100

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (20 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 11 ];       /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 11 ];     /* array of task names */

TEST_EXTERN rtems_name Semaphore_name[ 4 ]; /* array of semaphore names */
TEST_EXTERN rtems_id   Semaphore_id[ 4 ];   /* array of semaphore ids */

TEST_EXTERN rtems_name Queue_name[ 3 ];     /* array of queue names */
TEST_EXTERN rtems_id   Queue_id[ 3 ];       /* array of queue ids */

TEST_EXTERN rtems_name Port_name[ 2 ];      /* array of port names */
TEST_EXTERN rtems_id   Port_id[ 2 ];        /* array of port ids */

TEST_EXTERN rtems_name Period_name[ 2 ];    /* array of period names */
TEST_EXTERN rtems_id   Period_id[ 2 ];      /* array of period ids */

TEST_EXTERN rtems_id   Junk_id;             /* id used to return errors */

#define Internal_port_area (void *) 0x00001000
#define External_port_area (void *) 0x00002000

/* end of include file */
