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
#include <rtems/test.h>
#include <rtems.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPLOCK 1";

#define TASK_PRIORITY 1

#define CPU_COUNT 32

#define TEST_COUNT 13

typedef struct {
  rtems_test_parallel_context base;
  unsigned long counter[TEST_COUNT];
  unsigned long local_counter[CPU_COUNT][TEST_COUNT][CPU_COUNT];
  SMP_lock_Control lock RTEMS_ALIGNED(CPU_CACHE_LINE_BYTES);
  Atomic_Uint flag RTEMS_ALIGNED(CPU_CACHE_LINE_BYTES);
  SMP_MCS_lock_Control mcs_lock RTEMS_ALIGNED(CPU_CACHE_LINE_BYTES);
#if defined(RTEMS_PROFILING)
  SMP_lock_Stats mcs_stats;
#endif
  SMP_sequence_lock_Control seq_lock RTEMS_ALIGNED(CPU_CACHE_LINE_BYTES);
  int a RTEMS_ALIGNED(CPU_CACHE_LINE_BYTES);
  int b RTEMS_ALIGNED(CPU_CACHE_LINE_BYTES);
} test_context;

static test_context test_instance = {
  .lock = SMP_LOCK_INITIALIZER("global ticket"),
#if defined(RTEMS_PROFILING)
  .mcs_stats = SMP_LOCK_STATS_INITIALIZER("global MCS"),
#endif
  .flag = ATOMIC_INITIALIZER_UINT(0),
  .mcs_lock = SMP_MCS_LOCK_INITIALIZER,
  .seq_lock = SMP_SEQUENCE_LOCK_INITIALIZER
};

static rtems_interval test_duration(void)
{
  return rtems_clock_get_ticks_per_second();
}

static rtems_interval test_init(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  return test_duration();
}

static void test_fini(
  test_context *ctx,
  const char *name,
  size_t test,
  size_t active_workers
)
{
  unsigned long sum = 0;
  unsigned long n = active_workers;
  unsigned long i;

  printf("  <%s activeWorker=\"%lu\">\n", name, n);

  for (i = 0; i < n; ++i) {
    unsigned long local_counter =
      ctx->local_counter[active_workers - 1][test][i];

    sum += local_counter;

    printf(
      "    <LocalCounter worker=\"%lu\">%lu</LocalCounter>\n",
      i,
      local_counter
    );
  }

  printf(
    "    <GlobalCounter>%lu</GlobalCounter>\n"
    "    <SumOfLocalCounter>%lu</SumOfLocalCounter>\n"
    "  </%s>\n",
    ctx->counter[test],
    sum,
    name
  );
}

static void test_0_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  size_t test = 0;
  unsigned long counter = 0;
  SMP_lock_Context lock_context;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    _SMP_lock_Acquire(&ctx->lock, &lock_context);
    _SMP_lock_Release(&ctx->lock, &lock_context);
    ++counter;
  }

  ctx->local_counter[active_workers - 1][test][worker_index] = counter;
}

static void test_0_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "GlobalTicketLockWithLocalCounter",
    0,
    active_workers
  );
}

static void test_1_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  size_t test = 1;
  unsigned long counter = 0;
  SMP_MCS_lock_Context lock_context;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    _SMP_MCS_lock_Acquire(&ctx->mcs_lock, &lock_context, &ctx->mcs_stats);
    _SMP_MCS_lock_Release(&ctx->mcs_lock, &lock_context);
    ++counter;
  }

  ctx->local_counter[active_workers - 1][test][worker_index] = counter;
}

static void test_1_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "GlobalMCSLockWithLocalCounter",
    1,
    active_workers
  );
}

static void test_2_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  size_t test = 2;
  unsigned long counter = 0;
  SMP_lock_Context lock_context;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    _SMP_lock_Acquire(&ctx->lock, &lock_context);
    ++ctx->counter[test];
    _SMP_lock_Release(&ctx->lock, &lock_context);
    ++counter;
  }

  ctx->local_counter[active_workers - 1][test][worker_index] = counter;
}

static void test_2_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "GlobalTicketLockWithGlobalCounter",
    2,
    active_workers
  );
}

