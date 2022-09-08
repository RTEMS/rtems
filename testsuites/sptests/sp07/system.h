/* SPDX-License-Identifier: BSD-2-Clause */

/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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

static inline void assert_extension_counts( const int *table, int tasks )
{
  size_t i;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( Task_id ); ++i ) {
    rtems_test_assert(
      ( tasks & ( 1 << i ) ) != 0 ? table[ i ] == 1 : table[ i ] == 0
    );
  }
}

/* end of include file */
