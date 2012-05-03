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

rtems_task Tasks_Periodic(
  rtems_task_argument argument
);

rtems_task Tasks_Aperiodic(
  rtems_task_argument argument
);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 20000

#define CONFIGURE_MAXIMUM_TASKS               7
#define CONFIGURE_MAXIMUM_PERIODS             10

#define CONFIGURE_INIT_TASK_PRIORITY          100
#define CONFIGURE_INIT_TASK_INITIAL_MODES     RTEMS_NO_PREEMPT
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (6 * 4 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_SCHEDULER_EDF

#include <rtems/confdefs.h>

#include <rtems/rtems/clock.h>
#include <rtems/score/isr.h>
#include <rtems/rtems/intr.h>

#define  JOBS_PER_HP              (389)
#define  HP_LENGTH                (420)
#define  NUM_PERIODIC_TASKS       (4)
#define  NUM_APERIODIC_TASKS      (2)
#define  NUM_TASKS                ( NUM_PERIODIC_TASKS + NUM_APERIODIC_TASKS )

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 1+NUM_TASKS ];     /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 1+NUM_TASKS ];   /* array of task names */
TEST_EXTERN rtems_task_priority Priorities[ 1+NUM_TASKS ];
TEST_EXTERN uint32_t  Periods[ 1 + NUM_PERIODIC_TASKS ];
TEST_EXTERN uint32_t  Phases[1 + NUM_TASKS];
TEST_EXTERN uint32_t  Execution[1 + NUM_TASKS];

/* end of include file */
