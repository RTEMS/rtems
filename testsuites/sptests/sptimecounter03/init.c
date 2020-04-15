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

#include <rtems/test.h>

#include <rtems/bsd.h>

#include <test_support.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPTIMECOUNTER 3";

#define CPU_COUNT 32

static rtems_test_parallel_context ctx;;

static rtems_interval test_binuptime_init(
  rtems_test_parallel_context *ctx,
  void *arg,
  size_t active_workers
)
{
  return 10 * rtems_clock_get_ticks_per_second();
}

static void test_binuptime_body(
  rtems_test_parallel_context *ctx,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  struct bintime start;
  struct bintime end;

  rtems_bsd_binuptime(&start);

  do {
    rtems_bsd_binuptime(&end);
    rtems_test_assert(
      end.sec > start.sec
        || (end.sec == start.sec && end.frac >= start.frac)
    );
    start = end;
  } while (!rtems_test_parallel_stop_job(ctx));
}

static void test_binuptime_fini(
  rtems_test_parallel_context *ctx,
  void *arg,
  size_t active_workers
)
{
  /* Nothing to do */
}

static const rtems_test_parallel_job jobs[] = {
  {
    .init = test_binuptime_init,
    .body = test_binuptime_body,
    .fini = test_binuptime_fini,
    .cascade = false
  }
};

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  rtems_test_parallel(&ctx, NULL, &jobs[0], RTEMS_ARRAY_SIZE(jobs));

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS CPU_COUNT
#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
