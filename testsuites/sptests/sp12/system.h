/* SPDX-License-Identifier: BSD-2-Clause */

/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-2008.
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

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INIT_TASK_STACK_SIZE      (RTEMS_MINIMUM_STACK_SIZE * 2)
#define CONFIGURE_MAXIMUM_TASKS               8
#define CONFIGURE_MAXIMUM_SEMAPHORES         10
#define CONFIGURE_TICKS_PER_TIMESLICE       100

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

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
