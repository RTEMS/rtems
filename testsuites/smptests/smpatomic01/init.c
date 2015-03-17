/*
 * Copyright (c) 2013-2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * Copyright (c) 2013 Deng Hengyi.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/atomic.h>
#include <rtems.h>
#include <rtems/test.h>
#include <limits.h>
#include <string.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPATOMIC 1";

#define MASTER_PRIORITY 1

#define WORKER_PRIORITY 2

#define CPU_COUNT 32

typedef struct {
  rtems_test_parallel_context base;
  Atomic_Uint atomic_int_value;
  Atomic_Ulong atomic_value;
  unsigned long per_worker_value[CPU_COUNT];
  unsigned long normal_value;
  char unused_space_for_cache_line_separation[128];
  unsigned long second_value;
  Atomic_Flag global_flag;
} smpatomic01_context;

static smpatomic01_context test_instance;

static rtems_interval test_duration(void)
{
  return rtems_clock_get_ticks_per_second();
}

static void test_fini(
  smpatomic01_context *ctx,
  const char *test,
  bool atomic
)
{
  unsigned long expected_value = 0;
  unsigned long actual_value;
  size_t worker_index;

  printf("=== atomic %s test case ===\n", test);

  for (
    worker_index = 0;
    worker_index < ctx->base.worker_count;
    ++worker_index
  ) {
    unsigned long worker_value = ctx->per_worker_value[worker_index];

    expected_value += worker_value;

    printf(
      "worker %zu value: %lu\n",
      worker_index,
      worker_value
    );
  }

  if (atomic) {
    actual_value = _Atomic_Load_ulong(&ctx->atomic_value, ATOMIC_ORDER_RELAXED);
  } else {
    actual_value = ctx->normal_value;
  }

  printf(
    "atomic value: expected = %lu, actual = %lu\n",
    expected_value,
    actual_value
  );

  rtems_test_assert(expected_value == actual_value);
}


static rtems_interval test_atomic_add_init(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;

  _Atomic_Init_ulong(&ctx->atomic_value, 0);

  return test_duration();
}

static void test_atomic_add_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;
  unsigned long counter = 0;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    ++counter;
    _Atomic_Fetch_add_ulong(&ctx->atomic_value, 1, ATOMIC_ORDER_RELAXED);
  }

  ctx->per_worker_value[worker_index] = counter;
}

static void test_atomic_add_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;

  test_fini(ctx, "add", true);
}

static rtems_interval test_atomic_flag_init(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;

  _Atomic_Flag_clear(&ctx->global_flag, ATOMIC_ORDER_RELEASE);
  ctx->normal_value = 0;

  return test_duration();
}

static void test_atomic_flag_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;
  unsigned long counter = 0;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    while (_Atomic_Flag_test_and_set(&ctx->global_flag, ATOMIC_ORDER_ACQUIRE)) {
      /* Wait */
    }

    ++counter;
    ++ctx->normal_value;

    _Atomic_Flag_clear(&ctx->global_flag, ATOMIC_ORDER_RELEASE);
  }

  ctx->per_worker_value[worker_index] = counter;
}

static void test_atomic_flag_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
  )
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;

  test_fini(ctx, "flag", false);
}

static rtems_interval test_atomic_sub_init(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;

  _Atomic_Init_ulong(&ctx->atomic_value, 0);

  return test_duration();
}

static void test_atomic_sub_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;
  unsigned long counter = 0;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    --counter;
    _Atomic_Fetch_sub_ulong(&ctx->atomic_value, 1, ATOMIC_ORDER_RELAXED);
  }

  ctx->per_worker_value[worker_index] = counter;
}

static void test_atomic_sub_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;

  test_fini(ctx, "sub", true);
}

static rtems_interval test_atomic_compare_exchange_init(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;

  _Atomic_Init_ulong(&ctx->atomic_value, 0);
  ctx->normal_value = 0;

  return test_duration();
}

