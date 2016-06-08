/*
 * Copyright (c) 2013, 2016 embedded brains GmbH.  All rights reserved.
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

#include <rtems/score/smplock.h>
#include <rtems/score/smplockmcs.h>
#include <rtems/score/smplockseq.h>
#include <rtems/score/smpbarrier.h>
#include <rtems/score/atomic.h>
#include <rtems.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPLOCK 1";

#define TASK_PRIORITY 1

#define CPU_COUNT 32

#define TEST_COUNT 11

typedef enum {
  INITIAL,
  START_TEST,
  STOP_TEST
} states;

typedef struct {
  Atomic_Uint state;
  SMP_barrier_Control barrier;
  rtems_id timer_id;
  rtems_interval timeout;
  unsigned long counter[TEST_COUNT];
  unsigned long test_counter[TEST_COUNT][CPU_COUNT];
  SMP_lock_Control lock;
#if defined(RTEMS_PROFILING)
  SMP_lock_Stats mcs_stats;
#endif
  SMP_MCS_lock_Control mcs_lock;
  SMP_sequence_lock_Control seq_lock;
  char unused_space_for_cache_line_separation_0[128];
  int a;
  char unused_space_for_cache_line_separation_1[128];
  int b;
} global_context;

static global_context context = {
  .state = ATOMIC_INITIALIZER_UINT(INITIAL),
  .barrier = SMP_BARRIER_CONTROL_INITIALIZER,
  .lock = SMP_LOCK_INITIALIZER("global ticket"),
#if defined(RTEMS_PROFILING)
  .mcs_stats = SMP_LOCK_STATS_INITIALIZER("global MCS"),
#endif
  .mcs_lock = SMP_MCS_LOCK_INITIALIZER,
  .seq_lock = SMP_SEQUENCE_LOCK_INITIALIZER
};

static const char * const test_names[TEST_COUNT] = {
  "global ticket lock with local counter",
  "global MCS lock with local counter",
  "global ticket lock with global counter",
  "global MCS lock with global counter",
  "local ticket lock with local counter",
  "local MCS lock with local counter",
  "local ticket lock with global counter",
  "local MCS lock with global counter",
  "global ticket lock with busy section",
  "global MCS lock with busy section",
  "sequence lock"
};

static void stop_test_timer(rtems_id timer_id, void *arg)
{
  global_context *ctx = arg;

  _Atomic_Store_uint(&ctx->state, STOP_TEST, ATOMIC_ORDER_RELEASE);
}

static void wait_for_state(global_context *ctx, int desired_state)
{
  while (
    _Atomic_Load_uint(&ctx->state, ATOMIC_ORDER_ACQUIRE) != desired_state
  ) {
    /* Wait */
  }
}

static bool assert_state(global_context *ctx, int desired_state)
{
  return _Atomic_Load_uint(&ctx->state, ATOMIC_ORDER_RELAXED) == desired_state;
}

typedef void (*test_body)(
  int test,
  global_context *ctx,
  SMP_barrier_State *bs,
  unsigned int cpu_count,
  unsigned int cpu_self
);

static void test_0_body(
  int test,
  global_context *ctx,
  SMP_barrier_State *bs,
  unsigned int cpu_count,
  unsigned int cpu_self
)
{
  unsigned long counter = 0;
  SMP_lock_Context lock_context;

  while (assert_state(ctx, START_TEST)) {
    _SMP_lock_Acquire(&ctx->lock, &lock_context);
    _SMP_lock_Release(&ctx->lock, &lock_context);
    ++counter;
  }

  ctx->test_counter[test][cpu_self] = counter;
}

static void test_1_body(
  int test,
  global_context *ctx,
  SMP_barrier_State *bs,
  unsigned int cpu_count,
  unsigned int cpu_self
)
{
  unsigned long counter = 0;
  SMP_MCS_lock_Context lock_context;

  while (assert_state(ctx, START_TEST)) {
    _SMP_MCS_lock_Acquire(&ctx->mcs_lock, &lock_context, &ctx->mcs_stats);
    _SMP_MCS_lock_Release(&ctx->mcs_lock, &lock_context);
    ++counter;
  }

  ctx->test_counter[test][cpu_self] = counter;
}

static void test_2_body(
  int test,
  global_context *ctx,
  SMP_barrier_State *bs,
  unsigned int cpu_count,
  unsigned int cpu_self
)
{
  unsigned long counter = 0;
  SMP_lock_Context lock_context;

  while (assert_state(ctx, START_TEST)) {
    _SMP_lock_Acquire(&ctx->lock, &lock_context);
    ++ctx->counter[test];
    _SMP_lock_Release(&ctx->lock, &lock_context);
    ++counter;
  }

  ctx->test_counter[test][cpu_self] = counter;
}

