/*
 * Copyright (c) 2017, 2018 embedded brains GmbH.  All rights reserved.
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

#include <rtems.h>
#include <rtems/cpuuse.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPSCHEDEDF 1";

typedef struct {
  rtems_id task[2];
} test_context;

static test_context test_instance;

static void t(test_context *ctx, rtems_interval p, long nanoseconds)
{
  rtems_status_code sc;
  rtems_id period;
  rtems_name name;

  sc = rtems_object_get_classic_name(rtems_task_self(), &name);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_rate_monotonic_create(name, &period);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  while (true) {
    rtems_test_busy_cpu_usage(0, nanoseconds);

    sc = rtems_rate_monotonic_period(period, p);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void t1(rtems_task_argument arg)
{
  test_context *ctx = (test_context *) arg;

  t(ctx, 50, 25000000);
}

static void t2(rtems_task_argument arg)
{
  test_context *ctx = (test_context *) arg;

  t(ctx, 75, 30000000);
}

static void test(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_task_create(
    rtems_build_name('T', '1', ' ', ' '),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->task[0]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('T', '2', ' ', ' '),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->task[1]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_wake_after(1);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->task[0], t1, (rtems_task_argument) ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->task[1], t2, (rtems_task_argument) ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_cpu_usage_reset();

  sc = rtems_task_wake_after(50 * 75);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_suspend(ctx->task[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_suspend(ctx->task[1]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_cpu_usage_report_with_plugin(&rtems_test_printer);
  rtems_rate_monotonic_report_statistics_with_plugin(&rtems_test_printer);
}

static void Init(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;

  TEST_BEGIN();

  test(ctx);

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 3
#define CONFIGURE_MAXIMUM_PERIODS 2

#define CONFIGURE_MAXIMUM_PROCESSORS 1

#define CONFIGURE_SCHEDULER_EDF_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_EDF_SMP(a, CONFIGURE_MAXIMUM_PROCESSORS);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(a, rtems_build_name('E', 'D', 'F', ' '))

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
