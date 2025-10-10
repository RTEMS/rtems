/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2016, 2020 embedded brains GmbH & Co. KG
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

#include <tmacros.h>

#include <rtems.h>

const char rtems_test_name[] = "SMPSCHEDEDF 2";

#define CPU_COUNT 2

#define TASK_COUNT 5

#define P(i) (UINT32_C(2) + i)

#define A(cpu0, cpu1) ((cpu1 << 1) | cpu0)

typedef enum {
  T0,
  T1,
  T2,
  T3,
  T4,
  IDLE
} task_index;

typedef struct {
  enum {
    KIND_RESET,
    KIND_SET_PRIORITY,
    KIND_SET_AFFINITY,
    KIND_BLOCK,
    KIND_UNBLOCK
  } kind;

  task_index index;

  struct {
    rtems_task_priority priority;
    uint32_t cpu_set;
  } data;

  uint8_t expected_cpu_allocations[CPU_COUNT];
} test_action;

typedef struct {
  rtems_id timer_id;
  rtems_id master_id;
  rtems_id task_ids[TASK_COUNT];
  size_t action_index;
} test_context;

#define RESET \
  { \
    KIND_RESET, \
    0, \
    { 0 }, \
    { IDLE, IDLE } \
  }

#define SET_PRIORITY(index, prio, cpu0, cpu1) \
  { \
    KIND_SET_PRIORITY, \
    index, \
    { .priority = prio }, \
    { cpu0, cpu1 } \
  }

#define SET_AFFINITY(index, aff, cpu0, cpu1) \
  { \
    KIND_SET_AFFINITY, \
    index, \
    { .cpu_set = aff }, \
    { cpu0, cpu1 } \
  }

#define BLOCK(index, cpu0, cpu1) \
  { \
    KIND_BLOCK, \
    index, \
    { 0 }, \
    { cpu0, cpu1 } \
  }

#define UNBLOCK(index, cpu0, cpu1) \
  { \
    KIND_UNBLOCK, \
    index, \
    { 0 }, \
    { cpu0, cpu1 } \
  }

