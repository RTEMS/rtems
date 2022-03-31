/* SPDX-License-Identifier: BSD-2-Clause */

/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-1999.
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

rtems_task Tasks_Periodic(
  rtems_task_argument argument
);

rtems_task Tasks_Aperiodic(
  rtems_task_argument argument
);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 20000

#define CONFIGURE_MAXIMUM_TASKS               7
#define CONFIGURE_MAXIMUM_PERIODS             10

#define CONFIGURE_INIT_TASK_PRIORITY          100
#define CONFIGURE_INIT_TASK_INITIAL_MODES     RTEMS_NO_PREEMPT
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (6 * 4 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_SCHEDULER_CBS

#include <rtems/confdefs.h>

#include <rtems/rtems/clock.h>
#include <rtems/score/isr.h>
#include <rtems/rtems/intr.h>
#include <rtems/cbs.h>

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
TEST_EXTERN bool Violating_task[1 + NUM_PERIODIC_TASKS];

/* end of include file */
