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
#include <rtems/score/smpbarrier.h>

#define TESTS_USE_PRINTK
#include "tmacros.h"

const char rtems_test_name[] = "SMPMRSP 1";

#define CPU_COUNT 32

#define MRSP_COUNT 32

typedef struct {
  uint32_t sleep;
  uint32_t timeout;
  uint32_t obtain[MRSP_COUNT];
} counter;

typedef struct {
  rtems_id main_task_id;
  rtems_id counting_sem_id;
  rtems_id mrsp_ids[MRSP_COUNT];
  rtems_id scheduler_ids[CPU_COUNT];
  rtems_id worker_ids[2 * CPU_COUNT];
  volatile bool stop_worker[CPU_COUNT];
  counter counters[2 * CPU_COUNT];
  Thread_Control *worker_task;
  SMP_barrier_Control barrier;
} test_context;

static test_context test_instance = {
  .barrier = SMP_BARRIER_CONTROL_INITIALIZER
};

static void barrier(test_context *ctx, SMP_barrier_State *bs)
{
  _SMP_barrier_Wait(&ctx->barrier, bs, 2);
}

static void assert_prio(rtems_id task_id, rtems_task_priority expected_prio)
{
  rtems_status_code sc;
  rtems_task_priority prio;

  sc = rtems_task_set_priority(task_id, RTEMS_CURRENT_PRIORITY, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(prio == expected_prio);
}

static void change_prio(rtems_id task_id, rtems_task_priority prio)
{
  rtems_status_code sc;

  sc = rtems_task_set_priority(task_id, prio, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void assert_executing_worker(test_context *ctx)
{
  rtems_test_assert(
    _CPU_Context_Get_is_executing(&ctx->worker_task->Registers)
  );
}

static void obtain_and_release_worker(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  SMP_barrier_State barrier_state = SMP_BARRIER_STATE_INITIALIZER;

  ctx->worker_task = _Thread_Get_executing();

  assert_prio(RTEMS_SELF, 3);

  /* Obtain with timeout (A) */
  barrier(ctx, &barrier_state);

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[0], RTEMS_WAIT, 4);
  rtems_test_assert(sc == RTEMS_TIMEOUT);

  assert_prio(RTEMS_SELF, 3);

  /* Obtain with priority change and timeout (B) */
  barrier(ctx, &barrier_state);

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[0], RTEMS_WAIT, 4);
  rtems_test_assert(sc == RTEMS_TIMEOUT);

  assert_prio(RTEMS_SELF, 1);

  /* Restore priority (C) */
  barrier(ctx, &barrier_state);

  /* Obtain without timeout (D) */
  barrier(ctx, &barrier_state);

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[0], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 2);

  sc = rtems_semaphore_release(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 3);

  /* Worker done (E) */
  barrier(ctx, &barrier_state);

  rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(0);
}

