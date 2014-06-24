/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems.h>
#include <rtems/libcsupport.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/threadimpl.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPMIGRATION 2";

#define CPU_COUNT 32

#define TASK_COUNT (CPU_COUNT + 1)

#define PRIO_LOW 3

#define PRIO_HIGH 2

typedef struct {
  uint32_t value;
  uint32_t cache_line_separation[31];
} test_counter;

typedef struct {
  test_counter counters[TASK_COUNT];
  rtems_id scheduler_ids[CPU_COUNT];
  rtems_id task_ids[TASK_COUNT];
} test_context;

static test_context test_instance;

static void migration_task(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  uint32_t cpu_count = rtems_get_processor_count();
  uint32_t cpu_index = rtems_get_current_processor();

  while (true) {
    cpu_index = (cpu_index + 1) % cpu_count;

    sc = rtems_task_set_scheduler(RTEMS_SELF, ctx->scheduler_ids[cpu_index]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    ++ctx->counters[arg].value;

    rtems_test_assert(cpu_index == rtems_get_current_processor());
  }
}

static void test_migrations(test_context *ctx)
{
  rtems_status_code sc;
  uint32_t cpu_count = rtems_get_processor_count();
  uint32_t task_count = cpu_count + 1;
  uint32_t task_index;

  for (task_index = 0; task_index < task_count; ++task_index) {
    rtems_id task_id;

    sc = rtems_task_create(
      rtems_build_name('T', 'A', 'S', 'K'),
      task_index > 0 ? PRIO_LOW : PRIO_HIGH,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &task_id
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_set_scheduler(task_id, ctx->scheduler_ids[task_index % cpu_count]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_start(task_id, migration_task, task_index);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    ctx->task_ids[task_index] = task_id;
  }

  sc = rtems_task_wake_after(30 * rtems_clock_get_ticks_per_second());
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (task_index = 0; task_index < task_count; ++task_index) {
    printf(
      "task %" PRIu32 " counter: %" PRIu32 "\n",
      task_index,
      ctx->counters[task_index].value
    );

    sc = rtems_task_delete(ctx->task_ids[task_index]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void busy_loop_task(rtems_task_argument arg)
{
  while (true) {
    /* Do nothing */
  }
}

static Thread_Control *get_thread_by_id(rtems_id task_id)
{
  Objects_Locations location;
  Thread_Control *thread;

  thread = _Thread_Get(task_id, &location);
  rtems_test_assert(location == OBJECTS_LOCAL);
  _Thread_Enable_dispatch();

  return thread;
}

static void test_double_migration(test_context *ctx)
{
  uint32_t cpu_count = rtems_get_processor_count();

  if (cpu_count >= 2) {
    rtems_status_code sc;
    rtems_id task_id;
    rtems_id scheduler_id;
    uint32_t cpu_self_index = 0;
    uint32_t cpu_other_index = 1;
    Per_CPU_Control *cpu_self = _Per_CPU_Get_by_index(cpu_self_index);
    Per_CPU_Control *cpu_other = _Per_CPU_Get_by_index(cpu_other_index);
    Thread_Control *self;
    Thread_Control *other;

    sc = rtems_task_get_scheduler(RTEMS_SELF, &scheduler_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(scheduler_id == ctx->scheduler_ids[cpu_self_index]);

    sc = rtems_task_create(
      rtems_build_name('T', 'A', 'S', 'K'),
      2,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &task_id
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    other = get_thread_by_id(task_id);

    sc = rtems_task_set_scheduler(
      task_id,
      ctx->scheduler_ids[cpu_other_index]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_start(task_id, busy_loop_task, 0);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    while (!_Thread_Is_executing_on_a_processor(other)) {
      /* Wait */
    }

    _Thread_Disable_dispatch();

    self = _Thread_Executing;

    rtems_test_assert(cpu_self->executing == self);
    rtems_test_assert(cpu_self->heir == self);
    rtems_test_assert(!cpu_self->dispatch_necessary);

    rtems_test_assert(cpu_other->executing == other);
    rtems_test_assert(cpu_other->heir == other);
    rtems_test_assert(!cpu_other->dispatch_necessary);

    sc = rtems_task_set_scheduler(
      RTEMS_SELF,
      ctx->scheduler_ids[cpu_other_index]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(cpu_self->executing == self);
    rtems_test_assert(cpu_self->heir != self);
    rtems_test_assert(cpu_self->dispatch_necessary);

    while (_Thread_Is_executing_on_a_processor(other)) {
      /* Wait */
    }

    rtems_test_assert(cpu_other->executing == self);
    rtems_test_assert(cpu_other->heir == self);
    rtems_test_assert(!cpu_other->dispatch_necessary);

    sc = rtems_task_set_scheduler(
      RTEMS_SELF,
      ctx->scheduler_ids[cpu_self_index]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(cpu_self->executing == self);
    rtems_test_assert(cpu_self->heir == self);
    rtems_test_assert(cpu_self->dispatch_necessary);

    rtems_test_assert(cpu_other->heir == other);

    _Thread_Enable_dispatch();

    while (!_Thread_Is_executing_on_a_processor(other)) {
      /* Wait */
    }

    sc = rtems_task_delete(task_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void init_scheduler_ids(test_context *ctx)
{
  rtems_status_code sc;
  uint32_t cpu_count = rtems_get_processor_count();
  uint32_t cpu_index;

  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    sc = rtems_scheduler_ident(cpu_index, &ctx->scheduler_ids[cpu_index]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void Init(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  rtems_resource_snapshot snapshot;

  TEST_BEGIN();

  rtems_resource_snapshot_take(&snapshot);

  init_scheduler_ids(ctx);
  test_double_migration(ctx);
  test_migrations(ctx);

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_SMP_APPLICATION

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_SCHEDULER_SIMPLE_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(0);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(1);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(2);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(3);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(4);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(5);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(6);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(7);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(8);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(9);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(10);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(11);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(12);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(13);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(14);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(15);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(16);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(17);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(18);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(19);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(20);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(21);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(22);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(23);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(24);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(25);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(26);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(27);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(28);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(29);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(30);
RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(31);

#define CONFIGURE_SCHEDULER_CONTROLS \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(0, 0), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(1, 1), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(2, 2), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(3, 3), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(4, 4), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(5, 5), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(6, 6), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(7, 7), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(8, 8), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(9, 9), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(10, 10), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(11, 11), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(12, 12), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(13, 13), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(14, 14), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(15, 15), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(16, 16), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(17, 17), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(18, 18), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(19, 19), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(20, 20), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(21, 21), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(22, 22), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(23, 23), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(24, 24), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(25, 25), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(26, 26), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(27, 27), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(28, 28), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(29, 29), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(30, 30), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(31, 31)

#define CONFIGURE_SMP_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(2, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(3, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(4, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(5, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(6, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(7, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(8, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(9, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(10, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(11, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(12, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(13, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(14, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(15, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(16, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(17, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(18, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(19, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(20, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(21, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(22, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(23, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(24, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(25, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(26, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(27, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(28, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(29, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(30, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(31, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL)

#define CONFIGURE_MAXIMUM_TASKS (1 + TASK_COUNT)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
