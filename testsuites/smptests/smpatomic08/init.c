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

#include <rtems/score/atomic.h>
#include <rtems.h>

#include "tmacros.h"

/* FIXME: Add barrier to Score */

typedef struct {
	Atomic_Uint value;
	Atomic_Uint sense;
} SMP_barrier_Control;

typedef struct {
	uint_fast32_t sense;
} SMP_barrier_State;

#define SMP_BARRIER_CONTROL_INITIALIZER \
  { ATOMIC_INITIALIZER_UINT( 0 ), ATOMIC_INITIALIZER_UINT( 0 ) }

#define SMP_BARRIER_STATE_INITIALIZER { 0 }

static void _SMP_barrier_Wait(
  SMP_barrier_Control *control,
  SMP_barrier_State *state,
  uint_fast32_t count
)
{
  uint_fast32_t sense = ~state->sense;
  uint_fast32_t previous_value;

  state->sense = sense;

  previous_value = _Atomic_Fetch_add_uint(
    &control->value,
    1,
    ATOMIC_ORDER_RELAXED
  );

  if ( previous_value + 1 == count ) {
    _Atomic_Store_uint( &control->value, 0, ATOMIC_ORDER_RELAXED );
    _Atomic_Store_uint( &control->sense, sense, ATOMIC_ORDER_RELEASE );
  } else {
    while (
      _Atomic_Load_uint( &control->sense, ATOMIC_ORDER_ACQUIRE ) != sense
    ) {
      /* Wait */
    }
  }
}

#define MASTER_PRIORITY 1

#define WORKER_PRIORITY 2

#define CPU_COUNT 32

typedef struct {
  Atomic_Uint stop;
  SMP_barrier_Control barrier;
  size_t worker_count;
  rtems_id stop_worker_timer_id;
  Atomic_Uint global_uint;
  Atomic_Uint global_swap;
  uint_fast32_t global_swap_t;
  uint_fast32_t per_worker_uint[CPU_COUNT];
  uint32_t flag_counter;
  Atomic_Flag global_flag;
} test_context;

typedef struct {
  void (*init)(test_context *ctx);
  void (*body)(test_context *ctx, size_t worker_index);
  void (*fini)(test_context *ctx);
} test_case;

static test_context test_instance = {
  .stop = ATOMIC_INITIALIZER_UINT(0),
  .barrier = SMP_BARRIER_CONTROL_INITIALIZER
};

static bool stop(test_context *ctx)
{
  return _Atomic_Load_uint(&ctx->stop, ATOMIC_ORDER_RELAXED) != 0;
}

static bool is_master_worker(size_t worker_index)
{
  return worker_index == 0;
}

static void test_atomic_add_init(test_context *ctx)
{
  _Atomic_Init_uint(&ctx->global_uint, 0);
}

static void test_atomic_add_body(test_context *ctx, size_t worker_index)
{
  uint_fast32_t counter = 0;

  while (!stop(ctx)) {
    ++counter;
    _Atomic_Fetch_add_uint(&ctx->global_uint, 1, ATOMIC_ORDER_RELAXED);
  }

  ctx->per_worker_uint[worker_index] = counter;
}

static void test_atomic_add_fini(test_context *ctx)
{
  uint_fast32_t expected_counter = 0;
  uint_fast32_t actual_counter;
  size_t worker_index;

  printf("=== atomic add test case ==\n");

  for (worker_index = 0; worker_index < ctx->worker_count; ++worker_index) {
    uint_fast32_t worker_counter = ctx->per_worker_uint[worker_index];

    expected_counter += worker_counter;

    printf(
      "atomic add worker %zu counter: %" PRIuFAST32 "\n",
      worker_index,
      worker_counter
    );
  }

  actual_counter = _Atomic_Load_uint(&ctx->global_uint, ATOMIC_ORDER_RELAXED);

  printf(
    "global counter: expected = %" PRIuFAST32 ", actual = %" PRIuFAST32 "\n",
    expected_counter,
    actual_counter
  );

  rtems_test_assert(expected_counter == actual_counter);
}

static void test_atomic_flag_init(test_context *ctx)
{
  _Atomic_Flag_clear(&ctx->global_flag, ATOMIC_ORDER_RELEASE);
  ctx->flag_counter = 0;
}

static void test_atomic_flag_body(test_context *ctx, size_t worker_index)
{
  uint_fast32_t counter = 0;

  while (!stop(ctx)) {
    while (_Atomic_Flag_test_and_set(&ctx->global_flag, ATOMIC_ORDER_ACQUIRE)) {
      /* Wait */
    }

    ++counter;
    ++ctx->flag_counter;

    _Atomic_Flag_clear(&ctx->global_flag, ATOMIC_ORDER_RELEASE);
  }

  ctx->per_worker_uint[worker_index] = counter;
}

