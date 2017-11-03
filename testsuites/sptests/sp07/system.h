/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-2009.
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

bool Task_create_extension(
  rtems_tcb *unused,
  rtems_tcb *created_task
);

void Task_delete_extension(
  rtems_tcb *running_task,
  rtems_tcb *deleted_task
);

void Task_restart_extension(
  rtems_tcb *unused,
  rtems_tcb *restarted_task
);

void Task_start_extension(
  rtems_tcb *unused,
  rtems_tcb *started_task
);

void Task_exit_extension(
  rtems_tcb *running_task
);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS     2
#define CONFIGURE_TICKS_PER_TIMESLICE       100

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (4 * RTEMS_MINIMUM_STACK_SIZE)
#define CONFIGURE_MAXIMUM_TASKS             5

#define CONFIGURE_DISABLE_SMP_CONFIGURATION

#include <rtems/confdefs.h>

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 5 ];         /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 5 ];       /* array of task names */

TEST_EXTERN rtems_id   Extension_id[ 4 ];
TEST_EXTERN rtems_name Extension_name[ 4 ];  /* array of task names */

TEST_EXTERN int Task_created[ RTEMS_ARRAY_SIZE( Task_id ) ];

TEST_EXTERN int Task_started[ RTEMS_ARRAY_SIZE( Task_id ) ];

TEST_EXTERN int Task_restarted[ RTEMS_ARRAY_SIZE( Task_id ) ];

TEST_EXTERN int Task_deleted[ RTEMS_ARRAY_SIZE( Task_id ) ];

RTEMS_INLINE_ROUTINE void assert_extension_counts( const int *table, int tasks )
{
  size_t i;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( Task_id ); ++i ) {
    rtems_test_assert(
      ( tasks & ( 1 << i ) ) != 0 ? table[ i ] == 1 : table[ i ] == 0
    );
  }
}

/* end of include file */
