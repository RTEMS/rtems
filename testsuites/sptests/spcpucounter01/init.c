/*
 * Copyright (c) 2014, 2016 embedded brains GmbH.  All rights reserved.
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

#include <stdio.h>
#include <inttypes.h>

#include <rtems.h>
#include <rtems/counter.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPCPUCOUNTER 1";

#define NS_PER_TICK 1000000

#define N 10

typedef struct {
  rtems_counter_ticks delay_ns_t[N][2];
  rtems_counter_ticks delay_ticks_t[N][2];
  rtems_counter_ticks overhead_t[N][5];
  rtems_counter_ticks overhead_delta;
} test_context;

static test_context test_instance;

static rtems_interval sync_with_clock_tick(void)
{
  rtems_interval start = rtems_clock_get_ticks_since_boot();
  rtems_interval current;

  do {
    current = rtems_clock_get_ticks_since_boot();
  } while (current == start);

  return current;
}

static void test_converter(void)
{
  CPU_Counter_ticks frequency;
  CPU_Counter_ticks frequency2;
  uint64_t ns;
  int64_t sbt;

  frequency = rtems_counter_nanoseconds_to_ticks(1000000000);
  ns = rtems_counter_ticks_to_nanoseconds(frequency);

  printf("CPU counter frequency: %" PRIu32 "Hz\n", frequency);
  printf("nanoseconds for frequency count ticks: %" PRIu64 "\n", ns);

  rtems_test_assert(ns == 1000000000);

  sbt = rtems_counter_ticks_to_sbintime(frequency);
  rtems_test_assert(sbt == (INT64_C(1) << 32));

  frequency2 = rtems_counter_sbintime_to_ticks(sbt);
  rtems_test_assert(frequency == frequency2);
}

static void test_delay_nanoseconds(test_context *ctx)
{
  int i;

  for (i = 0; i < N; ++i) {
    rtems_counter_ticks t0;
    rtems_counter_ticks t1;
    rtems_interval tick;

    tick = sync_with_clock_tick();

    t0 = rtems_counter_read();
    rtems_counter_delay_nanoseconds(NS_PER_TICK);
    t1 = rtems_counter_read();

    ctx->delay_ns_t[i][0] = t0;
    ctx->delay_ns_t[i][1] = t1;

    rtems_test_assert(tick < rtems_clock_get_ticks_since_boot());
  }
}

static void test_delay_ticks(test_context *ctx)
{
  rtems_counter_ticks ticks = rtems_counter_nanoseconds_to_ticks(NS_PER_TICK);
  int i;

  for (i = 0; i < N; ++i) {
    rtems_counter_ticks t0;
    rtems_counter_ticks t1;
    rtems_interval tick;

    tick = sync_with_clock_tick();

    t0 = rtems_counter_read();
    rtems_counter_delay_ticks(ticks);
    t1 = rtems_counter_read();

    ctx->delay_ticks_t[i][0] = t0;
    ctx->delay_ticks_t[i][1] = t1;

    rtems_test_assert(tick < rtems_clock_get_ticks_since_boot());
  }
}

static void test_overheads(test_context *ctx)
{
  int i;

  for (i = 0; i < N; ++i) {
    rtems_counter_ticks t0;
    rtems_counter_ticks t1;
    rtems_counter_ticks t2;
    rtems_counter_ticks t3;
    rtems_counter_ticks t4;
    rtems_counter_ticks d;

    t0 = rtems_counter_read();
    t1 = rtems_counter_read();
    d = rtems_counter_difference(t1, t0);
    t2 = rtems_counter_read();
    rtems_counter_delay_nanoseconds(0);
    t3 = rtems_counter_read();
    rtems_counter_delay_ticks(0);
    t4 = rtems_counter_read();

    ctx->overhead_t[i][0] = t0;
    ctx->overhead_t[i][1] = t1;
    ctx->overhead_t[i][2] = t2;
    ctx->overhead_t[i][3] = t3;
    ctx->overhead_t[i][4] = t4;
    ctx->overhead_delta = d;
  }
}

static void report_overhead(
  const char *name,
  rtems_counter_ticks t1,
  rtems_counter_ticks t0
)
{
  rtems_counter_ticks d;
  uint64_t ns;

  d = rtems_counter_difference(t1, t0);
  ns = rtems_counter_ticks_to_nanoseconds(d);

  printf(
    "overhead %s: %" PRIu64 " ticks, %" PRIu64 "ns\n",
    name,
    (uint64_t) d,
    ns
  );
}

static uint64_t large_delta_to_ns(rtems_counter_ticks d)
{
  uint64_t ns;

  ns = rtems_counter_ticks_to_nanoseconds(d);

  /* Special case for CPU counters using the clock driver counter */
  if (ns < rtems_configuration_get_nanoseconds_per_tick()) {
    printf(
      "warning: the RTEMS counter seems to be unable to\n"
      "  measure intervals greater than the clock tick interval\n"
    );

    ns += rtems_configuration_get_nanoseconds_per_tick();
  }

  return ns;
}

static void test_report(test_context *ctx)
{
  double ns_per_tick = NS_PER_TICK;
  rtems_counter_ticks d;
  uint64_t ns;
  size_t i;

  printf("test delay nanoseconds (%i times)\n", N);

  for (i = 0; i < N; ++i) {
    d = rtems_counter_difference(ctx->delay_ns_t[i][1], ctx->delay_ns_t[i][0]);
    ns = large_delta_to_ns(d);

    printf(
      "ns busy wait duration: %" PRIu64 "ns\n"
      "ns busy wait relative to clock tick: %f\n",
      ns,
      (ns - ns_per_tick) / ns_per_tick
    );
  }

  printf("test delay ticks (%i times)\n", N);

  for (i = 0; i < N; ++i) {
    d = rtems_counter_difference(
      ctx->delay_ticks_t[i][1],
      ctx->delay_ticks_t[i][0]
    );
    ns = large_delta_to_ns(d);

    printf(
      "ticks busy wait duration: %" PRIu64 "ns\n"
      "ticks busy wait relative to clock tick: %f\n",
      ns,
      (ns - ns_per_tick) / ns_per_tick
    );
  }

  printf("test overheads (%i times)\n", N);

  for (i = 0; i < N; ++i) {
    report_overhead("read", ctx->overhead_t[i][1], ctx->overhead_t[i][0]);
    report_overhead("difference", ctx->overhead_t[i][2], ctx->overhead_t[i][1]);
    report_overhead("delay ns", ctx->overhead_t[i][3], ctx->overhead_t[i][2]);
    report_overhead("delay ticks", ctx->overhead_t[i][4], ctx->overhead_t[i][3]);
  }
}

static void Init(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;

  rtems_print_printer_fprintf_putc(&rtems_test_printer);
  TEST_BEGIN();

  test_delay_nanoseconds(ctx);
  test_delay_ticks(ctx);
  test_overheads(ctx);
  test_converter();
  test_report(ctx);

  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK (NS_PER_TICK / 1000)

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
