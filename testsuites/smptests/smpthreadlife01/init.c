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

#include <rtems.h>
#include <rtems/score/smpbarrier.h>

const char rtems_test_name[] = "SMPTHREADLIFE 1";

#define CPU_COUNT 2

typedef struct {
  volatile rtems_task_argument main_arg;
  volatile rtems_task_argument worker_arg;
  SMP_barrier_Control barrier;
  SMP_barrier_State worker_barrier_state;
} test_context;

static test_context test_instance = {
  .barrier = SMP_BARRIER_CONTROL_INITIALIZER,
  .worker_barrier_state = SMP_BARRIER_STATE_INITIALIZER
};

static void worker_task(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;

  rtems_test_assert(arg == ctx->main_arg);

  ctx->worker_arg = arg;

  _SMP_barrier_Wait(&ctx->barrier, &ctx->worker_barrier_state, CPU_COUNT);

  while (true) {
    /* Do nothing */
  }
}

static void test(void)
{
  test_context *ctx = &test_instance;
  SMP_barrier_State barrier_state = SMP_BARRIER_STATE_INITIALIZER;
  rtems_status_code sc;
  rtems_id id;
  rtems_task_argument arg;

  sc = rtems_task_create(
    rtems_build_name('W', 'O', 'R', 'K'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(id, worker_task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  _SMP_barrier_Wait(
    &ctx->barrier,
    &barrier_state,
    CPU_COUNT
  );

  for (arg = 1; arg < 23; ++arg) {
    ctx->main_arg = arg;
    ctx->worker_arg = 0;

    sc = rtems_task_restart(id, arg);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    _SMP_barrier_Wait(&ctx->barrier, &barrier_state, CPU_COUNT);

    rtems_test_assert(ctx->worker_arg == arg);
  }
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  if (rtems_smp_get_processor_count() >= CPU_COUNT) {
    test();
  }

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_SMP_APPLICATION

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_MAXIMUM_TASKS CPU_COUNT

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