static void test_3_body(
  int test,
  global_context *ctx,
  SMP_barrier_State *bs,
  unsigned int cpu_count,
  unsigned int cpu_self
)
{
  unsigned long counter = 0;
  SMP_MCS_lock_Context lock_context;

  while (assert_state(ctx, START_TEST)) {
    _SMP_MCS_lock_Acquire(&ctx->mcs_lock, &lock_context, &ctx->mcs_stats);
    ++ctx->counter[test];
    _SMP_MCS_lock_Release(&ctx->mcs_lock, &lock_context);
    ++counter;
  }

  ctx->test_counter[test][cpu_self] = counter;
}

static void test_4_body(
  int test,
  global_context *ctx,
  SMP_barrier_State *bs,
  unsigned int cpu_count,
  unsigned int cpu_self
)
{
  unsigned long counter = 0;
  SMP_lock_Control lock;
  SMP_lock_Context lock_context;

  _SMP_lock_Initialize(&lock, "local");

  while (assert_state(ctx, START_TEST)) {
    _SMP_lock_Acquire(&lock, &lock_context);
    _SMP_lock_Release(&lock, &lock_context);
    ++counter;
  }

  _SMP_lock_Destroy(&lock);

  ctx->test_counter[test][cpu_self] = counter;
}

static void test_5_body(
  int test,
  global_context *ctx,
  SMP_barrier_State *bs,
  unsigned int cpu_count,
  unsigned int cpu_self
)
{
  unsigned long counter = 0;
#if defined(RTEMS_PROFILING)
  SMP_lock_Stats stats;
#endif
  SMP_MCS_lock_Control lock;
  SMP_MCS_lock_Context lock_context;

  _SMP_lock_Stats_initialize(&stats, "local");
  _SMP_MCS_lock_Initialize(&lock);

  while (assert_state(ctx, START_TEST)) {
    _SMP_MCS_lock_Acquire(&lock, &lock_context, &stats);
    _SMP_MCS_lock_Release(&lock, &lock_context);
    ++counter;
  }

  _SMP_MCS_lock_Destroy(&lock);
  _SMP_lock_Stats_destroy(&stats);

  ctx->test_counter[test][cpu_self] = counter;
}

static void test_6_body(
  int test,
  global_context *ctx,
  SMP_barrier_State *bs,
  unsigned int cpu_count,
  unsigned int cpu_self
)
{
  unsigned long counter = 0;
  SMP_lock_Control lock;
  SMP_lock_Context lock_context;

  _SMP_lock_Initialize(&lock, "local");

  while (assert_state(ctx, START_TEST)) {
    _SMP_lock_Acquire(&lock, &lock_context);

    /* The counter value is not interesting, only the access to it */
    ++ctx->counter[test];

    _SMP_lock_Release(&lock, &lock_context);
    ++counter;
  }

  _SMP_lock_Destroy(&lock);

  ctx->test_counter[test][cpu_self] = counter;
}

static void test_7_body(
  int test,
  global_context *ctx,
  SMP_barrier_State *bs,
  unsigned int cpu_count,
  unsigned int cpu_self
)
{
  unsigned long counter = 0;
#if defined(RTEMS_PROFILING)
  SMP_lock_Stats stats;
#endif
  SMP_MCS_lock_Control lock;
  SMP_MCS_lock_Context lock_context;

  _SMP_lock_Stats_initialize(&stats, "local");
  _SMP_MCS_lock_Initialize(&lock);

  while (assert_state(ctx, START_TEST)) {
    _SMP_MCS_lock_Acquire(&lock, &lock_context, &stats);

    /* The counter value is not interesting, only the access to it */
    ++ctx->counter[test];

    _SMP_MCS_lock_Release(&lock, &lock_context);
    ++counter;
  }

  _SMP_MCS_lock_Destroy(&lock);
  _SMP_lock_Stats_destroy(&stats);

  ctx->test_counter[test][cpu_self] = counter;
}

static void busy_section(void)
{
  int i;

  for (i = 0; i < 101; ++i) {
    RTEMS_COMPILER_MEMORY_BARRIER();
  }
}

static void test_8_body(
  int test,
  global_context *ctx,
  SMP_barrier_State *bs,
  unsigned int cpu_count,
  unsigned int cpu_self
)
{
  unsigned long counter = 0;
  SMP_lock_Context lock_context;

  while (assert_state(ctx, START_TEST)) {
    _SMP_lock_Acquire(&ctx->lock, &lock_context);
    busy_section();
    _SMP_lock_Release(&ctx->lock, &lock_context);
    ++counter;
  }

  ctx->test_counter[test][cpu_self] = counter;
}

