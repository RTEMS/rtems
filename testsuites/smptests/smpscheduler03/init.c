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

#include <stdio.h>
#include <inttypes.h>

#include <rtems.h>
#include <rtems/libcsupport.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulersmpimpl.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPSCHEDULER 3";

#define CPU_MAX 3

#define SCHED_NAME(i) rtems_build_name(' ', ' ', ' ', (char) ('A' + (i)))

typedef struct {
  rtems_id barrier_id;
  rtems_id task_id[CPU_MAX];
  uint32_t cpu_index[CPU_MAX];
} test_context;

static test_context test_instance;

static void barrier_wait(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_barrier_wait(ctx->barrier_id, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void task(rtems_task_argument arg)
{
  rtems_test_assert(0);
}

static rtems_id start_task(rtems_task_priority prio)
{
  rtems_status_code sc;
  rtems_id task_id;

  sc = rtems_task_create(
    rtems_build_name('T', 'A', 'S', 'K'),
    prio,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(task_id, task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  return task_id;
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

static void test_case_change_priority(
  Thread_Control *executing,
  Scheduler_SMP_Node *node,
  Scheduler_SMP_Node_state start_state,
  Priority_Control prio,
  bool prepend_it,
  Scheduler_SMP_Node_state new_state
)
{
  switch (start_state) {
    case SCHEDULER_SMP_NODE_SCHEDULED:
      _Thread_Change_priority(executing, 1, true);
      break;
    case SCHEDULER_SMP_NODE_READY:
      _Thread_Change_priority(executing, 4, true);
      break;
    default:
      rtems_test_assert(0);
      break;
  }
  rtems_test_assert(node->state == start_state);

  _Thread_Change_priority(executing, prio, prepend_it);
  rtems_test_assert(node->state == new_state);
}

static const Scheduler_SMP_Node_state states[2] = {
  SCHEDULER_SMP_NODE_SCHEDULED,
  SCHEDULER_SMP_NODE_READY
};

static const Priority_Control priorities[2] = { 2, 5 };

static const bool prepend_it[2] = { true, false };

static void test_change_priority(void)
{
  rtems_status_code sc;
  rtems_id task_id;
  Thread_Control *executing;
  Scheduler_SMP_Node *node;
  size_t i;
  size_t j;
  size_t k;

  task_id = start_task(3);

  _Thread_Disable_dispatch();

  executing = _Thread_Executing;
  node = _Scheduler_SMP_Thread_get_node( executing );

  for (i = 0; i < RTEMS_ARRAY_SIZE(states); ++i) {
    for (j = 0; j < RTEMS_ARRAY_SIZE(priorities); ++j) {
      for (k = 0; k < RTEMS_ARRAY_SIZE(prepend_it); ++k) {
        test_case_change_priority(
          executing,
          node,
          states[i],
          priorities[j],
          prepend_it[k],
          states[j]
        );
      }
    }
  }

  _Thread_Change_priority(executing, 1, true);
  rtems_test_assert(node->state == SCHEDULER_SMP_NODE_SCHEDULED);

  _Thread_Enable_dispatch();

  sc = rtems_task_delete(task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static Thread_Control *change_priority_op(
  Thread_Control *thread,
  Priority_Control new_priority,
  bool prepend_it
)
{
  const Scheduler_Control *scheduler = _Scheduler_Get(thread);
  Thread_Control *needs_help;
  ISR_Level level;

  _ISR_Disable( level );
  thread->current_priority = new_priority;
  needs_help = (*scheduler->Operations.change_priority)(
    scheduler,
    thread,
    new_priority,
    prepend_it
  );
  _ISR_Enable( level );

  return needs_help;
}

static void test_case_change_priority_op(
  Thread_Control *executing,
  Scheduler_SMP_Node *executing_node,
  Thread_Control *other,
  Scheduler_SMP_Node_state start_state,
  Priority_Control prio,
  bool prepend_it,
  Scheduler_SMP_Node_state new_state
)
{
  Thread_Control *needs_help;

  switch (start_state) {
    case SCHEDULER_SMP_NODE_SCHEDULED:
      _Thread_Change_priority(executing, 1, true);
      break;
    case SCHEDULER_SMP_NODE_READY:
      _Thread_Change_priority(executing, 4, true);
      break;
    default:
      rtems_test_assert(0);
      break;
  }
  rtems_test_assert(executing_node->state == start_state);

  needs_help = change_priority_op(executing, prio, prepend_it);
  rtems_test_assert(executing_node->state == new_state);

  if (start_state != new_state) {
    switch (start_state) {
      case SCHEDULER_SMP_NODE_SCHEDULED:
        rtems_test_assert(needs_help == executing);
        break;
      case SCHEDULER_SMP_NODE_READY:
        rtems_test_assert(needs_help == other);
        break;
      default:
        rtems_test_assert(0);
        break;
    }
  } else {
    rtems_test_assert(needs_help == NULL);
  }
}

static void test_change_priority_op(void)
{
  rtems_status_code sc;
  rtems_id task_id;
  Thread_Control *executing;
  Scheduler_SMP_Node *executing_node;
  Thread_Control *other;
  size_t i;
  size_t j;
  size_t k;

  task_id = start_task(3);

  _Thread_Disable_dispatch();

  executing = _Thread_Executing;
  executing_node = _Scheduler_SMP_Thread_get_node(executing);

  other = get_thread_by_id(task_id);

  for (i = 0; i < RTEMS_ARRAY_SIZE(states); ++i) {
    for (j = 0; j < RTEMS_ARRAY_SIZE(priorities); ++j) {
      for (k = 0; k < RTEMS_ARRAY_SIZE(prepend_it); ++k) {
        test_case_change_priority_op(
          executing,
          executing_node,
          other,
          states[i],
          priorities[j],
          prepend_it[k],
          states[j]
        );
      }
    }
  }

  _Thread_Change_priority(executing, 1, true);
  rtems_test_assert(executing_node->state == SCHEDULER_SMP_NODE_SCHEDULED);

  _Thread_Enable_dispatch();

  sc = rtems_task_delete(task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static Thread_Control *yield_op(Thread_Control *thread)
{
  const Scheduler_Control *scheduler = _Scheduler_Get(thread);
  Thread_Control *needs_help;
  ISR_Level level;

  _ISR_Disable( level );
  needs_help = (*scheduler->Operations.yield)(scheduler, thread);
  _ISR_Enable( level );

  return needs_help;
}

static void test_case_yield_op(
  Thread_Control *executing,
  Scheduler_SMP_Node *executing_node,
  Thread_Control *other,
  Scheduler_SMP_Node_state start_state,
  Scheduler_SMP_Node_state new_state
)
{
  Thread_Control *needs_help;

  _Thread_Change_priority(executing, 4, false);
  _Thread_Change_priority(other, 4, false);

  switch (start_state) {
    case SCHEDULER_SMP_NODE_SCHEDULED:
      switch (new_state) {
        case SCHEDULER_SMP_NODE_SCHEDULED:
          _Thread_Change_priority(executing, 2, false);
          _Thread_Change_priority(other, 3, false);
          break;
        case SCHEDULER_SMP_NODE_READY:
          _Thread_Change_priority(executing, 2, false);
          _Thread_Change_priority(other, 2, false);
          break;
        default:
          rtems_test_assert(0);
          break;
      }
      break;
    case SCHEDULER_SMP_NODE_READY:
      switch (new_state) {
        case SCHEDULER_SMP_NODE_SCHEDULED:
          rtems_test_assert(0);
          break;
        case SCHEDULER_SMP_NODE_READY:
          _Thread_Change_priority(executing, 3, false);
          _Thread_Change_priority(other, 2, false);
          break;
        default:
          rtems_test_assert(0);
          break;
      }
      break;
    default:
      rtems_test_assert(0);
      break;
  }
  rtems_test_assert(executing_node->state == start_state);

  needs_help = yield_op(executing);
  rtems_test_assert(executing_node->state == new_state);

  if (start_state != new_state) {
    switch (start_state) {
      case SCHEDULER_SMP_NODE_SCHEDULED:
        rtems_test_assert(needs_help == executing);
        break;
      case SCHEDULER_SMP_NODE_READY:
        rtems_test_assert(needs_help == other);
        break;
      default:
        rtems_test_assert(0);
        break;
    }
  } else {
    rtems_test_assert(needs_help == NULL);
  }
}

static void test_yield_op(void)
{
  rtems_status_code sc;
  rtems_id task_id;
  Thread_Control *executing;
  Scheduler_SMP_Node *executing_node;
  Thread_Control *other;
  size_t i;
  size_t j;

  task_id = start_task(2);

  _Thread_Disable_dispatch();

  executing = _Thread_Executing;
  executing_node = _Scheduler_SMP_Thread_get_node(executing);

  other = get_thread_by_id(task_id);

  for (i = 0; i < RTEMS_ARRAY_SIZE(states); ++i) {
    for (j = 0; j < RTEMS_ARRAY_SIZE(states); ++j) {
      if (
        states[i] != SCHEDULER_SMP_NODE_READY
          || states[j] != SCHEDULER_SMP_NODE_SCHEDULED
      ) {
        test_case_yield_op(
          executing,
          executing_node,
          other,
          states[i],
          states[j]
        );
      }
    }
  }

  _Thread_Change_priority(executing, 1, true);
  rtems_test_assert(executing_node->state == SCHEDULER_SMP_NODE_SCHEDULED);

  _Thread_Enable_dispatch();

  sc = rtems_task_delete(task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void block_op(Thread_Control *thread)
{
  const Scheduler_Control *scheduler = _Scheduler_Get(thread);
  ISR_Level level;

  _ISR_Disable( level );
  (*scheduler->Operations.block)(scheduler, thread);
  _ISR_Enable( level );
}

static Thread_Control *unblock_op(Thread_Control *thread)
{
  const Scheduler_Control *scheduler = _Scheduler_Get(thread);
  Thread_Control *needs_help;
  ISR_Level level;

  _ISR_Disable( level );
  needs_help = (*scheduler->Operations.unblock)(scheduler, thread);
  _ISR_Enable( level );

  return needs_help;
}

static void test_case_unblock_op(
  Thread_Control *executing,
  Scheduler_SMP_Node *executing_node,
  Thread_Control *other,
  Scheduler_SMP_Node_state new_state
)
{
  Thread_Control *needs_help;

  switch (new_state) {
    case SCHEDULER_SMP_NODE_SCHEDULED:
      _Thread_Change_priority(executing, 2, false);
      rtems_test_assert(executing_node->state == SCHEDULER_SMP_NODE_SCHEDULED);
      break;
    case SCHEDULER_SMP_NODE_READY:
      _Thread_Change_priority(executing, 4, false);
      rtems_test_assert(executing_node->state == SCHEDULER_SMP_NODE_READY);
      break;
    default:
      rtems_test_assert(0);
      break;
  }

  block_op(executing);
  rtems_test_assert(executing_node->state == SCHEDULER_SMP_NODE_BLOCKED);

  needs_help = unblock_op(executing);
  rtems_test_assert(executing_node->state == new_state);

  switch (new_state) {
    case SCHEDULER_SMP_NODE_SCHEDULED:
      rtems_test_assert(needs_help == other);
      break;
    case SCHEDULER_SMP_NODE_READY:
      rtems_test_assert(needs_help == executing);
      break;
    default:
      rtems_test_assert(0);
      break;
  }
}

static void test_unblock_op(void)
{
  rtems_status_code sc;
  rtems_id task_id;
  Thread_Control *executing;
  Scheduler_SMP_Node *executing_node;
  Thread_Control *other;
  size_t i;

  task_id = start_task(3);

  _Thread_Disable_dispatch();

  executing = _Thread_Executing;
  executing_node = _Scheduler_SMP_Thread_get_node(executing);

  other = get_thread_by_id(task_id);

  for (i = 0; i < RTEMS_ARRAY_SIZE(states); ++i) {
    test_case_unblock_op(
      executing,
      executing_node,
      other,
      states[i]
    );
  }

  _Thread_Change_priority(executing, 1, true);
  rtems_test_assert(executing_node->state == SCHEDULER_SMP_NODE_SCHEDULED);

  _Thread_Enable_dispatch();

  sc = rtems_task_delete(task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void tests(void)
{
  test_change_priority();
  test_change_priority_op();
  test_yield_op();
  test_unblock_op();
}

static void test_task(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;

  tests();

  ctx->cpu_index[arg] = rtems_get_current_processor();

  barrier_wait(ctx);

  rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(0);
}

static void done(uint32_t cpu_index)
{
  printf("test done on processor %" PRIu32 "\n", cpu_index);
}

static void Init(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  rtems_resource_snapshot snapshot;
  uint32_t cpu_count = rtems_get_processor_count();
  uint32_t cpu_index;

  TEST_BEGIN();

  rtems_resource_snapshot_take(&snapshot);

  sc = rtems_barrier_create(
    rtems_build_name('B', 'A', 'R', 'I'),
    RTEMS_BARRIER_AUTOMATIC_RELEASE,
    cpu_count,
    &ctx->barrier_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (cpu_index = 1; cpu_index < cpu_count; ++cpu_index) {
    rtems_id scheduler_id;

    sc = rtems_task_create(
      rtems_build_name('T', 'A', 'S', 'K'),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &ctx->task_id[cpu_index]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_scheduler_ident(SCHED_NAME(cpu_index), &scheduler_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_set_scheduler(ctx->task_id[cpu_index], scheduler_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_start(ctx->task_id[cpu_index], test_task, cpu_index);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  tests();

  barrier_wait(ctx);

  sc = rtems_barrier_delete(ctx->barrier_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  done(0);

  for (cpu_index = 1; cpu_index < cpu_count; ++cpu_index) {
    sc = rtems_task_delete(ctx->task_id[cpu_index]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(ctx->cpu_index[cpu_index] == cpu_index);

    done(cpu_index);
  }

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_SMP_APPLICATION

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS CPU_MAX

#define CONFIGURE_MAXIMUM_PRIORITY 255

#define CONFIGURE_SCHEDULER_PRIORITY_SMP
#define CONFIGURE_SCHEDULER_SIMPLE_SMP
#define CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_CONTEXT_PRIORITY_SMP(a, CONFIGURE_MAXIMUM_PRIORITY + 1);

RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(b);

RTEMS_SCHEDULER_CONTEXT_PRIORITY_AFFINITY_SMP(
  c,
  CONFIGURE_MAXIMUM_PRIORITY + 1
);

#define CONFIGURE_SCHEDULER_CONTROLS \
  RTEMS_SCHEDULER_CONTROL_PRIORITY_SMP(a, SCHED_NAME(0)), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(b, SCHED_NAME(1)), \
  RTEMS_SCHEDULER_CONTROL_PRIORITY_AFFINITY_SMP(c, SCHED_NAME(2))

#define CONFIGURE_SMP_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(2, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL)

#define CONFIGURE_MAXIMUM_TASKS 6
#define CONFIGURE_MAXIMUM_BARRIERS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
