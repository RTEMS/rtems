/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2013, 2024 embedded brains GmbH & Co. KG
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

#include <rtems/score/smplock.h>
#include <rtems/score/smplockmcs.h>
#include <rtems/score/smplockseq.h>
#include <rtems/test-info.h>
#include <rtems.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPLOCK 1";

#define TASK_PRIORITY 1

#define CPU_COUNT 32

#define TEST_COUNT 13

typedef struct {
  rtems_test_parallel_context base;
  const char *test_sep;
  const char *counter_sep;
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

static const rtems_test_parallel_job test_jobs[TEST_COUNT];

static void test_fini(
  test_context *ctx,
  const char *lock_type,
  bool global_lock,
  const char *section_type,
  size_t test,
  size_t active_workers
)
{
  bool cascade = test_jobs[test].cascade;
  unsigned long sum = 0;
  const char *value_sep;
  size_t i;

  if (active_workers == 1 || !cascade) {
    printf(
      "%s{\n"
      "    \"lock-type\": \"%s\",\n"
      "    \"lock-object\": \"%s\",\n"
      "    \"section-type\": \"%s\",\n"
      "    \"results\": [",
      ctx->test_sep,
      lock_type,
      global_lock ? "global" : "local",
      section_type
    );
    ctx->test_sep = ", ";
    ctx->counter_sep = "\n      ";
  }

  printf(
    "%s{\n"
    "        \"counter\": [", ctx->counter_sep);
  ctx->counter_sep = "\n      }, ";
  value_sep = "";

  for (i = 0; i < active_workers; ++i) {
    unsigned long local_counter =
      ctx->local_counter[active_workers - 1][test][i];

    sum += local_counter;

    printf(
      "%s%lu",
      value_sep,
      local_counter
    );
    value_sep = ", ";
  }

  printf(
    "],\n"
    "        \"global-counter\": %lu,\n"
    "        \"sum-of-local-counter\": %lu",
    ctx->counter[test],
    sum
  );

  if (active_workers == rtems_scheduler_get_processor_maximum() || !cascade) {
    printf("\n      }\n    ]\n  }");
  }
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
    "Ticket Lock",
    true,
    "local counter",
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
    "MCS Lock",
    true,
    "local counter",
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
    "Ticket Lock",
    true,
    "global counter",
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
    "MCS Lock",
    true,
    "global counter",
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
    "Ticket Lock",
    false,
    "local counter",
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
    "MCS Lock",
    false,
    "local counter",
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
    "Ticket Lock",
    false,
    "global counter",
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
    "MCS Lock",
    false,
    "global counter",
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
    "Ticket Lock",
    true,
    "busy loop",
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
    "MCS Lock",
    true,
    "busy loop",
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
    "Sequence Lock",
    true,
    "two global counter",
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
    "TAS Lock",
    true,
    "local counter",
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
    "TTAS Lock",
    true,
    "local counter",
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

  printf("*** BEGIN OF JSON DATA ***\n[\n  ");
  ctx->test_sep = "";
  rtems_test_parallel(&ctx->base, NULL, &test_jobs[0], TEST_COUNT);
  printf("\n]\n*** END OF JSON DATA ***\n");
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
