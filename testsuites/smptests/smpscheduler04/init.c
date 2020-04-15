/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#include <rtems.h>
#include <rtems/libcsupport.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPSCHEDULER 4";

#define CPU_COUNT 32

#define PRIO_MIGRATION 2

#define PRIO_SCHEDULER 3

typedef struct {
  rtems_id migration_task;
  rtems_id scheduler_task;
  rtems_id scheduler_ids[CPU_COUNT];
  uint32_t migration_counter RTEMS_ALIGNED(CPU_CACHE_LINE_BYTES);
  uint32_t scheduler_counter RTEMS_ALIGNED(CPU_CACHE_LINE_BYTES);
} test_context;

static test_context test_instance;

static void migration_task(rtems_task_argument arg)
{
  test_context *ctx = (test_context *) arg;
  uint32_t cpu_count = rtems_scheduler_get_processor_maximum();
  uint32_t cpu_index = rtems_scheduler_get_processor();

  while (true) {
    rtems_status_code sc;

    cpu_index = (cpu_index + 1) % cpu_count;

    sc = rtems_task_set_scheduler(
      RTEMS_SELF,
      ctx->scheduler_ids[cpu_index],
      PRIO_MIGRATION
    );

    if (sc == RTEMS_UNSATISFIED) {
      continue;
    }

    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(cpu_index == rtems_scheduler_get_processor());
    ++ctx->migration_counter;
  }
}

static void scheduler_task(rtems_task_argument arg)
{
  test_context *ctx = (test_context *) arg;
  uint32_t cpu_count = rtems_scheduler_get_processor_maximum();
  uint32_t cpu_index = rtems_scheduler_get_processor();

  while (true) {
    rtems_status_code sc;

    cpu_index = (cpu_index - 1) % cpu_count;

    if (cpu_index == 0) {
      cpu_index = 1;
    }

    do {
      sc = rtems_scheduler_remove_processor(
        ctx->scheduler_ids[cpu_index],
        cpu_index
      );
    } while (sc == RTEMS_RESOURCE_IN_USE);

    sc = rtems_scheduler_add_processor(
      ctx->scheduler_ids[cpu_index],
      cpu_index
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    ++ctx->scheduler_counter;
  }
}

static void test(test_context *ctx)
{
  rtems_status_code sc;
  uint32_t i;

  for (i = 0; i < rtems_scheduler_get_processor_maximum(); ++i) {
    sc = rtems_scheduler_ident(i, &ctx->scheduler_ids[i]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  sc = rtems_task_create(
    rtems_build_name('M', 'I', 'G', 'R'),
    PRIO_MIGRATION,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->migration_task
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(
    ctx->migration_task,
    migration_task,
    (rtems_task_argument) ctx
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('S', 'C', 'H', 'D'),
    PRIO_SCHEDULER,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->scheduler_task
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(
    ctx->scheduler_task,
    scheduler_task,
    (rtems_task_argument) ctx
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_wake_after(10 * rtems_clock_get_ticks_per_second());
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(ctx->migration_task);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(ctx->scheduler_task);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  printf(
    "migration counter = %" PRIu32 "\n"
    "scheduler counter = %" PRIu32 "\n",
    ctx->migration_counter,
    ctx->scheduler_counter
  );
}

static void Init(rtems_task_argument arg)
{
  rtems_resource_snapshot snapshot;

  TEST_BEGIN();
  rtems_resource_snapshot_take(&snapshot);
  test(&test_instance);
  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));
  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 3

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_SCHEDULER_SIMPLE_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_SIMPLE_SMP(0);
RTEMS_SCHEDULER_SIMPLE_SMP(1);
RTEMS_SCHEDULER_SIMPLE_SMP(2);
RTEMS_SCHEDULER_SIMPLE_SMP(3);
RTEMS_SCHEDULER_SIMPLE_SMP(4);
RTEMS_SCHEDULER_SIMPLE_SMP(5);
RTEMS_SCHEDULER_SIMPLE_SMP(6);
RTEMS_SCHEDULER_SIMPLE_SMP(7);
RTEMS_SCHEDULER_SIMPLE_SMP(8);
RTEMS_SCHEDULER_SIMPLE_SMP(9);
RTEMS_SCHEDULER_SIMPLE_SMP(10);
RTEMS_SCHEDULER_SIMPLE_SMP(11);
RTEMS_SCHEDULER_SIMPLE_SMP(12);
RTEMS_SCHEDULER_SIMPLE_SMP(13);
RTEMS_SCHEDULER_SIMPLE_SMP(14);
RTEMS_SCHEDULER_SIMPLE_SMP(15);
RTEMS_SCHEDULER_SIMPLE_SMP(16);
RTEMS_SCHEDULER_SIMPLE_SMP(17);
RTEMS_SCHEDULER_SIMPLE_SMP(18);
RTEMS_SCHEDULER_SIMPLE_SMP(19);
RTEMS_SCHEDULER_SIMPLE_SMP(20);
RTEMS_SCHEDULER_SIMPLE_SMP(21);
RTEMS_SCHEDULER_SIMPLE_SMP(22);
RTEMS_SCHEDULER_SIMPLE_SMP(23);
RTEMS_SCHEDULER_SIMPLE_SMP(24);
RTEMS_SCHEDULER_SIMPLE_SMP(25);
RTEMS_SCHEDULER_SIMPLE_SMP(26);
RTEMS_SCHEDULER_SIMPLE_SMP(27);
RTEMS_SCHEDULER_SIMPLE_SMP(28);
RTEMS_SCHEDULER_SIMPLE_SMP(29);
RTEMS_SCHEDULER_SIMPLE_SMP(30);
RTEMS_SCHEDULER_SIMPLE_SMP(31);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(0, 0), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(1, 1), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(2, 2), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(3, 3), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(4, 4), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(5, 5), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(6, 6), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(7, 7), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(8, 8), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(9, 9), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(10, 10), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(11, 11), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(12, 12), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(13, 13), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(14, 14), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(15, 15), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(16, 16), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(17, 17), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(18, 18), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(19, 19), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(20, 20), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(21, 21), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(22, 22), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(23, 23), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(24, 24), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(25, 25), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(26, 26), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(27, 27), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(28, 28), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(29, 29), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(30, 30), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(31, 31)

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(2, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(3, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(4, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(5, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(6, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(7, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(8, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(9, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(10, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(11, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(12, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(13, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(14, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(15, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(16, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(17, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(18, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(19, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(20, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(21, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(22, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(23, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(24, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(25, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(26, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(27, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(28, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(29, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(30, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(31, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
