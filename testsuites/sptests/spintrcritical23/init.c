/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2015, 2020 embedded brains GmbH & Co. KG
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

#include <rtems/test.h>
#include <rtems/test-info.h>

#include <string.h>

#include <rtems.h>
#include <rtems/score/schedulerpriorityimpl.h>
#include <rtems/score/threadimpl.h>

const char rtems_test_name[] = "SPINTRCRITICAL 23";

typedef struct {
  RTEMS_INTERRUPT_LOCK_MEMBER(lock)
  rtems_id task_id;
  Scheduler_priority_Node *scheduler_node;
  rtems_task_priority priority_task;
  rtems_task_priority priority_interrupt;
  volatile bool early;
  volatile bool late;
} test_context;

static T_interrupt_test_state interrupt(void *arg)
{
  test_context *ctx = arg;
  T_interrupt_test_state state;
  rtems_interrupt_lock_context lock_context;
  unsigned int next_priority;

  state = T_interrupt_test_get_state();

  if (state != T_INTERRUPT_TEST_ACTION) {
    return T_INTERRUPT_TEST_CONTINUE;
  }

  rtems_interrupt_lock_acquire(&ctx->lock, &lock_context);

  next_priority = SCHEDULER_PRIORITY_UNMAP(
    (unsigned int) ctx->scheduler_node->Base.Priority.value
  );

  if ( ctx->scheduler_node->Ready_queue.current_priority != next_priority ) {
    rtems_task_priority priority_interrupt;
    rtems_task_priority priority_task;
    rtems_task_priority previous;
    rtems_status_code sc;

    priority_interrupt = ctx->priority_interrupt;
    priority_task = ctx->priority_task;

    rtems_interrupt_lock_release(&ctx->lock, &lock_context);

    sc = rtems_task_set_priority(
      ctx->task_id,
      priority_interrupt,
      &previous
    );
    T_quiet_rsc_success(sc);
    T_quiet_eq_u32(previous, priority_task);

    state = T_INTERRUPT_TEST_DONE;
  } else {
    rtems_interrupt_lock_release(&ctx->lock, &lock_context);

    if ( ctx->early ) {
      state = T_INTERRUPT_TEST_EARLY;
    } else if ( ctx->late ) {
      state = T_INTERRUPT_TEST_LATE;
    } else {
      state = T_INTERRUPT_TEST_CONTINUE;
    }
  }

  return state;
}

static void prepare(void *arg)
{
  test_context *ctx = arg;

  ctx->early = true;
  ctx->late = false;
}

static void action(void *arg)
{
  test_context *ctx = arg;
  rtems_status_code sc;
  rtems_interrupt_lock_context lock_context;
  rtems_task_priority priority_last;
  rtems_task_priority priority_task;
  rtems_task_priority priority_interrupt;
  rtems_task_priority previous;

  rtems_interrupt_lock_acquire(&ctx->lock, &lock_context);

  priority_last = ctx->priority_task;
  priority_task = 1 + (priority_last + 1) % 3;
  priority_interrupt = 1 + (priority_task + 1) % 3;
  ctx->priority_task = priority_task;
  ctx->priority_interrupt = priority_interrupt;

  rtems_interrupt_lock_release(&ctx->lock, &lock_context);

  ctx->early = false;
  sc = rtems_task_set_priority(
    ctx->task_id,
    priority_task,
    &previous
  );
  T_quiet_rsc_success(RTEMS_SUCCESSFUL);
  T_quiet_eq_u32(previous, priority_last);
  ctx->late = true;

  if (T_interrupt_test_get_state() == T_INTERRUPT_TEST_DONE) {
    sc = rtems_task_set_priority(
      ctx->task_id,
      RTEMS_CURRENT_PRIORITY,
      &previous
    );
    T_quiet_rsc_success(sc);
    T_quiet_eq_u32(previous, priority_interrupt);
  }
}

static const T_interrupt_test_config config = {
  .prepare = prepare,
  .action = action,
  .interrupt = interrupt,
  .max_iteration_count = 10000
};

T_TEST_CASE(TaskSetPriorityInterrupt)
{
  test_context ctx;
  T_interrupt_test_state state;
  rtems_status_code sc;
  rtems_task_priority prio;

  sc = rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &prio);
  T_rsc_success(sc);

  memset(&ctx, 0, sizeof(ctx));
  rtems_interrupt_lock_initialize(&ctx.lock, "Test");
  ctx.priority_task = 1;
  ctx.task_id = rtems_task_self();
  ctx.scheduler_node =
    _Scheduler_priority_Thread_get_node(_Thread_Get_executing());

  state = T_interrupt_test(&config, &ctx);
  T_eq_int(state, T_INTERRUPT_TEST_DONE);

  rtems_interrupt_lock_destroy(&ctx.lock);

  sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
  T_rsc_success(sc);
}

static rtems_task Init(rtems_task_argument argument)
{
  rtems_test_run(argument, TEST_STATE);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_TASKS 1

/* We use internal data structures of this scheduler in this test */
#define CONFIGURE_SCHEDULER_PRIORITY

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
