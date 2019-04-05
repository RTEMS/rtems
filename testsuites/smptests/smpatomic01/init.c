/*
 * Copyright (c) 2013, 2016 embedded brains GmbH.  All rights reserved.
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
#include <rtems/score/smpbarrier.h>
#include <rtems.h>
#include <rtems/bsd.h>
#include <rtems/test.h>
#include <limits.h>
#include <string.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPATOMIC 1";

#define MS_PER_TICK 10

#define MASTER_PRIORITY 1

#define WORKER_PRIORITY 2

#define CPU_COUNT 32

typedef struct {
  rtems_test_parallel_context base;
  Atomic_Ulong atomic_value;
  unsigned long per_worker_value[CPU_COUNT];
  unsigned long normal_value;
  char unused_space_for_cache_line_separation[128];
  unsigned long second_value;
  Atomic_Flag global_flag;
  SMP_barrier_Control barrier;
  SMP_barrier_State barrier_state[CPU_COUNT];
  sbintime_t load_trigger_time;
  sbintime_t load_change_time[CPU_COUNT];
  int load_count[CPU_COUNT];
  sbintime_t rmw_trigger_time;
  sbintime_t rmw_change_time[CPU_COUNT];
  int rmw_count[CPU_COUNT];
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

static rtems_interval test_atomic_store_load_rmw_init(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;
  size_t i;

  _Atomic_Init_ulong(&ctx->atomic_value, 0);

  _SMP_barrier_Control_initialize(&ctx->barrier);

  for (i = 0; i < active_workers; ++i) {
    _SMP_barrier_State_initialize(&ctx->barrier_state[i]);
  }

  return 0;
}

static sbintime_t now(void)
{
  struct bintime bt;

  rtems_bsd_binuptime(&bt);
  return bttosbt(bt);
}

static void test_atomic_store_load_rmw_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;
  uint32_t cpu_self_index;
  sbintime_t t;
  int counter;

  if (rtems_test_parallel_is_master_worker(worker_index)) {
    rtems_status_code sc;

    sc = rtems_task_wake_after(1);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    t = now();
    t += (MS_PER_TICK / 2) * SBT_1MS;
    ctx->load_trigger_time = t;
    t += MS_PER_TICK * SBT_1MS;
    ctx->rmw_trigger_time = t;
  }

  _Atomic_Fence(ATOMIC_ORDER_SEQ_CST);

  _SMP_barrier_Wait(
    &ctx->barrier,
    &ctx->barrier_state[worker_index],
    active_workers
  );

  /*
   * Use the physical processor index, to observe timing differences introduced
   * by the system topology.
   */
  cpu_self_index = rtems_scheduler_get_processor();

  /* Store release and load acquire test case */

  counter = 0;
  t = ctx->load_trigger_time;

  while (now() < t) {
    /* Wait */
  }

  if (cpu_self_index == 0) {
    _Atomic_Store_ulong(&ctx->atomic_value, 1, ATOMIC_ORDER_RELEASE);
  } else {
    while (_Atomic_Load_ulong(&ctx->atomic_value, ATOMIC_ORDER_ACQUIRE) == 0) {
      ++counter;
    }
  }

  ctx->load_change_time[cpu_self_index] = now();
  ctx->load_count[cpu_self_index] = counter;

  /* Read-modify-write test case */

  if (cpu_self_index == 0) {
    _Atomic_Store_ulong(&ctx->atomic_value, 0, ATOMIC_ORDER_RELAXED);
  }

  counter = 0;
  t = ctx->rmw_trigger_time;

  while (now() < t) {
    /* Wait */
  }

  if (cpu_self_index == 0) {
    _Atomic_Store_ulong(&ctx->atomic_value, 1, ATOMIC_ORDER_RELAXED);
  } else {
    while (
      (_Atomic_Fetch_or_ulong(&ctx->atomic_value, 2, ATOMIC_ORDER_RELAXED) & 1)
        == 0
    ) {
      ++counter;
    }
  }

  ctx->rmw_change_time[cpu_self_index] = now();
  ctx->rmw_count[cpu_self_index] = counter;
}

static void test_atomic_store_load_rmw_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;
  size_t i;
  struct bintime bt;
  struct timespec ts;

  printf("=== atomic store release and load acquire test case ===\n");

  for (i = 0; i < active_workers; ++i) {
    bt = sbttobt(ctx->load_change_time[i] - ctx->load_trigger_time);
    bintime2timespec(&bt, &ts);
    printf(
      "processor %zu delta %lins, load count %i\n",
      i,
      ts.tv_nsec,
      ctx->load_count[i]
    );
  }

  printf("=== atomic read-modify-write test case ===\n");

  for (i = 0; i < active_workers; ++i) {
    bt = sbttobt(ctx->rmw_change_time[i] - ctx->rmw_trigger_time);
    bintime2timespec(&bt, &ts);
    printf(
      "processor %zu delta %lins, read-modify-write count %i\n",
      i,
      ts.tv_nsec,
      ctx->rmw_count[i]
    );
  }
}

