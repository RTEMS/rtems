/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2015, 2024 embedded brains GmbH & Co. KG
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

#include <sys/lock.h>

#define _KERNEL

#include <sys/time.h>
#include <sys/timetc.h>

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/counter.h>
#include <rtems/test-info.h>

#include <rtems/score/timecounterimpl.h>
#include <rtems/timecounter.h>
#include <rtems/bsd.h>

#include <test_support.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPTIMECOUNTER 2";

#define CPU_COUNT 32

#define DURATION_IN_SECONDS 1

typedef struct {
  rtems_test_parallel_context base;
  const char *test_sep;
  const char *counter_sep;
  struct timecounter tc_null;
  uint32_t binuptime_per_job[CPU_COUNT];
  sbintime_t duration_per_job[CPU_COUNT];
  uint32_t rtemsuptime_per_job[CPU_COUNT];
} timecounter_context;

static timecounter_context test_instance;

static rtems_interval test_duration(void)
{
  return DURATION_IN_SECONDS * rtems_clock_get_ticks_per_second();
}

static uint32_t test_get_timecount_null(struct timecounter *tc)
{
  return 0;
}

static void install_tc_null(timecounter_context *ctx)
{
  struct timecounter *tc_cpu = &ctx->tc_null;

  tc_cpu->tc_get_timecount = test_get_timecount_null;
  tc_cpu->tc_counter_mask = 0xffffffff;
  tc_cpu->tc_frequency = rtems_counter_nanoseconds_to_ticks(1000000000);
  tc_cpu->tc_quality = 2000;
  rtems_timecounter_install(tc_cpu);
}

static void test_print_results(
  const char *driver,
  timecounter_context *ctx,
  size_t active_workers
)
{
  const char *value_sep;
  size_t i;

  if (active_workers == 1) {
    printf(
      "%s{\n"
      "    \"timecounter\": \"%s\",\n"
      "    \"counter\": [",
      ctx->test_sep,
      driver
    );
    ctx->test_sep = ", ";
    ctx->counter_sep = "\n      ";
  }

  printf("%s[", ctx->counter_sep);
  ctx->counter_sep = "],\n      ";
  value_sep = "";

  for (i = 0; i < active_workers; ++i) {
    printf(
      "%s%" PRIu32,
      value_sep,
      ctx->binuptime_per_job[i]
    );
    value_sep = ", ";
  }

  if (active_workers == rtems_scheduler_get_processor_maximum()) {
    printf("]\n    ]\n  }");
  }
}

static rtems_interval test_bintime_init(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  rtems_test_spin_until_next_tick();

  return test_duration();
}

static void test_bintime_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  timecounter_context *ctx = (timecounter_context *) base;
  uint32_t counter = 1;
  struct bintime start;
  struct bintime end;

  rtems_bsd_binuptime(&start);

  do {
    ++counter;
    rtems_bsd_binuptime(&end);
  } while (!rtems_test_parallel_stop_job(&ctx->base));

  ctx->binuptime_per_job[worker_index] = counter;
  ctx->duration_per_job[worker_index] = bttosbt(end) - bttosbt(start);
}

static void test_bintime_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  timecounter_context *ctx = (timecounter_context *) base;
  size_t i;

  for (i = 0; i < active_workers; ++i) {
    sbintime_t error;

    error = DURATION_IN_SECONDS * SBT_1S - ctx->duration_per_job[i];
    rtems_test_assert(error * error < SBT_1MS * SBT_1MS);
  }

  test_print_results("Clock Driver", ctx, active_workers);
}

static rtems_interval test_bintime_null_init(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  timecounter_context *ctx = &test_instance;

  install_tc_null(ctx);

  return test_duration();
}

static void test_bintime_null_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  timecounter_context *ctx = (timecounter_context *) base;
  struct bintime bt;
  uint32_t counter = 0;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    ++counter;
    rtems_bsd_binuptime(&bt);
  }

  ctx->binuptime_per_job[worker_index] = counter;
}

static void test_bintime_null_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_print_results("Null", (timecounter_context *) base, active_workers);
}

static const rtems_test_parallel_job timecounter_jobs[] = {
  {
    .init = test_bintime_init,
    .body = test_bintime_body,
    .fini = test_bintime_fini,
    .cascade = true
  },{
    .init = test_bintime_null_init,
    .body = test_bintime_null_body,
    .fini = test_bintime_null_fini,
    .cascade = true
  }
};

static void Init(rtems_task_argument arg)
{
  timecounter_context *ctx = &test_instance;
  struct bintime bt;
  struct timespec ts;
  struct timeval tv;

  TEST_BEGIN();

  printf("*** BEGIN OF JSON DATA ***\n[\n  ");

  ctx->test_sep = "";
  rtems_test_parallel(
    &ctx->base,
    NULL,
    &timecounter_jobs[0],
    RTEMS_ARRAY_SIZE(timecounter_jobs)
  );

  printf("\n]\n*** END OF JSON DATA ***\n");

  /* Check for all functions available in the bsd.h user space */

  rtems_bsd_bintime(&bt);
  rtems_bsd_microtime(&tv);
  rtems_bsd_nanotime(&ts);
  rtems_bsd_binuptime(&bt);
  rtems_bsd_microuptime(&tv);
  rtems_bsd_nanouptime(&ts);
  rtems_bsd_getbintime(&bt);
  rtems_bsd_getmicrotime(&tv);
  rtems_bsd_getnanotime(&ts);
  rtems_bsd_getbinuptime(&bt);
  rtems_bsd_getmicrouptime(&tv);
  rtems_bsd_getnanouptime(&ts);

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS (2 + CPU_COUNT - 1)
#define CONFIGURE_MAXIMUM_TIMERS 2
#define CONFIGURE_MAXIMUM_PERIODS 1

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
