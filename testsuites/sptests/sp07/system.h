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
 *
 *  $Id$
 */

#include <tmacros.h>

/* buffered IO */

void buffered_io_initialize( void );

void buffered_io_flush(void);

void buffered_io_add_string( char *s );

/* end of buffered IO */
 
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

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS     1
#define CONFIGURE_TICKS_PER_TIMESLICE       100

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (4 * RTEMS_MINIMUM_STACK_SIZE)
#define CONFIGURE_MAXIMUM_TASKS             5

#include <confdefs.h>

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 5 ];         /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 5 ];       /* array of task names */

TEST_EXTERN rtems_id   Extension_id[ 4 ];
TEST_EXTERN rtems_name Extension_name[ 4 ];  /* array of task names */
 
/* end of include file */
