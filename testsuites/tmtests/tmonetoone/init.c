/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
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

#include <sched.h>

#include <rtems.h>
#include <rtems/test.h>
#include <rtems/thread.h>

#include <tmacros.h>

const char rtems_test_name[] = "TMONETOONE";

typedef enum {
  TEST_YIELD,
  TEST_EVENTS,
  TEST_BSEM,
  TEST_CLASSIC_FIFO_BSEM,
  TEST_CLASSIC_PRIO_BSEM
} test_variant;

typedef struct {
  volatile uint32_t counter;
  test_variant variant;
  rtems_id task;
  rtems_binary_semaphore bsem;
  rtems_id classic_fifo_bsem;
  rtems_id classic_prio_bsem;
  rtems_id other_task;
  rtems_binary_semaphore *other_bsem;
  rtems_id other_classic_fifo_bsem;
  rtems_id other_classic_prio_bsem;
} task_context RTEMS_ALIGNED(CPU_CACHE_LINE_BYTES);

typedef struct {
  task_context a;
  task_context b;
} test_context;

static test_context test_instance;

static void test_yield(task_context *tc)
{
  rtems_event_set events;
  uint32_t counter;

  (void)rtems_event_receive(
    RTEMS_EVENT_0,
    RTEMS_WAIT | RTEMS_EVENT_ALL,
    RTEMS_NO_TIMEOUT,
    &events
  );

  counter = 0;

  while (true) {
    (void)sched_yield();
    ++counter;
    tc->counter = counter;
  }
}

static void test_events(task_context *tc)
{
  uint32_t counter;
  rtems_id other;

  counter = 0;
  other = tc->other_task;

  while (true) {
    rtems_event_set events;

    (void)rtems_event_receive(
      RTEMS_EVENT_0,
      RTEMS_WAIT | RTEMS_EVENT_ALL,
      RTEMS_NO_TIMEOUT,
      &events
    );
    (void)rtems_event_send(other, RTEMS_EVENT_0);
    ++counter;
    tc->counter = counter;
  }
}

static void test_bsem(task_context *tc)
{
  uint32_t counter;
  rtems_binary_semaphore *other;

  counter = 0;
  other = tc->other_bsem;

  while (true) {
    rtems_binary_semaphore_wait(&tc->bsem);
    rtems_binary_semaphore_post(other);
    ++counter;
    tc->counter = counter;
  }
}

static void test_classic_fifo_bsem(task_context *tc)
{
  uint32_t counter;
  rtems_id own;
  rtems_id other;

  counter = 0;
  own = tc->classic_fifo_bsem;
  other = tc->other_classic_fifo_bsem;

  while (true) {
    (void)rtems_semaphore_obtain(own, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    (void)rtems_semaphore_release(other);
    ++counter;
    tc->counter = counter;
  }
}

static void test_classic_prio_bsem(task_context *tc)
{
  uint32_t counter;
  rtems_id own;
  rtems_id other;

  counter = 0;
  own = tc->classic_prio_bsem;
  other = tc->other_classic_prio_bsem;

  while (true) {
    (void)rtems_semaphore_obtain(own, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    (void)rtems_semaphore_release(other);
    ++counter;
    tc->counter = counter;
  }
}

static void worker_task(rtems_task_argument arg)
{
  task_context *tc;

  tc = (task_context *) arg;

  switch (tc->variant) {
    case TEST_YIELD:
      test_yield(tc);
      break;
    case TEST_EVENTS:
      test_events(tc);
      break;
    case TEST_BSEM:
      test_bsem(tc);
      break;
    case TEST_CLASSIC_FIFO_BSEM:
      test_classic_fifo_bsem(tc);
      break;
    case TEST_CLASSIC_PRIO_BSEM:
      test_classic_prio_bsem(tc);
      break;
    default:
      rtems_test_assert(0);
      break;
  }
}

static void create_task(task_context *tc)
{
  rtems_status_code sc;

  rtems_binary_semaphore_init(&tc->bsem, "test");

  sc = rtems_semaphore_create(
    rtems_build_name('T', 'E', 'S', 'T'),
    0,
    RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_FIFO,
    0,
    &tc->classic_fifo_bsem
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_create(
    rtems_build_name('T', 'E', 'S', 'T'),
    0,
    RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_PRIORITY,
    0,
    &tc->classic_prio_bsem
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('T', 'E', 'S', 'T'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &tc->task
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(tc->task, worker_task, (rtems_task_argument) tc);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static const char * const variant_names[] = {
  "yield",
  "event",
  "self-contained binary semaphore",
  "Classic binary semaphore (FIFO)",
  "Classic binary semaphore (priority)"
};

static void prepare(test_context *ctx, test_variant variant)
{
  rtems_status_code sc;

  printf("%s\n", variant_names[variant]);

  ctx->a.variant = variant;
  ctx->b.variant = variant;

  ctx->a.counter = 0;
  ctx->b.counter = 0;

  sc = rtems_task_restart(ctx->a.task, (rtems_task_argument) &ctx->a);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_restart(ctx->b.task, (rtems_task_argument) &ctx->b);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_wake_after(2);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void run(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_task_wake_after(rtems_clock_get_ticks_per_second());
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  printf("a %" PRIu32 "\nb %" PRIu32 "\n", ctx->a.counter, ctx->b.counter);
}

static void Init(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;

  TEST_BEGIN();

  create_task(&ctx->a);
  create_task(&ctx->b);

  ctx->a.other_task = ctx->b.task;
  ctx->a.other_bsem = &ctx->b.bsem;
  ctx->a.other_classic_fifo_bsem = ctx->b.classic_fifo_bsem;
  ctx->a.other_classic_prio_bsem = ctx->b.classic_prio_bsem;

  ctx->b.other_task = ctx->a.task;
  ctx->b.other_bsem = &ctx->a.bsem;
  ctx->b.other_classic_fifo_bsem = ctx->a.classic_fifo_bsem;
  ctx->b.other_classic_prio_bsem = ctx->a.classic_prio_bsem;

  prepare(ctx, TEST_YIELD);

  sc = rtems_event_send(ctx->a.task, RTEMS_EVENT_0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_send(ctx->b.task, RTEMS_EVENT_0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  run(ctx);
  prepare(ctx, TEST_EVENTS);

  sc = rtems_event_send(ctx->a.task, RTEMS_EVENT_0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  run(ctx);
  prepare(ctx, TEST_BSEM);

  rtems_binary_semaphore_post(&ctx->a.bsem);

  run(ctx);
  prepare(ctx, TEST_CLASSIC_FIFO_BSEM);

  sc = rtems_semaphore_release(ctx->a.classic_fifo_bsem);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  run(ctx);
  prepare(ctx, TEST_CLASSIC_PRIO_BSEM);

  sc = rtems_semaphore_release(ctx->a.classic_prio_bsem);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  run(ctx);

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 3

#define CONFIGURE_MAXIMUM_SEMAPHORES 4

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
