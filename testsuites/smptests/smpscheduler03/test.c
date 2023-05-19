/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2014, 2017 embedded brains GmbH & Co. KG
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
#include <rtems/libcsupport.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulersmpimpl.h>

#include "tmacros.h"

void Init(rtems_task_argument arg);

static Scheduler_SMP_Node *get_scheduler_node(Thread_Control *thread)
{
  return _Scheduler_SMP_Node_downcast(_Thread_Scheduler_get_home_node(thread));
}

static void apply_priority(
  Thread_Control *thread,
  Priority_Control new_priority,
  Priority_Group_order priority_group_order,
  Thread_queue_Context *queue_context
)
{
  const Scheduler_Control *scheduler;

  scheduler = _Thread_Scheduler_get_home(thread);
  new_priority = _Scheduler_Map_priority(scheduler, new_priority);

  _Thread_queue_Context_initialize(queue_context);
  _Thread_queue_Context_clear_priority_updates(queue_context);
  _Thread_Wait_acquire(thread, queue_context);
  _Thread_Priority_change(
    thread,
    &thread->Real_priority,
    new_priority,
    priority_group_order,
    queue_context
  );
  _Thread_Wait_release(thread, queue_context);
}

static void change_priority(
  Thread_Control *thread,
  Priority_Control new_priority,
  Priority_Group_order priority_group_order
)
{
  Thread_queue_Context queue_context;

  apply_priority(thread, new_priority, priority_group_order, &queue_context);
  _Thread_Priority_update(&queue_context);
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
  ISR_lock_Context lock_context;
  Thread_Control *thread;

  thread = _Thread_Get(task_id, &lock_context);
  rtems_test_assert(thread != NULL);
  _ISR_lock_ISR_enable(&lock_context);

  return thread;
}

static void test_case_change_priority(
  Thread_Control *executing,
  Scheduler_SMP_Node *executing_node,
  Scheduler_SMP_Node_state start_state,
  Priority_Control prio,
  Priority_Group_order priority_group_order,
  Scheduler_SMP_Node_state new_state
)
{
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();

  switch (start_state) {
    case SCHEDULER_SMP_NODE_SCHEDULED:
      change_priority(executing, 1, PRIORITY_GROUP_FIRST);
      break;
    case SCHEDULER_SMP_NODE_READY:
      change_priority(executing, 4, PRIORITY_GROUP_FIRST);
      break;
    default:
      rtems_test_assert(0);
      break;
  }
  rtems_test_assert(executing_node->state == start_state);

  change_priority(executing, prio, priority_group_order);
  rtems_test_assert(executing_node->state == new_state);

  change_priority(executing, 1, PRIORITY_GROUP_FIRST);
  rtems_test_assert(executing_node->state == SCHEDULER_SMP_NODE_SCHEDULED);

  _Thread_Dispatch_enable( cpu_self );
}

static const Scheduler_SMP_Node_state states[2] = {
  SCHEDULER_SMP_NODE_SCHEDULED,
  SCHEDULER_SMP_NODE_READY
};

static const Priority_Control priorities[2] = { 2, 5 };

static const Priority_Group_order priority_group_order[2] = {
  PRIORITY_GROUP_FIRST,
  PRIORITY_GROUP_LAST
};