static void test_9_body(
  int test,
  global_context *ctx,
  SMP_barrier_State *bs,
  unsigned int cpu_count,
  unsigned int cpu_self
)
{
  unsigned long counter = 0;
  SMP_MCS_lock_Context lock_context;

  while (assert_state(ctx, START_TEST)) {
    _SMP_MCS_lock_Acquire(&ctx->mcs_lock, &lock_context, &ctx->mcs_stats);
    busy_section();
    _SMP_MCS_lock_Release(&ctx->mcs_lock, &lock_context);
    ++counter;
  }

  ctx->test_counter[test][cpu_self] = counter;
}

static void test_10_body(
  int test,
  global_context *ctx,
  SMP_barrier_State *bs,
  unsigned int cpu_count,
  unsigned int cpu_self
)
{
  unsigned long counter = 0;
  unsigned long seq;

  if (cpu_self == 0) {
    while (assert_state(ctx, START_TEST)) {
      seq = _SMP_sequence_lock_Write_begin(&ctx->seq_lock);

      ctx->a = counter;
      ctx->b = counter;

      _SMP_sequence_lock_Write_end(&ctx->seq_lock, seq);

      ++counter;
    }
  } else {
    while (assert_state(ctx, START_TEST)) {
      unsigned long a;
      unsigned long b;

      do {
        seq = _SMP_sequence_lock_Read_begin(&ctx->seq_lock);

        a = ctx->a;
        b = ctx->b;

      } while (_SMP_sequence_lock_Read_retry(&ctx->seq_lock, seq));

      ++counter;
      rtems_test_assert(a == b);
    }
  }

  ctx->test_counter[test][cpu_self] = counter;
}

static const test_body test_bodies[TEST_COUNT] = {
  test_0_body,
  test_1_body,
  test_2_body,
  test_3_body,
  test_4_body,
  test_5_body,
  test_6_body,
  test_7_body,
  test_8_body,
  test_9_body,
  test_10_body
};

static void run_tests(
  global_context *ctx,
  SMP_barrier_State *bs,
  unsigned int cpu_count,
  unsigned int cpu_self,
  bool master
)
{
  int test;

  for (test = 0; test < TEST_COUNT; ++test) {
    _SMP_barrier_Wait(&ctx->barrier, bs, cpu_count);

    if (master) {
      rtems_status_code sc = rtems_timer_fire_after(
        ctx->timer_id,
        ctx->timeout,
        stop_test_timer,
        ctx
      );
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      _Atomic_Store_uint(&ctx->state, START_TEST, ATOMIC_ORDER_RELEASE);
    }

    wait_for_state(ctx, START_TEST);

    (*test_bodies[test])(test, ctx, bs, cpu_count, cpu_self);
  }

  _SMP_barrier_Wait(&ctx->barrier, bs, cpu_count);
}

static void task(rtems_task_argument arg)
{
  global_context *ctx = (global_context *) arg;
  uint32_t cpu_count = rtems_get_processor_count();
  uint32_t cpu_self = rtems_get_current_processor();
  rtems_status_code sc;
  SMP_barrier_State bs = SMP_BARRIER_STATE_INITIALIZER;

  run_tests(ctx, &bs, cpu_count, cpu_self, false);

  sc = rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test(void)
{
  global_context *ctx = &context;
  uint32_t cpu_count = rtems_get_processor_count();
  uint32_t cpu_self = rtems_get_current_processor();
  uint32_t cpu;
  int test;
  rtems_status_code sc;
  SMP_barrier_State bs = SMP_BARRIER_STATE_INITIALIZER;

  for (cpu = 0; cpu < cpu_count; ++cpu) {
    if (cpu != cpu_self) {
      rtems_id task_id;

      sc = rtems_task_create(
        rtems_build_name('T', 'A', 'S', 'K'),
        TASK_PRIORITY,
        RTEMS_MINIMUM_STACK_SIZE,
        RTEMS_DEFAULT_MODES,
        RTEMS_DEFAULT_ATTRIBUTES,
        &task_id
      );
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      sc = rtems_task_start(task_id, task, (rtems_task_argument) ctx);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    }
  }

  ctx->timeout = 5 * rtems_clock_get_ticks_per_second();

  sc = rtems_timer_create(rtems_build_name('T', 'I', 'M', 'R'), &ctx->timer_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  run_tests(ctx, &bs, cpu_count, cpu_self, true);

  for (test = 0; test < TEST_COUNT; ++test) {
    unsigned long sum = 0;

    printf("%s\n", test_names[test]);

    for (cpu = 0; cpu < cpu_count; ++cpu) {
      unsigned long local_counter = ctx->test_counter[test][cpu];

      sum += local_counter;

      printf(
        "\tprocessor %" PRIu32 ", local counter %lu\n",
        cpu,
        local_counter
      );
    }

    printf(
      "\tglobal counter %lu, sum of local counter %lu\n",
      ctx->counter[test],
      sum
    );
  }
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_SMP_APPLICATION

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_MAXIMUM_TASKS CPU_COUNT

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_INIT_TASK_PRIORITY TASK_PRIORITY
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
