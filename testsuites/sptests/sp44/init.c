/*
 *  Original version submitted as part of PR1212
 *
 *  This example shows a possible blocking of timeslicing if task mode is
 *  changed to timeslicing in the middle of its executing.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <rtems.h>
#include <stdio.h>
#include <stdlib.h>

rtems_task Init(rtems_task_argument ignored);
rtems_task TaskAB_entry(rtems_task_argument me);

/*** Task priorities ***/
#define TASK_A_PRIORITY   10
#define TASK_B_PRIORITY   10

/*** Task names ***/
#define TASK_A_NAME     1
#define TASK_B_NAME     2

/*** Task atributes ***/
#define TASK_A_INITMODE    RTEMS_DEFAULT_MODES
#define TASK_B_INITMODE    RTEMS_DEFAULT_MODES

/*** Task generic parameters ***/
#define TASK_A_STACKSIZE   RTEMS_MINIMUM_STACK_SIZE
#define TASK_A_MODEATTR    RTEMS_DEFAULT_ATTRIBUTES
#define TASK_B_STACKSIZE   RTEMS_MINIMUM_STACK_SIZE
#define TASK_B_MODEATTR    RTEMS_DEFAULT_ATTRIBUTES

volatile uint32_t turn;
rtems_id TaskA_id, TaskB_id;

/* TASK A/B */
rtems_task TaskAB_entry(rtems_task_argument me)
{
  static rtems_mode previous_mode_set;
  rtems_status_code status;
  uint32_t iterations = 0;

  status = rtems_task_mode(
    RTEMS_PREEMPT | RTEMS_TIMESLICE,
    RTEMS_PREEMPT_MASK | RTEMS_TIMESLICE_MASK,
    &previous_mode_set
  );
  directive_failed(status, "Unable to change task mode.");

  while(1) {
    if (turn == me) {
      printf(
        "Task #%" PRIdrtems_task_argument "'s turn. Now setting turn to %"
          PRIdrtems_task_argument "\n",
          me,
          1 - me
      );
      turn = 1 - me;

      if ( ++iterations == 10 ) {
        puts( "*** END OF SP44 TEST ***" );
        exit( 0 );
      }
    }
  }
}

rtems_task Init(rtems_task_argument ignored)
{
  static rtems_status_code status;

  puts( "\n\n*** SP44 TEST ***" );

  /* Create Task A */
  status = rtems_task_create(
    TASK_A_NAME,
    TASK_A_PRIORITY,
    TASK_A_STACKSIZE,
    TASK_A_INITMODE,
    TASK_A_MODEATTR,
    &TaskA_id
  );
  directive_failed(status,"rtems_task_create");

  /* Start Task A */
  status = rtems_task_start(TaskA_id, TaskAB_entry, 0);
  directive_failed(status,"rtems_task_start");

  /* Create Task B */
  status = rtems_task_create(
    TASK_B_NAME,
    TASK_B_PRIORITY,
    TASK_B_STACKSIZE,
    TASK_B_INITMODE,
    TASK_B_MODEATTR,
    &TaskB_id
  );
  directive_failed( status, "rtems_task_create" );

  /* Start Task B */
  status = rtems_task_start(TaskB_id, TaskAB_entry, 1);
  directive_failed( status, "rtems_task_start" );

  /* Suspend itself */
  status = rtems_task_suspend(RTEMS_SELF);
  directive_failed( status, "rtems_task_suspend" );

  /* This task is not suposed to be executed anymore */
  printf("\nNOT SUPOSED TO RETURN HERE...\n");
  rtems_test_exit(1);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000
#define CONFIGURE_TICKS_PER_TIMESLICE   10
#define CONFIGURE_MAXIMUM_TASKS         3

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
