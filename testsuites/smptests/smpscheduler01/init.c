/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2013 embedded brains GmbH & Co. KG
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

#include <rtems.h>
#include <rtems/score/threadimpl.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPSCHEDULER 1";

#define CPU_COUNT 2

#define TASK_COUNT 4

#define FIRST_TASK_PRIORITY 1

#define SECOND_TASK_READY RTEMS_EVENT_0

static rtems_id task_ids[TASK_COUNT];

static void suspend(size_t i)
{
  rtems_status_code sc = rtems_task_suspend(task_ids[i]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void resume(size_t i)
{
  rtems_status_code sc = rtems_task_resume(task_ids[i]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void task(rtems_task_argument arg)
{
  (void) arg;

  rtems_task_priority task_priority;
  rtems_status_code sc;

  sc = rtems_task_set_priority(
    RTEMS_SELF,
    RTEMS_CURRENT_PRIORITY,
    &task_priority
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  if (arg == 1) {
    sc = rtems_event_send(task_ids[0], SECOND_TASK_READY);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  while (true) {
    /* Do nothing */
  }
}

static bool is_per_cpu_state_ok(void)
{
  bool ok = true;
  uint32_t n = rtems_scheduler_get_processor_maximum();
  uint32_t i;

  for (i = 0; i < n; ++i) {
    const Thread_Control *thread = _Per_CPU_Get_by_index(i)->executing;
    uint32_t count = 0;
    uint32_t j;

    for (j = 0; j < n; ++j) {
      const Per_CPU_Control *cpu = _Per_CPU_Get_by_index(j);
      const Thread_Control *executing = cpu->executing;
      const Thread_Control *heir = cpu->heir;

      if (i != j) {
        count += executing == thread;
        count += heir == thread;
      } else {
        ++count;
      }

      ok = ok && _Thread_Get_CPU( executing ) == cpu;
      ok = ok && _Thread_Get_CPU( heir ) == cpu;
    }

    ok = ok && (count == 1);
  }

  return ok;
}

static void test_scheduler_cross(void)
{
  bool per_cpu_state_ok;
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();

  suspend(0);
  suspend(1);
  resume(0);
  resume(1);

  per_cpu_state_ok = is_per_cpu_state_ok();

  _Thread_Dispatch_enable( cpu_self );

  rtems_test_assert(per_cpu_state_ok);
}

static void test_scheduler_move_heir(void)
{
  bool per_cpu_state_ok;
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();

  suspend(2);
  suspend(3);
  suspend(0);
  resume(2);
  suspend(1);
  resume(3);
  resume(0);

  per_cpu_state_ok = is_per_cpu_state_ok();

  resume(1);

  _Thread_Dispatch_enable( cpu_self );

  rtems_test_assert(per_cpu_state_ok);
}

static void test(void)
{
  rtems_event_set events;
  rtems_status_code sc;
  rtems_task_argument task_index;

  task_ids[0] = rtems_task_self();

  for (task_index = 1; task_index < TASK_COUNT; ++task_index) {
      rtems_id task_id;

      sc = rtems_task_create(
        rtems_build_name('T', 'A', 'S', 'K'),
        FIRST_TASK_PRIORITY + task_index,
        RTEMS_MINIMUM_STACK_SIZE,
        RTEMS_DEFAULT_MODES,
        RTEMS_DEFAULT_ATTRIBUTES,
        &task_id
      );
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      sc = rtems_task_start(task_id, task, task_index);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      task_ids[task_index] = task_id;
  }

  sc = rtems_event_receive(
    SECOND_TASK_READY,
    RTEMS_EVENT_ALL | RTEMS_WAIT,
    RTEMS_NO_TIMEOUT,
    &events
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(events == SECOND_TASK_READY);

  test_scheduler_cross();
  test_scheduler_move_heir();
}

static void Init(rtems_task_argument arg)
{
  (void) arg;

  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_MAXIMUM_TASKS TASK_COUNT

/* We need a scheduler with lazy processor allocation for this test */
#define CONFIGURE_SCHEDULER_SIMPLE_SMP

#define CONFIGURE_INIT_TASK_PRIORITY FIRST_TASK_PRIORITY
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