/*
 * See also Hans-J. Boehm, HP Laboratories,
 * "Can Seqlocks Get Along With Programming Language Memory Models?",
 * http://www.hpl.hp.com/techreports/2012/HPL-2012-68.pdf
 */

static rtems_interval test_seqlock_init(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;

  ctx->normal_value = 0;
  ctx->second_value = 0;
  _Atomic_Store_ulong(&ctx->atomic_value, 0, ATOMIC_ORDER_RELEASE);

  return test_duration();
}

static unsigned long seqlock_read(smpatomic01_context *ctx)
{
  unsigned long counter = 0;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    unsigned long seq0;
    unsigned long seq1;
    unsigned long a;
    unsigned long b;

    do {
      seq0 = _Atomic_Load_ulong(&ctx->atomic_value, ATOMIC_ORDER_ACQUIRE);

      a = ctx->normal_value;
      b = ctx->second_value;

      seq1 =
        _Atomic_Fetch_add_ulong(&ctx->atomic_value, 0, ATOMIC_ORDER_RELEASE);
    } while (seq0 != seq1 || seq0 % 2 != 0);

    ++counter;
    rtems_test_assert(a == b);
  }

  return counter;
}

static void test_single_writer_seqlock_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;
  uint32_t cpu_self_index;
  unsigned long counter;

  /*
   * Use the physical processor index, to observe timing differences introduced
   * by the system topology.
   */
  cpu_self_index = rtems_scheduler_get_processor();

  if (cpu_self_index == 0) {
    counter = 0;

    while (!rtems_test_parallel_stop_job(&ctx->base)) {
      unsigned long seq;

      seq = _Atomic_Load_ulong(&ctx->atomic_value, ATOMIC_ORDER_RELAXED);
      _Atomic_Store_ulong(&ctx->atomic_value, seq + 1, ATOMIC_ORDER_RELAXED);

      /* There is no atomic store with acquire/release semantics */
      _Atomic_Fence(ATOMIC_ORDER_ACQ_REL);

      ++counter;
      ctx->normal_value = counter;
      ctx->second_value = counter;

      _Atomic_Store_ulong(&ctx->atomic_value, seq + 2, ATOMIC_ORDER_RELEASE);
    }
  } else {
    counter = seqlock_read(ctx);
  }

  ctx->per_worker_value[cpu_self_index] = counter;
}

static void test_single_writer_seqlock_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;
  size_t i;

  printf("=== single writer seqlock test case ===\n");

  for (i = 0; i < active_workers; ++i) {
    printf(
      "processor %zu count %lu\n",
      i,
      ctx->per_worker_value[i]
    );
  }
}

static void test_multi_writer_seqlock_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;
  uint32_t cpu_self_index;
  unsigned long counter;

  /*
   * Use the physical processor index, to observe timing differences introduced
   * by the system topology.
   */
  cpu_self_index = rtems_scheduler_get_processor();

  if (cpu_self_index % 2 == 0) {
    counter = 0;

    while (!rtems_test_parallel_stop_job(&ctx->base)) {
      unsigned long seq;

      do {
        seq = _Atomic_Load_ulong(&ctx->atomic_value, ATOMIC_ORDER_RELAXED);
      } while (
        seq % 2 != 0
          || !_Atomic_Compare_exchange_ulong(
              &ctx->atomic_value,
              &seq,
              seq + 1,
              ATOMIC_ORDER_ACQ_REL,
              ATOMIC_ORDER_RELAXED
            )
      );

      ++counter;
      ctx->normal_value = counter;
      ctx->second_value = counter;

      _Atomic_Store_ulong(&ctx->atomic_value, seq + 2, ATOMIC_ORDER_RELEASE);
    }
  } else {
    counter = seqlock_read(ctx);
  }

  ctx->per_worker_value[cpu_self_index] = counter;
}

static void test_multi_writer_seqlock_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  smpatomic01_context *ctx = (smpatomic01_context *) base;
  size_t i;

  printf("=== multi writer seqlock test case ===\n");

  for (i = 0; i < active_workers; ++i) {
    printf(
      "processor %zu count %lu\n",
      i,
      ctx->per_worker_value[i]
    );
  }
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
  }, {
    .init = test_atomic_store_load_rmw_init,
    .body = test_atomic_store_load_rmw_body,
    .fini = test_atomic_store_load_rmw_fini
  }, {
    .init = test_seqlock_init,
    .body = test_single_writer_seqlock_body,
    .fini = test_single_writer_seqlock_fini
  }, {
    .init = test_seqlock_init,
    .body = test_multi_writer_seqlock_body,
    .fini = test_multi_writer_seqlock_fini
  }
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
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK (MS_PER_TICK * 1000)

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_MAXIMUM_TASKS CPU_COUNT

#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_INIT_TASK_PRIORITY MASTER_PRIORITY
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
