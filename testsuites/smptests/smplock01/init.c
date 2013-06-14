/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems.h>

#include "tmacros.h"

/* FIXME: Use C11 for atomic operations */

static void atomic_store(int *addr, int value)
{
  *addr = value;

  RTEMS_COMPILER_MEMORY_BARRIER();
}

static unsigned int atomic_load(const int *addr)
{
  RTEMS_COMPILER_MEMORY_BARRIER();

  return *addr;
}

/* FIXME: Add barrier to Score */

typedef struct {
	int value;
	int sense;
  SMP_lock_Control lock;
} barrier_control;

typedef struct {
	int sense;
} barrier_state;

#define BARRIER_CONTROL_INITIALIZER { 0, 0, SMP_LOCK_INITIALIZER }

#define BARRIER_STATE_INITIALIZER { 0 }

static void barrier_wait(
  barrier_control *control,
  barrier_state *state,
  int cpu_count
)
{
  int sense = ~state->sense;
  int value;

  state->sense = sense;

  _SMP_lock_Acquire(&control->lock);
  value = control->value;
  ++value;
  control->value = value;
  _SMP_lock_Release(&control->lock);

  if (value == cpu_count) {
    atomic_store(&control->value, 0);
    atomic_store(&control->sense, sense);
  }

  while (atomic_load(&control->sense) != sense) {
    /* Wait */
  }
}

#define TASK_PRIORITY 1

#define CPU_COUNT 32

#define TEST_COUNT 5

typedef enum {
  INITIAL,
  START_TEST,
  STOP_TEST
} states;

typedef struct {
  int state;
  barrier_control barrier;
  rtems_id timer_id;
  rtems_interval timeout;
  unsigned long counter[TEST_COUNT];
  unsigned long test_counter[TEST_COUNT][CPU_COUNT];
  SMP_lock_Control lock;
} global_context;

static global_context context = {
  .state = INITIAL,
  .barrier = BARRIER_CONTROL_INITIALIZER,
  .lock = SMP_LOCK_INITIALIZER
};

static const char *test_names[TEST_COUNT] = {
  "aquire global lock with local counter",
  "aquire global lock with global counter",
  "aquire local lock with local counter",
  "aquire local lock with global counter",
  "aquire global lock with busy section"
};

static void stop_test_timer(rtems_id timer_id, void *arg)
{
  global_context *ctx = arg;

  atomic_store(&ctx->state, STOP_TEST);
}

static void wait_for_state(global_context *ctx, int desired_state)
{
  while (atomic_load(&ctx->state) != desired_state) {
    /* Wait */
  }
}

static bool assert_state(global_context *ctx, int desired_state)
{
  return atomic_load(&ctx->state) == desired_state;
}

typedef void (*test_body)(
  int test,
  global_context *ctx,
  barrier_state *bs,
  int cpu_count,
  int cpu_self
);

static void test_0_body(
  int test,
  global_context *ctx,
  barrier_state *bs,
  int cpu_count,
  int cpu_self
)
{
  unsigned long counter = 0;

  while (assert_state(ctx, START_TEST)) {
    _SMP_lock_Acquire(&ctx->lock);
    _SMP_lock_Release(&ctx->lock);
    ++counter;
  }

  ctx->test_counter[test][cpu_self] = counter;
}

static void test_1_body(
  int test,
  global_context *ctx,
  barrier_state *bs,
  int cpu_count,
  int cpu_self
)
{
  unsigned long counter = 0;

  while (assert_state(ctx, START_TEST)) {
    _SMP_lock_Acquire(&ctx->lock);
    ++ctx->counter[test];
    _SMP_lock_Release(&ctx->lock);
    ++counter;
  }

  ctx->test_counter[test][cpu_self] = counter;
}

static void test_2_body(
  int test,
  global_context *ctx,
  barrier_state *bs,
  int cpu_count,
  int cpu_self
)
{
  unsigned long counter = 0;
  SMP_lock_Control lock = SMP_LOCK_INITIALIZER;

  while (assert_state(ctx, START_TEST)) {
    _SMP_lock_Acquire(&lock);
    _SMP_lock_Release(&lock);
    ++counter;
  }

  ctx->test_counter[test][cpu_self] = counter;
}

static void test_3_body(
  int test,
  global_context *ctx,
  barrier_state *bs,
  int cpu_count,
  int cpu_self
)
{
  unsigned long counter = 0;
  SMP_lock_Control lock = SMP_LOCK_INITIALIZER;

  while (assert_state(ctx, START_TEST)) {
    _SMP_lock_Acquire(&lock);

    /* The counter value is not interesting, only the access to it */
    ++ctx->counter[test];

    _SMP_lock_Release(&lock);
    ++counter;
  }

  ctx->test_counter[test][cpu_self] = counter;
}

static void busy_section(void)
{
  int i;

  for (i = 0; i < 101; ++i) {
    RTEMS_COMPILER_MEMORY_BARRIER();
  }
}

static void test_4_body(
  int test,
  global_context *ctx,
  barrier_state *bs,
  int cpu_count,
  int cpu_self
)
{
  unsigned long counter = 0;

  while (assert_state(ctx, START_TEST)) {
    _SMP_lock_Acquire(&ctx->lock);
    busy_section();
    _SMP_lock_Release(&ctx->lock);
    ++counter;
  }

  ctx->test_counter[test][cpu_self] = counter;
}

static const test_body test_bodies[TEST_COUNT] = {
  test_0_body,
  test_1_body,
  test_2_body,
  test_3_body,
  test_4_body
};

static void run_tests(
  global_context *ctx,
  barrier_state *bs,
  int cpu_count,
  int cpu_self,
  bool master
)
{
  int test;

  for (test = 0; test < TEST_COUNT; ++test) {
    barrier_wait(&ctx->barrier, bs, cpu_count);

    if (master) {
      rtems_status_code sc = rtems_timer_fire_after(
        ctx->timer_id,
        ctx->timeout,
        stop_test_timer,
        ctx
      );
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      atomic_store(&ctx->state, START_TEST);
    }

    wait_for_state(ctx, START_TEST);

    (*test_bodies[test])(test, ctx, bs, cpu_count, cpu_self);
  }

  barrier_wait(&ctx->barrier, bs, cpu_count);
}

static void task(rtems_task_argument arg)
{
  global_context *ctx = (global_context *) arg;
  int cpu_count = (int) rtems_smp_get_processor_count();
  int cpu_self = rtems_smp_get_current_processor();
  rtems_status_code sc;
  barrier_state bs = BARRIER_STATE_INITIALIZER;

  run_tests(ctx, &bs, cpu_count, cpu_self, false);

  sc = rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test(void)
{
  global_context *ctx = &context;
  int cpu_count = (int) rtems_smp_get_processor_count();
  int cpu_self = rtems_smp_get_current_processor();
  int cpu;
  int test;
  rtems_status_code sc;
  barrier_state bs = BARRIER_STATE_INITIALIZER;

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

  ctx->timeout = 10 * rtems_clock_get_ticks_per_second();

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
        "\tprocessor %i, local counter %lu\n",
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
  puts("\n\n*** TEST SMPLOCK 1 ***");

  test();

  puts("*** END OF TEST SMPLOCK 1 ***");

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

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