static void test_mrsp_obtain_and_release(void)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  rtems_task_priority prio;
  rtems_id scheduler_id;
  SMP_barrier_State barrier_state = SMP_BARRIER_STATE_INITIALIZER;

  puts("test MrsP obtain and release");

  /* Check executing task parameters */

  sc = rtems_task_get_scheduler(RTEMS_SELF, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(ctx->scheduler_ids[0] == scheduler_id);

  assert_prio(RTEMS_SELF, 2);

  /* Create a MrsP semaphore object and lock it */

  sc = rtems_semaphore_create(
    rtems_build_name('M', 'R', 'S', 'P'),
    1,
    RTEMS_MULTIPROCESSOR_RESOURCE_SHARING
      | RTEMS_BINARY_SEMAPHORE,
    1,
    &ctx->mrsp_ids[0]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 2);

  sc = rtems_semaphore_obtain(ctx->mrsp_ids[0], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(RTEMS_SELF, 1);

  /*
   * The ceiling priority values per scheduler are equal to the value specified
   * for object creation.
   */

  prio = RTEMS_CURRENT_PRIORITY;
  sc = rtems_semaphore_set_priority(
    ctx->mrsp_ids[0],
    ctx->scheduler_ids[0],
    prio,
    &prio
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(prio == 1);

  /* Check the old value and set a new ceiling priority for scheduler B */

  prio = 2;
  sc = rtems_semaphore_set_priority(
    ctx->mrsp_ids[0],
    ctx->scheduler_ids[1],
    prio,
    &prio
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(prio == 1);

  /* Check the ceiling priority values */

  prio = RTEMS_CURRENT_PRIORITY;
  sc = rtems_semaphore_set_priority(
    ctx->mrsp_ids[0],
    ctx->scheduler_ids[0],
    prio,
    &prio
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(prio == 1);

  prio = RTEMS_CURRENT_PRIORITY;
  sc = rtems_semaphore_set_priority(
    ctx->mrsp_ids[0],
    ctx->scheduler_ids[1],
    prio,
    &prio
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(prio == 2);

  /* Check that a thread waiting to get ownership remains executing */

  sc = rtems_task_create(
    rtems_build_name('W', 'O', 'R', 'K'),
    3,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->worker_ids[0]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_scheduler(ctx->worker_ids[0], ctx->scheduler_ids[1]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->worker_ids[0], obtain_and_release_worker, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* Obtain with timeout (A) */
  barrier(ctx, &barrier_state);

  sc = rtems_task_wake_after(2);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(ctx->worker_ids[0], 2);
  assert_executing_worker(ctx);

  /* Obtain with priority change and timeout (B) */
  barrier(ctx, &barrier_state);

  sc = rtems_task_wake_after(2);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(ctx->worker_ids[0], 2);
  change_prio(ctx->worker_ids[0], 1);
  assert_executing_worker(ctx);

  /* Restore priority (C) */
  barrier(ctx, &barrier_state);

  assert_prio(ctx->worker_ids[0], 1);
  change_prio(ctx->worker_ids[0], 3);

  /* Obtain without timeout (D) */
  barrier(ctx, &barrier_state);

  sc = rtems_task_wake_after(2);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(ctx->worker_ids[0], 2);
  assert_executing_worker(ctx);

  sc = rtems_semaphore_release(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* Worker done (E) */
  barrier(ctx, &barrier_state);

  sc = rtems_task_delete(ctx->worker_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_delete(ctx->mrsp_ids[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_mrsp_flush_error(void)
{
  rtems_status_code sc;
  rtems_id id;

  puts("test MrsP flush error");

  sc = rtems_semaphore_create(
    rtems_build_name('M', 'R', 'S', 'P'),
    1,
    RTEMS_MULTIPROCESSOR_RESOURCE_SHARING
      | RTEMS_BINARY_SEMAPHORE,
    1,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_flush(id);
  rtems_test_assert(sc == RTEMS_NOT_DEFINED);

  sc = rtems_semaphore_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_mrsp_initially_locked_error(void)
{
  rtems_status_code sc;
  rtems_id id;

  puts("test MrsP initially locked error");

  sc = rtems_semaphore_create(
    rtems_build_name('M', 'R', 'S', 'P'),
    0,
    RTEMS_MULTIPROCESSOR_RESOURCE_SHARING
      | RTEMS_BINARY_SEMAPHORE,
    1,
    &id
  );
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);
}

static void test_mrsp_nested_obtain_error(void)
{
  rtems_status_code sc;
  rtems_id id;

  puts("test MrsP nested obtain error");

  sc = rtems_semaphore_create(
    rtems_build_name('M', 'R', 'S', 'P'),
    1,
    RTEMS_MULTIPROCESSOR_RESOURCE_SHARING
      | RTEMS_BINARY_SEMAPHORE,
    1,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_obtain(id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_obtain(id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_UNSATISFIED);

  sc = rtems_semaphore_release(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static uint32_t simple_random(uint32_t v)
{
  v *= 1664525;
  v += 1013904223;

  return v;
}

static rtems_interval timeout(uint32_t v)
{
  return (v >> 23) % 4;
}

static void load_worker(rtems_task_argument index)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  uint32_t v = index;

  while (!ctx->stop_worker[index]) {
    uint32_t i = (v >> 13) % MRSP_COUNT;

    assert_prio(RTEMS_SELF, 3 + CPU_COUNT + index);

    if ((v >> 7) % 1024 == 0) {
      /* Give some time to the lower priority tasks */

      ++ctx->counters[index].sleep;

      sc = rtems_task_wake_after(1);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    } else {
      uint32_t n = (v >> 17) % (i + 1);
      uint32_t s;
      uint32_t t;

      /* Nested obtain */
      for (s = 0; s <= n; ++s) {
        uint32_t k = i - s;

        sc = rtems_semaphore_obtain(ctx->mrsp_ids[k], RTEMS_WAIT, timeout(v));
        if (sc == RTEMS_SUCCESSFUL) {
          ++ctx->counters[index].obtain[n];

          assert_prio(RTEMS_SELF, 3 + k);
        } else {
          rtems_test_assert(sc == RTEMS_TIMEOUT);

          ++ctx->counters[index].timeout;

          break;
        }

        v = simple_random(v);
      }

      /* Release in reverse obtain order */
      for (t = 0; t < s; ++t) {
        uint32_t k = i + t - s + 1;

        sc = rtems_semaphore_release(ctx->mrsp_ids[k]);
        rtems_test_assert(sc == RTEMS_SUCCESSFUL);
      }
    }

    v = simple_random(v);
  }

  sc = rtems_semaphore_release(ctx->counting_sem_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(0);
}

static void test_mrsp_load(void)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  uint32_t cpu_count = rtems_get_processor_count();
  uint32_t index;

  puts("test MrsP load");

  assert_prio(RTEMS_SELF, 2);

  sc = rtems_semaphore_create(
    rtems_build_name('S', 'Y', 'N', 'C'),
    0,
    RTEMS_COUNTING_SEMAPHORE,
    0,
    &ctx->counting_sem_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (index = 0; index < MRSP_COUNT; ++index) {
    sc = rtems_semaphore_create(
      'A' + index,
      1,
      RTEMS_MULTIPROCESSOR_RESOURCE_SHARING
        | RTEMS_BINARY_SEMAPHORE,
      3 + index,
      &ctx->mrsp_ids[index]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  for (index = 0; index < cpu_count; ++index) {
    uint32_t a = 2 * index;
    uint32_t b = a + 1;

    sc = rtems_task_create(
      'A' + a,
      3 + MRSP_COUNT + a,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &ctx->worker_ids[a]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_set_scheduler(
      ctx->worker_ids[a],
      ctx->scheduler_ids[index]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_start(
      ctx->worker_ids[a],
      load_worker,
      (rtems_task_argument) a
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_create(
      'A' + b,
      3 + MRSP_COUNT + b,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &ctx->worker_ids[b]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_set_scheduler(
      ctx->worker_ids[b],
      ctx->scheduler_ids[index]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_start(
      ctx->worker_ids[b],
      load_worker,
      (rtems_task_argument) b
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  sc = rtems_task_wake_after(30 * rtems_clock_get_ticks_per_second());
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (index = 0; index < 2 * cpu_count; ++index) {
    ctx->stop_worker[index] = true;
  }

  for (index = 0; index < 2 * cpu_count; ++index) {
    sc = rtems_semaphore_obtain(
      ctx->counting_sem_id,
      RTEMS_WAIT,
      RTEMS_NO_TIMEOUT
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  for (index = 0; index < 2 * cpu_count; ++index) {
    sc = rtems_task_delete(ctx->worker_ids[index]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  for (index = 0; index < MRSP_COUNT; ++index) {
    sc = rtems_semaphore_delete(ctx->mrsp_ids[index]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  sc = rtems_semaphore_delete(ctx->counting_sem_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (index = 0; index < 2 * cpu_count; ++index) {
    uint32_t nest_level;

    printf(
      "worker[%" PRIu32 "][%" PRIu32 "]\n"
        "  sleep = %" PRIu32 "\n"
        "  timeout = %" PRIu32 "\n",
      index / 2,
      index % 2,
      ctx->counters[index].sleep,
      ctx->counters[index].timeout
    );

    for (nest_level = 0; nest_level < MRSP_COUNT; ++nest_level) {
      printf(
        "  obtain[%" PRIu32 "] = %" PRIu32 "\n",
        nest_level,
        ctx->counters[index].obtain[nest_level]
      );
    }
  }
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

  ctx->main_task_id = rtems_task_self();

  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    sc = rtems_scheduler_ident(cpu_index, &ctx->scheduler_ids[cpu_index]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  test_mrsp_flush_error();
  test_mrsp_initially_locked_error();
  test_mrsp_nested_obtain_error();
  test_mrsp_obtain_and_release();
  test_mrsp_load();

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_SMP_APPLICATION

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS (2 * CPU_COUNT + 1)
#define CONFIGURE_MAXIMUM_SEMAPHORES (MRSP_COUNT + 1)
#define CONFIGURE_MAXIMUM_MRSP_SEMAPHORES MRSP_COUNT
#define CONFIGURE_MAXIMUM_TIMERS 1

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
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(16, 16)

#define CONFIGURE_SMP_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(2, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(2, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(3, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(3, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(4, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(4, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(5, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(5, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(6, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(6, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(7, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(7, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(8, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(8, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(9, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(9, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(10, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(10, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(11, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(11, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(12, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(12, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(13, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(13, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(14, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(14, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(15, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(15, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(16, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(16, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT_TASK_PRIORITY 2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
