/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
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
#include <rtems/score/schedulerpriority.h>
#include <rtems/score/threadimpl.h>

const char rtems_test_name[] = "SPINTRCRITICAL 23";

typedef struct {
  RTEMS_INTERRUPT_LOCK_MEMBER(lock)
  rtems_id task_id;
  Thread_Control *tcb;
  rtems_task_priority priority_task;
  rtems_task_priority priority_interrupt;
  uint32_t priority_generation;
  Scheduler_priority_Node scheduler_node;
  bool done;
} test_context;

static test_context ctx_instance;

static Thread_Control *get_tcb(rtems_id id)
{
  Objects_Locations location;
  Thread_Control *tcb;

  tcb = _Thread_Get(id, &location);
  _Objects_Put(&tcb->Object);

  rtems_test_assert(tcb != NULL && location == OBJECTS_LOCAL);

  return tcb;
}

static bool scheduler_node_unchanged(const test_context *ctx)
{
   return memcmp(
     &ctx->scheduler_node,
     ctx->tcb->Scheduler.node,
     sizeof(ctx->scheduler_node)
   ) == 0;
}

static void change_priority(rtems_id timer, void *arg)
{
  /* The arg is NULL */
  test_context *ctx = &ctx_instance;
  rtems_interrupt_lock_context lock_context;

  rtems_interrupt_lock_acquire(&ctx->lock, &lock_context);
  if (
    ctx->priority_generation != ctx->tcb->priority_generation
      && scheduler_node_unchanged(ctx)
  ) {
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
  ctx->priority_generation = ctx->tcb->priority_generation;
  memcpy(
    &ctx->scheduler_node,
    ctx->tcb->Scheduler.node,
    sizeof(ctx->scheduler_node)
  );
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
  rtems_status_code sc;

  TEST_BEGIN();

  rtems_interrupt_lock_initialize(&ctx->lock, "Test");
  ctx->priority_task = 1;

  sc = rtems_task_create(
    rtems_build_name('T', 'E', 'S', 'T'),
    ctx->priority_task,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  ctx->tcb = get_tcb(ctx->task_id);

  interrupt_critical_section_test(test_body, ctx, change_priority);
  rtems_test_assert(ctx->done);

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_TASKS 2
#define CONFIGURE_MAXIMUM_TIMERS 1
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