static void test_3_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  size_t test = 3;
  unsigned long counter = 0;
  SMP_MCS_lock_Context lock_context;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    _SMP_MCS_lock_Acquire(&ctx->mcs_lock, &lock_context, &ctx->mcs_stats);
    ++ctx->counter[test];
    _SMP_MCS_lock_Release(&ctx->mcs_lock, &lock_context);
    ++counter;
  }

  ctx->local_counter[active_workers - 1][test][worker_index] = counter;
}

static void test_3_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "GlobalMCSLockWithGlobalCounter",
    3,
    active_workers
  );
}

static void test_4_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  size_t test = 4;
  unsigned long counter = 0;
  SMP_lock_Control lock;
  SMP_lock_Context lock_context;

  _SMP_lock_Initialize(&lock, "local");

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    _SMP_lock_Acquire(&lock, &lock_context);
    _SMP_lock_Release(&lock, &lock_context);
    ++counter;
  }

  _SMP_lock_Destroy(&lock);

  ctx->local_counter[active_workers - 1][test][worker_index] = counter;
}

static void test_4_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "LocalTicketLockWithLocalCounter",
    4,
    active_workers
  );
}

static void test_5_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  size_t test = 5;
  unsigned long counter = 0;
#if defined(RTEMS_PROFILING)
  SMP_lock_Stats stats;
#endif
  SMP_MCS_lock_Control lock;
  SMP_MCS_lock_Context lock_context;

  _SMP_lock_Stats_initialize(&stats, "local");
  _SMP_MCS_lock_Initialize(&lock);

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    _SMP_MCS_lock_Acquire(&lock, &lock_context, &stats);
    _SMP_MCS_lock_Release(&lock, &lock_context);
    ++counter;
  }

  _SMP_MCS_lock_Destroy(&lock);
  _SMP_lock_Stats_destroy(&stats);

  ctx->local_counter[active_workers - 1][test][worker_index] = counter;
}

static void test_5_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "LocalMCSLockWithLocalCounter",
    5,
    active_workers
  );
}

static void test_6_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  size_t test = 6;
  unsigned long counter = 0;
  SMP_lock_Control lock;
  SMP_lock_Context lock_context;

  _SMP_lock_Initialize(&lock, "local");

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    _SMP_lock_Acquire(&lock, &lock_context);

    /* The counter value is not interesting, only the access to it */
    ++ctx->counter[test];

    _SMP_lock_Release(&lock, &lock_context);
    ++counter;
  }

  _SMP_lock_Destroy(&lock);

  ctx->local_counter[active_workers - 1][test][worker_index] = counter;
}

static void test_6_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "LocalTicketLockWithGlobalCounter",
    6,
    active_workers
  );
}

static void test_7_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  size_t test = 7;
  unsigned long counter = 0;
#if defined(RTEMS_PROFILING)
  SMP_lock_Stats stats;
#endif
  SMP_MCS_lock_Control lock;
  SMP_MCS_lock_Context lock_context;

  _SMP_lock_Stats_initialize(&stats, "local");
  _SMP_MCS_lock_Initialize(&lock);

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    _SMP_MCS_lock_Acquire(&lock, &lock_context, &stats);

    /* The counter value is not interesting, only the access to it */
    ++ctx->counter[test];

    _SMP_MCS_lock_Release(&lock, &lock_context);
    ++counter;
  }

  _SMP_MCS_lock_Destroy(&lock);
  _SMP_lock_Stats_destroy(&stats);

  ctx->local_counter[active_workers - 1][test][worker_index] = counter;
}

static void test_7_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "LocalMCSLockWithGlobalCounter",
    7,
    active_workers
  );
}

static void busy_section(void)
{
  int i;

  for (i = 0; i < 101; ++i) {
    RTEMS_COMPILER_MEMORY_BARRIER();
  }
}

static void test_8_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  size_t test = 8;
  unsigned long counter = 0;
  SMP_lock_Context lock_context;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    _SMP_lock_Acquire(&ctx->lock, &lock_context);
    busy_section();
    _SMP_lock_Release(&ctx->lock, &lock_context);
    ++counter;
  }

  ctx->local_counter[active_workers - 1][test][worker_index] = counter;
}

static void test_8_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "GlobalTicketLockWithBusySection",
    8,
    active_workers
  );
}