static void test_atomic_flag_fini(test_context *ctx)
{
  uint_fast32_t expected_counter = 0;
  uint_fast32_t actual_counter;
  size_t worker_index;

  printf("=== atomic flag test case ===\n");

  for (worker_index = 0; worker_index < ctx->worker_count; ++worker_index) {
    uint_fast32_t worker_counter = ctx->per_worker_uint[worker_index];

    expected_counter += worker_counter;

    printf(
      "atomic flag worker %zu counter: %" PRIuFAST32 "\n",
      worker_index,
      worker_counter
    );
  }

  actual_counter = ctx->flag_counter;

  printf(
    "global flag counter: expected = %" PRIuFAST32 ", actual = %" PRIuFAST32 "\n",
    expected_counter,
    actual_counter
  );

  rtems_test_assert(expected_counter == actual_counter);
}

static void test_atomic_sub_init(test_context *ctx)
{
  _Atomic_Init_uint(&ctx->global_uint, 0xffffffff);
}

static void test_atomic_sub_body(test_context *ctx, size_t worker_index)
{
  uint_fast32_t counter = 0xffffffff;

  while (!stop(ctx)) {
    --counter;
    _Atomic_Fetch_sub_uint(&ctx->global_uint, 1, ATOMIC_ORDER_RELAXED);
  }

  ctx->per_worker_uint[worker_index] = 0xffffffff - counter;
}

static void test_atomic_sub_fini(test_context *ctx)
{
  uint_fast32_t expected_counter = 0;
  uint_fast32_t actual_counter;
  size_t worker_index;

  printf("=== atomic sub test case ==\n");

  for (worker_index = 0; worker_index < ctx->worker_count; ++worker_index) {
    uint_fast32_t worker_counter = ctx->per_worker_uint[worker_index];

    expected_counter += worker_counter;

    printf(
      "atomic sub worker %zu counter: %" PRIuFAST32 "\n",
      worker_index,
      worker_counter
    );
  }

  actual_counter = _Atomic_Load_uint(&ctx->global_uint, ATOMIC_ORDER_RELAXED);
  actual_counter = 0xffffffff - actual_counter;

  printf(
    "global counter: expected = %" PRIuFAST32 ", actual = %" PRIuFAST32 "\n",
    expected_counter,
    actual_counter
  );

  rtems_test_assert(expected_counter == actual_counter);
}

static void test_atomic_compare_exchange_init(test_context *ctx)
{
  _Atomic_Init_uint(&ctx->global_swap, 0xffffffff);
  ctx->global_swap_t = 0xffffffff;
  ctx->flag_counter = 0;
}

static void test_atomic_compare_exchange_body(test_context *ctx, size_t worker_index)
{
  uint_fast32_t counter = 0;

  while (!stop(ctx)) {
    while (_Atomic_Compare_exchange_uint(&ctx->global_swap, &ctx->global_swap_t,
      worker_index, ATOMIC_ORDER_ACQUIRE, ATOMIC_ORDER_RELAXED)) {
      /* Wait */
    }
    ++counter;
    ++ctx->flag_counter;
    _Atomic_Store_uint(&ctx->global_swap, 0, ATOMIC_ORDER_RELEASE);
  }

  ctx->per_worker_uint[worker_index] = counter;
}

static void test_atomic_compare_exchange_fini(test_context *ctx)
{
  uint_fast32_t expected_counter = 0;
  uint_fast32_t actual_counter;
  size_t worker_index;

  printf("=== atomic compare_exchange test case ==\n");

  for (worker_index = 0; worker_index < ctx->worker_count; ++worker_index) {
    uint_fast32_t worker_counter = ctx->per_worker_uint[worker_index];

    expected_counter += worker_counter;

    printf(
      "atomic compare_exchange worker %zu counter: %" PRIuFAST32 "\n",
      worker_index,
      worker_counter
    );
  }

  actual_counter = ctx->flag_counter;

  printf(
    "global counter: expected = %" PRIuFAST32 ", actual = %" PRIuFAST32 "\n",
    expected_counter,
    actual_counter
  );

  rtems_test_assert(expected_counter == actual_counter);
}

static void test_atomic_or_and_init(test_context *ctx)
{
  _Atomic_Init_uint(&ctx->global_uint, 0);
}

