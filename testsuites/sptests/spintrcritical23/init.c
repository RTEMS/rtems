/*
 * Copyright (c) 2015, 2017 embedded brains GmbH.  All rights reserved.
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

#include <tmacros.h>
#include <intrcritical.h>

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
  bool done;
} test_context;

static test_context ctx_instance;

static void change_priority(rtems_id timer, void *arg)
{
  /* The arg is NULL */
  test_context *ctx = &ctx_instance;
  rtems_interrupt_lock_context lock_context;
  unsigned int next_priority;

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
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(previous == priority_task);

    ctx->done = true;
  } else {
    rtems_interrupt_lock_release(&ctx->lock, &lock_context);
  }
}

static bool test_body(void *arg)
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

  sc = rtems_task_set_priority(
    ctx->task_id,
    priority_task,
    &previous
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(previous == priority_last);

  if (ctx->done) {
    sc = rtems_task_set_priority(
      ctx->task_id,
      RTEMS_CURRENT_PRIORITY,
      &previous
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(previous == priority_interrupt);
  }

  return ctx->done;
}

static void Init(rtems_task_argument arg)
{
  test_context *ctx = &ctx_instance;

  TEST_BEGIN();

  rtems_interrupt_lock_initialize(&ctx->lock, "Test");
  ctx->priority_task = 1;
  ctx->task_id = rtems_task_self();
  ctx->scheduler_node =
    _Scheduler_priority_Thread_get_node(_Thread_Get_executing());

  interrupt_critical_section_test(test_body, ctx, change_priority);
  rtems_test_assert(ctx->done);

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_TIMERS 1
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

/* We use internal data structures of this scheduler in this test */
#define CONFIGURE_SCHEDULER_PRIORITY

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