static void test_9_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  size_t test = 9;
  unsigned long counter = 0;
  SMP_MCS_lock_Context lock_context;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    _SMP_MCS_lock_Acquire(&ctx->mcs_lock, &lock_context, &ctx->mcs_stats);
    busy_section();
    _SMP_MCS_lock_Release(&ctx->mcs_lock, &lock_context);
    ++counter;
  }

  ctx->local_counter[active_workers - 1][test][worker_index] = counter;
}

static void test_9_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "GlobalMCSLockWithBusySection",
    9,
    active_workers
  );
}

static void test_10_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  size_t test = 10;
  unsigned long counter = 0;
  unsigned long seq;

  if (rtems_test_parallel_is_master_worker(worker_index)) {
    while (!rtems_test_parallel_stop_job(&ctx->base)) {
      seq = _SMP_sequence_lock_Write_begin(&ctx->seq_lock);

      ctx->a = counter;
      ctx->b = counter;

      _SMP_sequence_lock_Write_end(&ctx->seq_lock, seq);

      ++counter;
    }
  } else {
    while (!rtems_test_parallel_stop_job(&ctx->base)) {
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

  ctx->local_counter[active_workers - 1][test][worker_index] = counter;
}

static void test_10_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "SequenceLock",
    10,
    active_workers
  );
}

static void test_11_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  size_t test = 11;
  unsigned long counter = 0;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    while (_Atomic_Exchange_uint(&ctx->flag, 1, ATOMIC_ORDER_ACQUIRE) != 0) {
      /* Wait */
    }

    _Atomic_Store_uint(&ctx->flag, 0, ATOMIC_ORDER_RELEASE);
    ++counter;
  }

  ctx->local_counter[active_workers - 1][test][worker_index] = counter;
}

static void test_11_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "GlobalTASLockWithLocalCounter",
    11,
    active_workers
  );
}

static void test_12_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  size_t test = 12;
  unsigned long counter = 0;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    while (_Atomic_Exchange_uint(&ctx->flag, 1, ATOMIC_ORDER_ACQUIRE) != 0) {
      while (_Atomic_Load_uint(&ctx->flag, ATOMIC_ORDER_RELAXED) != 0) {
        /* Wait */
      }
    }

    _Atomic_Store_uint(&ctx->flag, 0, ATOMIC_ORDER_RELEASE);
    ++counter;
  }

  ctx->local_counter[active_workers - 1][test][worker_index] = counter;
}

static void test_12_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "GlobalTTASLockWithLocalCounter",
    12,
    active_workers
  );
}

static const rtems_test_parallel_job test_jobs[TEST_COUNT] = {
  {
    .init = test_init,
    .body = test_0_body,
    .fini = test_0_fini,
    .cascade = true
  }, {
    .init = test_init,
    .body = test_1_body,
    .fini = test_1_fini,
    .cascade = true
  }, {
    .init = test_init,
    .body = test_2_body,
    .fini = test_2_fini,
    .cascade = false
  }, {
    .init = test_init,
    .body = test_3_body,
    .fini = test_3_fini,
    .cascade = false
  }, {
    .init = test_init,
    .body = test_4_body,
    .fini = test_4_fini,
    .cascade = true
  }, {
    .init = test_init,
    .body = test_5_body,
    .fini = test_5_fini,
    .cascade = true
  }, {
    .init = test_init,
    .body = test_6_body,
    .fini = test_6_fini,
    .cascade = false
  }, {
    .init = test_init,
    .body = test_7_body,
    .fini = test_7_fini,
    .cascade = false
  }, {
    .init = test_init,
    .body = test_8_body,
    .fini = test_8_fini,
    .cascade = false
  }, {
    .init = test_init,
    .body = test_9_body,
    .fini = test_9_fini,
    .cascade = false
  }, {
    .init = test_init,
    .body = test_10_body,
    .fini = test_10_fini,
    .cascade = false
  }, {
    .init = test_init,
    .body = test_11_body,
    .fini = test_11_fini,
    .cascade = true
  }, {
    .init = test_init,
    .body = test_12_body,
    .fini = test_12_fini,
    .cascade = true
  }
};

static void test(void)
{
  test_context *ctx = &test_instance;
  const char *test = "SMPLock01";

  printf("<%s>\n", test);
  rtems_test_parallel(&ctx->base, NULL, &test_jobs[0], TEST_COUNT);
  printf("</%s>\n", test);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

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