static void test_change_priority(void)
{
  rtems_status_code sc;
  rtems_id task_id;
  Thread_Control *executing;
  Scheduler_SMP_Node *executing_node;
  size_t i;
  size_t j;
  size_t k;

  task_id = start_task(3);
  executing = _Thread_Get_executing();
  executing_node = get_scheduler_node(executing);

  for (i = 0; i < RTEMS_ARRAY_SIZE(states); ++i) {
    for (j = 0; j < RTEMS_ARRAY_SIZE(priorities); ++j) {
      for (k = 0; k < RTEMS_ARRAY_SIZE(priority_group_order); ++k) {
        test_case_change_priority(
          executing,
          executing_node,
          states[i],
          priorities[j],
          priority_group_order[k],
          states[j]
        );
      }
    }
  }

  sc = rtems_task_delete(task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void update_priority_op(
  Thread_Control *thread,
  Scheduler_SMP_Node *scheduler_node,
  Priority_Control new_priority,
  Priority_Group_order priority_group_order
)
{
  const Scheduler_Control *scheduler;
  ISR_lock_Context state_lock_context;
  ISR_lock_Context scheduler_lock_context;
  Thread_queue_Context queue_context;

  apply_priority(thread, new_priority, priority_group_order, &queue_context);

  _Thread_State_acquire( thread, &state_lock_context );
  scheduler = _Thread_Scheduler_get_home( thread );
  _Scheduler_Acquire_critical( scheduler, &scheduler_lock_context );

  (*scheduler->Operations.update_priority)(
    scheduler,
    thread,
    &scheduler_node->Base
  );

  _Scheduler_Release_critical( scheduler, &scheduler_lock_context );
  _Thread_State_release( thread, &state_lock_context );
}

static void test_case_update_priority_op(
  Thread_Control *executing,
  Scheduler_SMP_Node *executing_node,
  Thread_Control *other,
  Scheduler_SMP_Node_state start_state,
  Priority_Control prio,
  Priority_Group_order priority_group_order,
  Scheduler_SMP_Node_state new_state
)
{
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();

  switch (start_state) {
    case SCHEDULER_SMP_NODE_SCHEDULED:
      change_priority(executing, 1, PRIORITY_GROUP_FIRST);
      break;
    case SCHEDULER_SMP_NODE_READY:
      change_priority(executing, 4, PRIORITY_GROUP_FIRST);
      break;
    default:
      rtems_test_assert(0);
      break;
  }
  rtems_test_assert(executing_node->state == start_state);

  update_priority_op(executing, executing_node, prio, priority_group_order);
  rtems_test_assert(executing_node->state == new_state);

  if (start_state != new_state) {
    switch (start_state) {
      case SCHEDULER_SMP_NODE_SCHEDULED:
        rtems_test_assert(cpu_self->heir == other);
        break;
      case SCHEDULER_SMP_NODE_READY:
        rtems_test_assert(cpu_self->heir == executing);
        break;
      default:
        rtems_test_assert(0);
        break;
    }
  }

  change_priority(executing, 1, PRIORITY_GROUP_FIRST);
  rtems_test_assert(executing_node->state == SCHEDULER_SMP_NODE_SCHEDULED);

  _Thread_Dispatch_enable( cpu_self );
}

static void test_update_priority_op(void)
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
  executing = _Thread_Get_executing();
  executing_node = get_scheduler_node(executing);

  other = get_thread_by_id(task_id);

  for (i = 0; i < RTEMS_ARRAY_SIZE(states); ++i) {
    for (j = 0; j < RTEMS_ARRAY_SIZE(priorities); ++j) {
      for (k = 0; k < RTEMS_ARRAY_SIZE(priority_group_order); ++k) {
        test_case_update_priority_op(
          executing,
          executing_node,
          other,
          states[i],
          priorities[j],
          priority_group_order[k],
          states[j]
        );
      }
    }
  }

  sc = rtems_task_delete(task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void yield_op(
  Thread_Control *thread,
  Scheduler_SMP_Node *scheduler_node
)
{
  const Scheduler_Control *scheduler;
  ISR_lock_Context state_lock_context;
  ISR_lock_Context scheduler_lock_context;

  _Thread_State_acquire( thread, &state_lock_context );
  scheduler = _Thread_Scheduler_get_home( thread );
  _Scheduler_Acquire_critical( scheduler, &scheduler_lock_context );

  (*scheduler->Operations.yield)(
    scheduler,
    thread,
    &scheduler_node->Base
  );

  _Scheduler_Release_critical( scheduler, &scheduler_lock_context );
  _Thread_State_release( thread, &state_lock_context );
}

static void test_case_yield_op(
  Thread_Control *executing,
  Scheduler_SMP_Node *executing_node,
  Thread_Control *other,
  Scheduler_SMP_Node_state start_state,
  Scheduler_SMP_Node_state new_state
)
{
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();

  change_priority(executing, 4, PRIORITY_GROUP_LAST);
  change_priority(other, 4, PRIORITY_GROUP_LAST);

  switch (start_state) {
    case SCHEDULER_SMP_NODE_SCHEDULED:
      switch (new_state) {
        case SCHEDULER_SMP_NODE_SCHEDULED:
          change_priority(executing, 2, PRIORITY_GROUP_LAST);
          change_priority(other, 3, PRIORITY_GROUP_LAST);
          break;
        case SCHEDULER_SMP_NODE_READY:
          change_priority(executing, 2, PRIORITY_GROUP_LAST);
          change_priority(other, 2, PRIORITY_GROUP_LAST);
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
          change_priority(executing, 3, PRIORITY_GROUP_LAST);
          change_priority(other, 2, PRIORITY_GROUP_LAST);
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

  yield_op(executing, executing_node);
  rtems_test_assert(executing_node->state == new_state);

  switch (new_state) {
    case SCHEDULER_SMP_NODE_SCHEDULED:
    case SCHEDULER_SMP_NODE_READY:
      break;
    default:
      rtems_test_assert(0);
      break;
  }

  change_priority(executing, 1, PRIORITY_GROUP_FIRST);
  rtems_test_assert(executing_node->state == SCHEDULER_SMP_NODE_SCHEDULED);

  _Thread_Dispatch_enable( cpu_self );
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
  executing = _Thread_Get_executing();
  executing_node = get_scheduler_node(executing);

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

  sc = rtems_task_delete(task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void block_op(
  Thread_Control *thread,
  Scheduler_SMP_Node *scheduler_node
)
{
  const Scheduler_Control *scheduler;
  ISR_lock_Context state_lock_context;
  ISR_lock_Context scheduler_lock_context;

  _Thread_State_acquire( thread, &state_lock_context );
  scheduler = _Thread_Scheduler_get_home( thread );
  _Scheduler_Acquire_critical( scheduler, &scheduler_lock_context );

  (*scheduler->Operations.block)(scheduler, thread, &scheduler_node->Base);

  _Scheduler_Release_critical( scheduler, &scheduler_lock_context );
  _Thread_State_release( thread, &state_lock_context );
}

static void unblock_op(
  Thread_Control *thread,
  Scheduler_SMP_Node *scheduler_node
)
{
  const Scheduler_Control *scheduler;
  ISR_lock_Context state_lock_context;
  ISR_lock_Context scheduler_lock_context;

  _Thread_State_acquire( thread, &state_lock_context );
  scheduler = _Thread_Scheduler_get_home( thread );
  _Scheduler_Acquire_critical( scheduler, &scheduler_lock_context );

  (*scheduler->Operations.unblock)(
    scheduler,
    thread,
    &scheduler_node->Base
  );

  _Scheduler_Release_critical( scheduler, &scheduler_lock_context );
  _Thread_State_release( thread, &state_lock_context );
}

static void test_case_unblock_op(
  Thread_Control *executing,
  Scheduler_SMP_Node *executing_node,
  Thread_Control *other,
  Scheduler_SMP_Node_state new_state
)
{
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();

  switch (new_state) {
    case SCHEDULER_SMP_NODE_SCHEDULED:
      change_priority(executing, 2, PRIORITY_GROUP_LAST);
      rtems_test_assert(executing_node->state == SCHEDULER_SMP_NODE_SCHEDULED);
      break;
    case SCHEDULER_SMP_NODE_READY:
      change_priority(executing, 4, PRIORITY_GROUP_LAST);
      rtems_test_assert(executing_node->state == SCHEDULER_SMP_NODE_READY);
      break;
    default:
      rtems_test_assert(0);
      break;
  }

  block_op(executing, executing_node);
  rtems_test_assert(executing_node->state == SCHEDULER_SMP_NODE_BLOCKED);

  unblock_op(executing, executing_node);
  rtems_test_assert(executing_node->state == new_state);

  switch (new_state) {
    case SCHEDULER_SMP_NODE_SCHEDULED:
    case SCHEDULER_SMP_NODE_READY:
      break;
    default:
      rtems_test_assert(0);
      break;
  }

  change_priority(executing, 1, PRIORITY_GROUP_FIRST);
  rtems_test_assert(executing_node->state == SCHEDULER_SMP_NODE_SCHEDULED);

  _Thread_Dispatch_enable( cpu_self );
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
  executing = _Thread_Get_executing();
  executing_node = get_scheduler_node(executing);

  other = get_thread_by_id(task_id);

  for (i = 0; i < RTEMS_ARRAY_SIZE(states); ++i) {
    test_case_unblock_op(
      executing,
      executing_node,
      other,
      states[i]
    );
  }

  sc = rtems_task_delete(task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

void Init(rtems_task_argument arg)
{
  rtems_resource_snapshot snapshot;
  rtems_status_code sc;
  rtems_id task_id;

  TEST_BEGIN();

  rtems_resource_snapshot_take(&snapshot);

  sc = rtems_task_create(
    rtems_build_name('T', 'A', 'S', 'K'),
    255,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  test_change_priority();
  test_update_priority_op();
  test_yield_op();
  test_unblock_op();

  sc = rtems_task_delete(task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));

  TEST_END();
  rtems_test_exit(0);
}
