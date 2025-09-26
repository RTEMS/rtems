/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2015 embedded brains GmbH & Co. KG
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

#include <rtems/test-info.h>

#include <rtems/bsd.h>

#include <test_support.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPTIMECOUNTER 3";

#define CPU_COUNT 32

static rtems_test_parallel_context ctx;

static rtems_interval test_binuptime_init(
  rtems_test_parallel_context *ctx,
  void *arg,
  size_t active_workers
)
{
  (void) ctx;
  (void) arg;
  (void) active_workers;

  return 10 * rtems_clock_get_ticks_per_second();
}

static void test_binuptime_body(
  rtems_test_parallel_context *ctx,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  (void) ctx;
  (void) arg;
  (void) active_workers;
  (void) worker_index;

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
  (void) ctx;
  (void) arg;
  (void) active_workers;

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
  (void) arg;

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
