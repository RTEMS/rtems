/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
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
 
rtems_task Task_3(
  rtems_task_argument argument
);
 
rtems_task Task_4(
  rtems_task_argument argument
);
 
rtems_boolean Task_create_extension(
  rtems_tcb *unused,
  rtems_tcb *created_task
);
 
rtems_extension Task_delete_extension(
  rtems_tcb *running_task,
  rtems_tcb *deleted_task
);
 
rtems_extension Task_restart_extension(
  rtems_tcb *unused,
  rtems_tcb *restarted_task
);
 
rtems_extension Task_start_extension(
  rtems_tcb *unused,
  rtems_tcb *started_task
);
 
rtems_extension Task_exit_extension(
  rtems_tcb *running_task
);

/* configuration information */

#define CONFIGURE_SPTEST

#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS     1
#define CONFIGURE_TICKS_PER_TIMESLICE       100

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <confdefs.h>

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 5 ];         /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 5 ];       /* array of task names */

TEST_EXTERN rtems_id   Extension_id[ 4 ];
TEST_EXTERN rtems_name Extension_name[ 4 ];  /* array of task names */
 
/* end of include file */