static const test_action test_actions[] = {
  RESET,
  UNBLOCK(      T0,             T0, IDLE),
  UNBLOCK(      T1,             T0,   T1),
  UNBLOCK(      T3,             T0,   T1),
  SET_PRIORITY( T1,  P(2),      T0,   T1),
  SET_PRIORITY( T3,  P(1),      T0,   T3),
  BLOCK(        T3,             T0,   T1),
  SET_AFFINITY( T1,  A(1, 1),   T0,   T1),
  SET_AFFINITY( T1,  A(1, 0),   T1,   T0),
  SET_AFFINITY( T1,  A(1, 1),   T1,   T0),
  SET_AFFINITY( T1,  A(1, 0),   T1,   T0),
  SET_AFFINITY( T1,  A(0, 1),   T0,   T1),
  BLOCK(        T0,           IDLE,   T1),
  UNBLOCK(      T0,             T0,   T1),
  BLOCK(        T1,             T0, IDLE),
  UNBLOCK(      T1,             T0,   T1),
  /*
   * Show that FIFO order is honoured across all threads of the same priority.
   */
  RESET,
  SET_PRIORITY( T1,  P(0),    IDLE, IDLE),
  SET_PRIORITY( T2,  P(1),    IDLE, IDLE),
  SET_PRIORITY( T3,  P(1),    IDLE, IDLE),
  SET_AFFINITY( T3,  A(1, 0), IDLE, IDLE),
  SET_PRIORITY( T4,  P(1),    IDLE, IDLE),
  SET_AFFINITY( T4,  A(1, 0), IDLE, IDLE),
  UNBLOCK(      T0,             T0, IDLE),
  UNBLOCK(      T1,             T0,   T1),
  UNBLOCK(      T2,             T0,   T1),
  UNBLOCK(      T3,             T0,   T1),
  UNBLOCK(      T4,             T0,   T1),
  BLOCK(        T1,             T0,   T2),
  BLOCK(        T2,             T3,   T0),
  BLOCK(        T3,             T4,   T0),
  /*
   * Schedule a high priority affine thread directly with a low priority affine
   * thread in the corresponding ready queue.  In this case we, remove the
   * affine ready queue in _Scheduler_EDF_SMP_Allocate_processor().
   */
  RESET,
  UNBLOCK(      T0,             T0, IDLE),
  UNBLOCK(      T1,             T0,   T1),
  SET_PRIORITY( T1,  P(2),      T0,   T1),
  SET_AFFINITY( T3,  A(0, 1),   T0,   T1),
  UNBLOCK(      T3,             T0,   T1),
  SET_PRIORITY( T2,  P(1),      T0,   T1),
  SET_AFFINITY( T2,  A(0, 1),   T0,   T1),
  UNBLOCK(      T2,             T0,   T2),
  BLOCK(        T1,             T0,   T2),
  BLOCK(        T2,             T0,   T3),
  /* Force migration of a higher priority one-to-all thread */
  RESET,
  UNBLOCK(      T0,             T0, IDLE),
  SET_AFFINITY( T1,  A(1, 0),   T0, IDLE),
  UNBLOCK(      T1,             T1,   T0),
  /*
   * Block a one-to-one thread while having a non-empty affine ready queue on
   * the same processor.
   */
  RESET,
  SET_AFFINITY( T1,  A(1, 0), IDLE, IDLE),
  SET_AFFINITY( T3,  A(1, 0), IDLE, IDLE),
  UNBLOCK(      T0,             T0, IDLE),
  UNBLOCK(      T1,             T1,   T0),
  UNBLOCK(      T2,             T1,   T0),
  UNBLOCK(      T3,             T1,   T0),
  BLOCK(        T1,             T2,   T0),
  BLOCK(        T0,             T3,   T2),
  /*
   * Make sure that a one-to-one thread does not get the wrong processor
   * allocated after selecting the highest ready thread.
   */
  RESET,
  SET_AFFINITY( T1,  A(1, 0), IDLE, IDLE),
  SET_AFFINITY( T2,  A(1, 0), IDLE, IDLE),
  UNBLOCK(      T0,             T0, IDLE),
  UNBLOCK(      T1,             T1,   T0),
  UNBLOCK(      T2,             T1,   T0),
  BLOCK(        T0,             T1, IDLE),
  RESET
};

static test_context test_instance;

