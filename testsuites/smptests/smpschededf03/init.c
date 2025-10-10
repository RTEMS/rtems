/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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

#include "tmacros.h"

#include <rtems.h>

const char rtems_test_name[] = "SMPSCHEDEDF 3";

#define CPU_COUNT 32

#define TASK_COUNT (3 * CPU_COUNT)

typedef struct {
  rtems_id task_ids[TASK_COUNT];
} test_context;

static test_context test_instance;

static void wait_task(rtems_task_argument arg)
{
  (void) arg;

  while (true) {
    rtems_status_code sc;

    sc = rtems_task_wake_after(1);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static uint32_t simple_random(uint32_t v)
{
  v *= 1664525;
  v += 1013904223;
  return v;
}

static void affinity_task(rtems_task_argument arg)
{
  uint32_t v;
  uint32_t n;

  v = (uint32_t) arg;
  n = rtems_scheduler_get_processor_maximum();

  while (true) {
    rtems_status_code sc;
    cpu_set_t set;

    CPU_ZERO(&set);
    CPU_SET((v >> 13) % n, &set);
    v = simple_random(v);

    sc = rtems_task_set_affinity(RTEMS_SELF, sizeof(set), &set);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void create_and_start_task(
  test_context *ctx,
  rtems_task_entry entry,
  size_t i,
  size_t j
)
{
  rtems_status_code sc;

  j = j * CPU_COUNT + i;

  sc = rtems_task_create(
    rtems_build_name('E', 'D', 'F', ' '),
    i + 2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->task_ids[j]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->task_ids[j], entry, j);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void delete_task(
  test_context *ctx,
  size_t i,
  size_t j
)
{
  rtems_status_code sc;

  j = j * CPU_COUNT + i;

  sc = rtems_task_delete(ctx->task_ids[j]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test(test_context *ctx)
{
  rtems_status_code sc;
  size_t i;

  for (i = 0; i < CPU_COUNT; ++i) {
    create_and_start_task(ctx, wait_task, i, 0);
    create_and_start_task(ctx, affinity_task, i, 1);
    create_and_start_task(ctx, affinity_task, i, 2);
  }

  sc = rtems_task_wake_after(10 * rtems_clock_get_ticks_per_second());
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (i = 0; i < CPU_COUNT; ++i) {
    delete_task(ctx, i, 0);
    delete_task(ctx, i, 1);
    delete_task(ctx, i, 2);
  }
}

static void Init(rtems_task_argument arg)
{
  (void) arg;

  TEST_BEGIN();
  test(&test_instance);
  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS (1 + TASK_COUNT)

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_SCHEDULER_EDF_SMP

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
