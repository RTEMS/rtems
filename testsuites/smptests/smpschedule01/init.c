/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems.h>

#include "tmacros.h"

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
  uint32_t n = rtems_smp_get_processor_count();
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

      ok = ok && executing->cpu == cpu;
      ok = ok && heir->cpu == cpu;
    }

    ok = ok && (count == 1);
  }

  return ok;
}

static void test_scheduler_cross(void)
{
  bool per_cpu_state_ok;

  _Thread_Disable_dispatch();

  suspend(0);
  suspend(1);
  resume(0);
  resume(1);

  per_cpu_state_ok = is_per_cpu_state_ok();

  _Thread_Enable_dispatch();

  rtems_test_assert(per_cpu_state_ok);
}

static void test_scheduler_move_heir(void)
{
  bool per_cpu_state_ok;

  _Thread_Disable_dispatch();

  suspend(2);
  suspend(3);
  suspend(0);
  resume(2);
  suspend(1);
  resume(3);
  resume(0);

  per_cpu_state_ok = is_per_cpu_state_ok();

  resume(1);

  _Thread_Enable_dispatch();

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
  puts("\n\n*** TEST SMPSCHEDULE 1 ***");

  test();

  puts("*** END OF TEST SMPSCHEDULE 1 ***");

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_SMP_APPLICATION

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_MAXIMUM_TASKS TASK_COUNT

#define CONFIGURE_INIT_TASK_PRIORITY FIRST_TASK_PRIORITY
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