static void set_priority(rtems_id id, rtems_task_priority prio)
{
  rtems_status_code sc;

  sc = rtems_task_set_priority(id, prio, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void set_affinity(rtems_id id, uint32_t cpu_set_32)
{
  rtems_status_code sc;
  cpu_set_t cpu_set;
  size_t i;

  CPU_ZERO(&cpu_set);

  for (i = 0; i < CPU_COUNT; ++i) {
    if ((cpu_set_32 & (UINT32_C(1) << i)) != 0) {
      CPU_SET(i, &cpu_set);
    }
  }

  sc = rtems_task_set_affinity(id, sizeof(cpu_set), &cpu_set);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

/*
 * The goal of the reset() function is to bring back a defined initial system
 * state for each test case.  All tasks of the test shall be suspended.  The
 * idle threads shall be ordered in the scheduled chain according to the CPU
 * index.
 */
static void reset(test_context *ctx)
{
  rtems_status_code sc;
  size_t i;

  for (i = 0; i < TASK_COUNT; ++i) {
    set_priority(ctx->task_ids[i], P(i));
    set_affinity(ctx->task_ids[i], A(1, 1));
  }

  for (i = CPU_COUNT; i < TASK_COUNT; ++i) {
    sc = rtems_task_suspend(ctx->task_ids[i]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL || sc == RTEMS_ALREADY_SUSPENDED);
  }

  for (i = 0; i < CPU_COUNT; ++i) {
    sc = rtems_task_resume(ctx->task_ids[i]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL || sc == RTEMS_INCORRECT_STATE);
  }

  /*
   * Order the idle threads explicitly.  Test cases may move the idle threads
   * around.  We have to ensure that the idle threads are ordered according to
   * the CPU index, otherwise the processor allocations cannot be specified for
   * a test case.  The idle threads of a scheduler have all the same priority,
   * so we have to take the FIFO ordering within a priority group into account.
   */
  for (i = 0; i < CPU_COUNT; ++i) {
    const Per_CPU_Control *c;
    const Thread_Control *h;

    c = _Per_CPU_Get_by_index(CPU_COUNT - 1 - i);
    h = c->heir;

    sc = rtems_task_suspend(h->Object.id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void check_cpu_allocations(test_context *ctx, const test_action *action)
{
  size_t i;

  for (i = 0; i < CPU_COUNT; ++i) {
    task_index e;
    const Per_CPU_Control *c;
    const Thread_Control *h;

    e = action->expected_cpu_allocations[i];
    c = _Per_CPU_Get_by_index(i);
    h = c->heir;

    if (e != IDLE) {
      rtems_test_assert(h->Object.id == ctx->task_ids[e]);
    } else {
      rtems_test_assert(h->is_idle);
    }
  }
}

/*
 * Use a timer to execute the actions, since it runs with thread dispatching
 * disabled.  This is necessary to check the expected processor allocations.
 */
static void timer(rtems_id id, void *arg)
{
  test_context *ctx;
  rtems_status_code sc;
  size_t i;

  ctx = arg;
  i = ctx->action_index;

  if (i == 0) {
    sc = rtems_task_suspend(ctx->master_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  if (i < RTEMS_ARRAY_SIZE(test_actions)) {
    const test_action *action = &test_actions[i];
    rtems_id task;

    ctx->action_index = i + 1;

    task = ctx->task_ids[action->index];

    switch (action->kind) {
      case KIND_SET_PRIORITY:
        set_priority(task, action->data.priority);
        break;
      case KIND_SET_AFFINITY:
        set_affinity(task, action->data.cpu_set);
        break;
      case KIND_BLOCK:
        sc = rtems_task_suspend(task);
        rtems_test_assert(sc == RTEMS_SUCCESSFUL);
        break;
      case KIND_UNBLOCK:
        sc = rtems_task_resume(task);
        rtems_test_assert(sc == RTEMS_SUCCESSFUL);
        break;
      default:
        rtems_test_assert(action->kind == KIND_RESET);
        reset(ctx);
        break;
    }

    check_cpu_allocations(ctx, action);

    sc = rtems_timer_reset(id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  } else {
    sc = rtems_task_resume(ctx->master_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_event_transient_send(ctx->master_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void do_nothing_task(rtems_task_argument arg)
{
  (void) arg;

  while (true) {
    /* Do nothing */
  }
}

static void test(void)
{
  test_context *ctx;
  rtems_status_code sc;
  size_t i;

  ctx = &test_instance;

  ctx->master_id = rtems_task_self();

  for (i = 0; i < TASK_COUNT; ++i) {
    sc = rtems_task_create(
      rtems_build_name(' ', ' ', 'T', '0' + i),
      P(i),
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &ctx->task_ids[i]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_start(ctx->task_ids[i], do_nothing_task, 0);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  sc = rtems_timer_create(
    rtems_build_name('A', 'C', 'T', 'N'),
    &ctx->timer_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_timer_fire_after(ctx->timer_id, 1, timer, ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (i = 0; i < TASK_COUNT; ++i) {
    sc = rtems_task_delete(ctx->task_ids[i]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  sc = rtems_timer_delete(ctx->timer_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void Init(rtems_task_argument arg)
{
  (void) arg;

  TEST_BEGIN();

  if (rtems_scheduler_get_processor_maximum() == CPU_COUNT) {
    test();
  } else {
    puts("warning: wrong processor count to run the test");
  }

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS (1 + TASK_COUNT)
#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_SCHEDULER_EDF_SMP

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
