/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
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

#include <rtems.h>

const char rtems_test_name[] = "SMPSCHEDEDF 4";

#define CPU_COUNT 4

#define TASK_COUNT 2

#define MAIN rtems_build_name('M', 'A', 'I', 'N')

#define OTHER rtems_build_name('O', 'T', 'H', 'R')

typedef struct {
  rtems_id other_scheduler_id;
  rtems_id task_ids[TASK_COUNT];
} test_context;

static test_context test_instance;

static void do_nothing_task(rtems_task_argument arg)
{
  (void) arg;

  _CPU_Thread_Idle_body(0);
}

static void test(void)
{
  test_context *ctx;
  rtems_status_code sc;
  size_t i;

  ctx = &test_instance;

  for (i = 0; i < TASK_COUNT; ++i) {
    sc = rtems_task_create(
      rtems_build_name('N', 'B', 'D', 'Y'),
      2,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &ctx->task_ids[i]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_start(ctx->task_ids[i], do_nothing_task, 0);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  sc = rtems_scheduler_ident(OTHER, &ctx->other_scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (i = 0; i < TASK_COUNT; ++i) {
    const Per_CPU_Control *cpu;

    sc = rtems_task_set_scheduler(ctx->task_ids[i], ctx->other_scheduler_id, 2);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    cpu = _Per_CPU_Get_by_index(CPU_COUNT - 1 - i);
    rtems_test_assert(cpu->heir->Object.id == ctx->task_ids[i]);
  }

  for (i = 0; i < TASK_COUNT; ++i) {
    sc = rtems_task_delete(ctx->task_ids[i]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  if (rtems_scheduler_get_processor_maximum() == CPU_COUNT) {
    test();
  } else {
    puts("warning: wrong processor count to run the test");
  }

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS (1 + TASK_COUNT)

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_SCHEDULER_EDF_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_EDF_SMP(a, CONFIGURE_MAXIMUM_PROCESSORS);

RTEMS_SCHEDULER_EDF_SMP(b, CONFIGURE_MAXIMUM_PROCESSORS);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(a, MAIN), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(b, OTHER)

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
