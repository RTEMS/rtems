/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2010 embedded brains GmbH & Co. KG
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
#include <inttypes.h>

#include <rtems.h>

#include "tmacros.h"

const char rtems_test_name[] = "PSX 15";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

/*
 * This test case shows that post switch extension handlers must cope with
 * already deleted resources (e.g. _POSIX_signals_Post_switch_extension()).
 * The thread delete extensions run with thread dispatching enabled.  Only the
 * allocation mutex is locked.
 */

static rtems_id task_0 = RTEMS_ID_NONE;

static rtems_id task_1 = RTEMS_ID_NONE;

static void thread_delete_hook(
  Thread_Control *executing,
  Thread_Control *deleted
)
{
  (void) executing;

  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (deleted->Object.id == task_0) {
    rtems_task_priority old = 0;

    sc = rtems_task_set_priority(task_1, 2, &old);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void suicide_task(rtems_task_argument arg)
{
  int me = (int) arg;

  printf("suicide task %d\n", me);

  rtems_task_exit();
  rtems_test_assert(false);
}

void Init(rtems_task_argument arg)
{
  (void) arg;

  rtems_status_code sc = RTEMS_SUCCESSFUL;

  TEST_BEGIN();

  sc = rtems_task_create(
    rtems_build_name('T', 'S', 'K', '1'),
    5,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_1
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(task_1, suicide_task, 1);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('T', 'S', 'K', '0'),
    3,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_0
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(task_0, suicide_task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  TEST_END();

  rtems_test_exit(0);
  rtems_test_assert(false);
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 3
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .thread_delete = thread_delete_hook }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_PREEMPT
#define CONFIGURE_INIT_TASK_PRIORITY 4

#include <rtems/confdefs.h>
