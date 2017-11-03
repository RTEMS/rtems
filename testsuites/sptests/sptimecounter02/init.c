/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
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
#include <rtems/test.h>

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

  printf("  <BinuptimeTest activeWorker=\"%zu\">\n", active_workers);

  for (i = 0; i < active_workers; ++i) {
    sbintime_t error;

    printf(
      "    <Counter worker=\"%zu\">%" PRIu32 "</Counter>\n"
      "    <Duration worker=\"%zu\" unit=\"sbintime\">%" PRId64 "</Duration>\n",
      i + 1,
      ctx->binuptime_per_job[i],
      i + 1,
      ctx->duration_per_job[i]
    );

    error = DURATION_IN_SECONDS * SBT_1S - ctx->duration_per_job[i];
    rtems_test_assert(error * error < SBT_1MS * SBT_1MS);
  }

  printf("  </BinuptimeTest>\n");
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
  timecounter_context *ctx = (timecounter_context *) base;
  size_t i;

  printf("  <BinuptimeNullTest activeWorker=\"%zu\">\n", active_workers);

  for (i = 0; i < active_workers; ++i) {
    printf(
      "    <Counter worker=\"%zu\">%" PRIu32 "</Counter>\n",
      i + 1,
      ctx->binuptime_per_job[i]
    );
  }

  printf("  </BinuptimeNullTest>\n");
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

  printf("<SPTimecounter01>\n");

  rtems_test_parallel(
    &ctx->base,
    NULL,
    &timecounter_jobs[0],
    RTEMS_ARRAY_SIZE(timecounter_jobs)
  );

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

  printf("</SPTimecounter01>\n");

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