static void test_atomic_or_and_body(test_context *ctx, size_t worker_index)
{
  uint_fast32_t counter = 0;

  while (!stop(ctx)) {
    _Atomic_Fetch_or_uint(&ctx->global_uint, (1 << worker_index), ATOMIC_ORDER_RELAXED);
    counter = 1;
    if (!stop(ctx))
      break;
    _Atomic_Fetch_and_uint(&ctx->global_uint, ~(1 << worker_index), ATOMIC_ORDER_RELAXED);
    counter = 0;
  }

  ctx->per_worker_uint[worker_index] = counter;
}

static void test_atomic_or_and_fini(test_context *ctx)
{
  uint_fast32_t expected_counter = 0;
  uint_fast32_t actual_counter;
  size_t worker_index;

  printf("=== atomic or_and test case ==\n");

  for (worker_index = 0; worker_index < ctx->worker_count; ++worker_index) {
    uint_fast32_t worker_counter = ctx->per_worker_uint[worker_index];

    expected_counter |= ( worker_counter << worker_index );

    printf(
      "atomic or_and worker %zu counter: %" PRIuFAST32 "\n",
      worker_index,
      worker_counter
    );
  }

  actual_counter = _Atomic_Load_uint(&ctx->global_uint, ATOMIC_ORDER_RELAXED);

  printf(
    "global counter: expected = %" PRIuFAST32 ", actual = %" PRIuFAST32 "\n",
    expected_counter,
    actual_counter
  );

  rtems_test_assert(expected_counter == actual_counter);
}

static const test_case test_cases[] = {
  { test_atomic_add_init, test_atomic_add_body, test_atomic_add_fini },
  { test_atomic_flag_init, test_atomic_flag_body, test_atomic_flag_fini },
  { test_atomic_sub_init, test_atomic_sub_body, test_atomic_sub_fini },
  { test_atomic_compare_exchange_init, test_atomic_compare_exchange_body,
    test_atomic_compare_exchange_fini },
  { test_atomic_or_and_init, test_atomic_or_and_body, test_atomic_or_and_fini },
};

#define TEST_COUNT RTEMS_ARRAY_SIZE(test_cases)

static void stop_worker_timer(rtems_id timer_id, void *arg)
{
  test_context *ctx = arg;

  _Atomic_Store_uint(&ctx->stop, 1, ATOMIC_ORDER_RELAXED);
}

static void start_worker_stop_timer(test_context *ctx)
{
  rtems_status_code sc;

  _Atomic_Store_uint(&ctx->stop, 0, ATOMIC_ORDER_RELEASE);

  sc = rtems_timer_fire_after(
    ctx->stop_worker_timer_id,
    rtems_clock_get_ticks_per_second(),
    stop_worker_timer,
    ctx
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void run_tests(test_context *ctx, size_t worker_index)
{
  SMP_barrier_State bs = SMP_BARRIER_STATE_INITIALIZER;
  size_t test;

  for (test = 0; test < TEST_COUNT; ++test) {
    const test_case *tc = &test_cases[test];

    if (is_master_worker(worker_index)) {
      start_worker_stop_timer(ctx);
      (*tc->init)(ctx);
    }

    _SMP_barrier_Wait(&ctx->barrier, &bs, ctx->worker_count);

    (*tc->body)(ctx, worker_index);

    _SMP_barrier_Wait(&ctx->barrier, &bs, ctx->worker_count);

    if (is_master_worker(worker_index)) {
      (*tc->fini)(ctx);
    }
  }
}

static void worker_task(size_t worker_index)
{
  test_context *ctx = &test_instance;

  run_tests(ctx, worker_index);

  (void) rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(0);
}

static void test(void)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  size_t worker_index;

  ctx->worker_count = rtems_smp_get_processor_count();

  sc = rtems_timer_create(
    rtems_build_name('S', 'T', 'O', 'P'),
    &ctx->stop_worker_timer_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (worker_index = 1; worker_index < ctx->worker_count; ++worker_index) {
    rtems_id worker_id;

    sc = rtems_task_create(
      rtems_build_name('W', 'O', 'R', 'K'),
      WORKER_PRIORITY,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &worker_id
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_start(worker_id, worker_task, worker_index);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  run_tests(ctx, 0);
}

static void Init(rtems_task_argument arg)
{
  puts("\n\n*** TEST SMPATOMIC 8 ***");

  test();

  puts("*** END OF TEST SMPATOMIC 8 ***");

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_SMP_APPLICATION

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_MAXIMUM_TASKS CPU_COUNT

#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_INIT_TASK_PRIORITY MASTER_PRIORITY
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
