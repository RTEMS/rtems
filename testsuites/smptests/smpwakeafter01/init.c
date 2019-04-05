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

#include "tmacros.h"

#include <stdio.h>
#include <inttypes.h>

#include <rtems.h>
#include <rtems/libcsupport.h>

const char rtems_test_name[] = "SMPWAKEAFTER 1";

#define CPU_COUNT 32

#define INTERVAL_COUNT 8

#define SCHED_A rtems_build_name(' ', ' ', ' ', 'A')

#define SCHED_B rtems_build_name(' ', ' ', ' ', 'B')

static rtems_id task_ids[CPU_COUNT][INTERVAL_COUNT];

static const rtems_interval intervals[INTERVAL_COUNT] =
  { 1, 2, 3, 5, 7, 11, 13, 17 };

static uint32_t counts[CPU_COUNT][INTERVAL_COUNT];

static rtems_task_argument make_arg(uint32_t i, uint32_t j)
{
  return (i << 8) | (j << 0);
}

static void get_indices(rtems_task_argument arg, uint32_t *i, uint32_t *j)
{
  *i = (arg >> 8) & 0xff;
  *j = (arg >> 0) & 0xff;
}

static void task(rtems_task_argument arg)
{
  uint32_t i;
  uint32_t j;
  rtems_interval ticks;

  get_indices(arg, &i, &j);
  ticks = intervals[j];

  while (true) {
    rtems_status_code sc;

    ++counts[i][j];

    sc = rtems_task_wake_after(ticks);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void test(void)
{
  rtems_status_code sc;
  uint32_t test_time_in_seconds = 10;
  uint32_t cpu_count = rtems_scheduler_get_processor_maximum();
  uint32_t i;

  for (i = 0; i < cpu_count; ++i) {
    rtems_id scheduler_id;
    uint32_t j;

    if (i != 1) {
      sc = rtems_scheduler_ident(SCHED_A, &scheduler_id);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    } else {
      sc = rtems_scheduler_ident(SCHED_B, &scheduler_id);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    }

    for (j = 0; j < INTERVAL_COUNT; ++j) {
      sc = rtems_task_create(
        rtems_build_name('T', 'A', 'S', 'K'),
        255,
        RTEMS_MINIMUM_STACK_SIZE,
        RTEMS_DEFAULT_MODES,
        RTEMS_DEFAULT_ATTRIBUTES,
        &task_ids[i][j]
      );
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      sc = rtems_task_set_scheduler(task_ids[i][j], scheduler_id, 2);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      sc = rtems_task_start(task_ids[i][j], task, make_arg(i, j));
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    }
  }

  for (i = 0; i < test_time_in_seconds; ++i) {
    printf("%" PRIu32 " seconds remaining\n", test_time_in_seconds - i);

    sc = rtems_task_wake_after(rtems_clock_get_ticks_per_second());
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  for (i = 0; i < cpu_count; ++i) {
    uint32_t j;

    for (j = 0; j < INTERVAL_COUNT; ++j) {
      sc = rtems_task_delete(task_ids[i][j]);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      printf(
        "counts[%" PRIu32 "][%" PRIu32 "] = %" PRIu32 "\n",
        i,
        j,
        counts[i][j]
      );
    }
  }
}

static void Init(rtems_task_argument arg)
{
  rtems_resource_snapshot snapshot;

  TEST_BEGIN();

  rtems_resource_snapshot_take(&snapshot);

  test();

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_MAXIMUM_PRIORITY 255

#define CONFIGURE_SCHEDULER_PRIORITY_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_PRIORITY_SMP(a, CONFIGURE_MAXIMUM_PRIORITY + 1);

RTEMS_SCHEDULER_PRIORITY_SMP(b, CONFIGURE_MAXIMUM_PRIORITY + 1);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_PRIORITY_SMP(a, SCHED_A), \
  RTEMS_SCHEDULER_TABLE_PRIORITY_SMP(b, SCHED_B)

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL)

#define CONFIGURE_MAXIMUM_TASKS (1 + CPU_COUNT * INTERVAL_COUNT)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