static void test_atomic_compare_exchange_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;
  unsigned long counter = 0;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    bool success;

    do {
      unsigned long zero = 0;

      success = _Atomic_Compare_exchange_ulong(
        &ctx->atomic_value,
        &zero,
        1,
        ATOMIC_ORDER_ACQUIRE,
        ATOMIC_ORDER_RELAXED
      );
    } while (!success);

    ++counter;
    ++ctx->normal_value;

    _Atomic_Store_ulong(&ctx->atomic_value, 0, ATOMIC_ORDER_RELEASE);
  }

  ctx->per_worker_value[worker_index] = counter;
}

static void test_atomic_compare_exchange_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;

  test_fini(ctx, "compare exchange", false);
}

static rtems_interval test_atomic_or_and_init(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;

  _Atomic_Init_ulong(&ctx->atomic_value, 0);

  return test_duration();
}

static void test_atomic_or_and_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;
  unsigned long the_bit = 1UL << worker_index;
  unsigned long current_bit = 0;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    unsigned long previous;

    if (current_bit != 0) {
      previous = _Atomic_Fetch_and_ulong(
        &ctx->atomic_value,
        ~the_bit,
        ATOMIC_ORDER_RELAXED
      );
      current_bit = 0;
    } else {
      previous = _Atomic_Fetch_or_ulong(
        &ctx->atomic_value,
        the_bit,
        ATOMIC_ORDER_RELAXED
      );
      current_bit = the_bit;
    }

    rtems_test_assert((previous & the_bit) != current_bit);
  }

  ctx->per_worker_value[worker_index] = current_bit;
}

static void test_atomic_or_and_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;

  test_fini(ctx, "or/and", true);
}

static rtems_interval test_atomic_fence_init(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;

  ctx->normal_value = 0;
  ctx->second_value = 0;
  _Atomic_Fence(ATOMIC_ORDER_RELEASE);

  return test_duration();
}

static void test_atomic_fence_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;

  if (rtems_test_parallel_is_master_worker(worker_index)) {
    unsigned long counter = 0;

    while (!rtems_test_parallel_stop_job(&ctx->base)) {
      ++counter;
      ctx->normal_value = counter;
      _Atomic_Fence(ATOMIC_ORDER_RELEASE);
      ctx->second_value = counter;
    }
  } else {
    while (!rtems_test_parallel_stop_job(&ctx->base)) {
      unsigned long n;
      unsigned long s;

      s = ctx->second_value;
      _Atomic_Fence(ATOMIC_ORDER_ACQUIRE);
      n = ctx->normal_value;

      rtems_test_assert(n - s < LONG_MAX);
    }
  }
}

static void test_atomic_fence_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;

  printf(
    "=== atomic fence test case ===\n"
    "normal value = %lu, second value = %lu\n",
    ctx->normal_value,
    ctx->second_value
  );
}

static const rtems_test_parallel_job test_jobs[] = {
  {
    .init = test_atomic_add_init,
    .body = test_atomic_add_body,
    .fini = test_atomic_add_fini
  }, {
    .init = test_atomic_flag_init,
    .body = test_atomic_flag_body,
    .fini = test_atomic_flag_fini
  }, {
    .init = test_atomic_sub_init,
    .body = test_atomic_sub_body,
    .fini = test_atomic_sub_fini
  }, {
    .init = test_atomic_compare_exchange_init,
    .body = test_atomic_compare_exchange_body,
    .fini = test_atomic_compare_exchange_fini
  }, {
    .init = test_atomic_or_and_init,
    .body = test_atomic_or_and_body,
    .fini = test_atomic_or_and_fini
  }, {
    .init = test_atomic_fence_init,
    .body = test_atomic_fence_body,
    .fini = test_atomic_fence_fini
  },
};

static void setup_worker(
  rtems_test_parallel_context *base,
  size_t worker_index,
  rtems_id worker_id
)
{
  rtems_status_code sc;
  rtems_task_priority prio;

  sc = rtems_task_set_priority(worker_id, WORKER_PRIORITY, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void Init(rtems_task_argument arg)
{
  smpatomic01_context *ctx = &test_instance;

  TEST_BEGIN();

  rtems_test_parallel(
    &ctx->base,
    setup_worker,
    &test_jobs[0],
    RTEMS_ARRAY_SIZE(test_jobs)
  );

  TEST_END();
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

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
