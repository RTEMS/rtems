/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2011.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <rtems.h>
#include <rtems/error.h>

static rtems_id taskId1;
static rtems_id taskId2;
rtems_interval ticksPerSecond;

#include "system.h"

static int
isSuspended (rtems_id tid)
{
  rtems_status_code sc;

  sc = rtems_task_is_suspended (tid);
  if (sc == RTEMS_ALREADY_SUSPENDED)
    return 1;
  if (sc != RTEMS_SUCCESSFUL)
    printf ("rtems_task_is_suspended: %s\n", rtems_status_text (sc));
  return 0;
}

static void
subTask1 (rtems_task_argument arg)
{
  rtems_status_code sc;

  rtems_task_wake_after (ticksPerSecond * 3);
  sc = rtems_event_send (taskId2, 1);
  if (sc != RTEMS_SUCCESSFUL) {
    printf ("subTask1 - Can't send event (%d)\n", sc);
    rtems_task_suspend (RTEMS_SELF);
  }
  rtems_task_wake_after (ticksPerSecond * 3);
  printf ("subTask1 - Event sent\n");
  rtems_task_suspend (RTEMS_SELF);
  printf ("subTask1 - Back to task 1\n");
  rtems_task_wake_after (ticksPerSecond * 3);
  rtems_task_suspend (RTEMS_SELF);
}

static void
subTask2 (rtems_task_argument arg)
{
  rtems_status_code sc;
  rtems_event_set ev;

  rtems_task_wake_after (ticksPerSecond * 1);
  sc = rtems_event_receive(
    1,
    RTEMS_WAIT|RTEMS_EVENT_ANY,
    RTEMS_NO_TIMEOUT,
    &ev
  );
  if (sc != RTEMS_SUCCESSFUL) {
    printf ("subTask2 - Can't receive event (%d)\n", sc);
    rtems_task_suspend (RTEMS_SELF);
  }
  printf ("subTask2 - Task 1 suspended? - should be 0: %d\n",
     isSuspended (taskId1));
  rtems_task_wake_after (ticksPerSecond * 4);
  printf ("subTask2 - Task 1 suspended? - should be 1: %d\n",
     isSuspended (taskId1));
  rtems_task_resume (taskId1);
  printf ("subTask2 - Task 1 suspended? - should be 0: %d\n",
     isSuspended (taskId1));
  rtems_task_wake_after (ticksPerSecond * 4);
  printf ("subTask2 - Task 1 suspended? - should be 1: %d\n",
     isSuspended (taskId1));

  TEST_END();
  rtems_test_exit( 0 );
}

static void
createTask (char c, rtems_id *tid)
{
  rtems_status_code sc;

  sc = rtems_task_create (rtems_build_name('S','u','b',c),
    RTEMS_MAXIMUM_PRIORITY - 1u,
    RTEMS_MINIMUM_STACK_SIZE * 4,
    RTEMS_PREEMPT|RTEMS_NO_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
    RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
    tid);
  if (sc != RTEMS_SUCCESSFUL) {
    printf ("Can't create task (%d)\n", sc);
    rtems_task_suspend (RTEMS_SELF);
  }
}

static void
startTask (rtems_id tid, rtems_task_entry entry_point)
{
  rtems_status_code sc;

  sc = rtems_task_start (tid, entry_point, 0);
  if (sc != RTEMS_SUCCESSFUL) {
    printf ("Can't start task (%d)\n", sc);
    rtems_task_suspend (RTEMS_SELF);
  }
}

void
task1 (void)
{
  ticksPerSecond = rtems_clock_get_ticks_per_second();
  createTask ('1', &taskId1);
  createTask ('2', &taskId2);
  startTask (taskId1, subTask1);
  startTask (taskId2, subTask2);
  rtems_task_suspend (RTEMS_SELF);
}
