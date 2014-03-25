/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#define TESTS_USE_PRINTF
#include "tmacros.h"

const char rtems_test_name[] = "SPCPUCOUNTER 1";

#define NS_PER_TICK 1000000

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
  CPU_Counter_ticks frequency = rtems_counter_nanoseconds_to_ticks(1000000000);
  uint64_t ns = rtems_counter_ticks_to_nanoseconds(frequency);

  printf("CPU counter frequency: %" PRIu32 "Hz\n", frequency);
  printf("nanoseconds for frequency count ticks: %" PRIu64 "\n", ns);

  rtems_test_assert(ns == 1000000000);
}

static void test_delay_nanoseconds(void)
{
  rtems_counter_ticks start;
  rtems_counter_ticks end;
  rtems_counter_ticks delta;
  double ns_per_tick = NS_PER_TICK;
  uint64_t ns_delta;
  rtems_interval tick;
  int n = 10;
  int i;

  printf("test delay nanoseconds (%i times)\n", n);

  for (i = 0; i < n; ++i) {
    tick = sync_with_clock_tick();

    start = rtems_counter_read();
    rtems_counter_delay_nanoseconds(NS_PER_TICK);
    end = rtems_counter_read();

    rtems_test_assert(tick < rtems_clock_get_ticks_since_boot());

    delta = rtems_counter_difference(end, start);
    ns_delta = rtems_counter_ticks_to_nanoseconds(delta);

    /* Special case for CPU counters using the clock driver counter */
    if (ns_delta < rtems_configuration_get_nanoseconds_per_tick()) {
      printf(
        "warning: the RTEMS counter seems to be unable to\n"
        "  measure intervals greater than the clock tick interval\n"
      );

      ns_delta += rtems_configuration_get_nanoseconds_per_tick();
    }

    printf(
      "busy wait duration: %" PRIu64 "ns\n"
      "busy wait relative to clock tick: %f\n",
      ns_delta,
      (ns_delta - ns_per_tick) / ns_per_tick
    );
  }
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test_converter();
  test_delay_nanoseconds();

  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK (NS_PER_TICK / 1000)

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
