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
#include <rtems/counter.h>
#include <rtems/libcsupport.h>
#include <rtems/score/profiling.h>
#include <rtems/score/smpbarrier.h>
#include <rtems/score/threadimpl.h>

const char rtems_test_name[] = "SMPTHREADLIFE 1";

#define CPU_COUNT 2

typedef struct {
  volatile rtems_task_argument main_arg;
  volatile rtems_task_argument worker_arg;
  volatile bool terminated;
  SMP_barrier_Control barrier;
  SMP_barrier_State main_barrier_state;
  SMP_barrier_State worker_barrier_state;
  Thread_Control *delay_switch_for_executing;
} test_context;

static test_context test_instance = {
  .barrier = SMP_BARRIER_CONTROL_INITIALIZER,
  .main_barrier_state = SMP_BARRIER_STATE_INITIALIZER,
  .worker_barrier_state = SMP_BARRIER_STATE_INITIALIZER
};

static void restart_extension(
  Thread_Control *executing,
  Thread_Control *restarted
)
{
  rtems_test_assert(executing == restarted);
}

static void delete_extension(
  Thread_Control *executing,
  Thread_Control *deleted
)
{
  rtems_test_assert(executing != deleted);
}

static void terminate_extension(Thread_Control *executing)
{
  test_context *ctx = &test_instance;

  ctx->terminated = true;
}

static void switch_extension(Thread_Control *executing, Thread_Control *heir)
{
  test_context *ctx = &test_instance;

  if (ctx->delay_switch_for_executing == executing) {
    ctx->delay_switch_for_executing = NULL;
    _SMP_barrier_Wait(&ctx->barrier, &ctx->worker_barrier_state, CPU_COUNT);
    rtems_counter_delay_nanoseconds(100000000);

    /* Avoid bad profiling statisitics */
    _Profiling_Thread_dispatch_disable( _Per_CPU_Get(), 0 );
  }
}

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

static void test_restart(void)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  rtems_id id;
  rtems_task_argument arg;
  rtems_resource_snapshot snapshot;

  rtems_resource_snapshot_take(&snapshot);

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

  _SMP_barrier_Wait(&ctx->barrier, &ctx->main_barrier_state, CPU_COUNT);

  for (arg = 1; arg < 23; ++arg) {
    ctx->main_arg = arg;
    ctx->worker_arg = 0;

    sc = rtems_task_restart(id, arg);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    _SMP_barrier_Wait(&ctx->barrier, &ctx->main_barrier_state, CPU_COUNT);

    rtems_test_assert(ctx->worker_arg == arg);
  }

  sc = rtems_task_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));
}

static void test_delete(void)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  rtems_id id;
  rtems_task_argument arg;
  rtems_resource_snapshot snapshot;

  rtems_resource_snapshot_take(&snapshot);

  for (arg = 31; arg < 57; ++arg) {
    ctx->main_arg = arg;
    ctx->worker_arg = 0;
    ctx->terminated = false;

    sc = rtems_task_create(
      rtems_build_name('W', 'O', 'R', 'K'),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_start(id, worker_task, arg);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    _SMP_barrier_Wait(&ctx->barrier, &ctx->main_barrier_state, CPU_COUNT);

    rtems_test_assert(ctx->worker_arg == arg);
    rtems_test_assert(!ctx->terminated);

    sc = rtems_task_delete(id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(ctx->terminated);

    rtems_test_assert(rtems_resource_snapshot_check(&snapshot));
  }
}

static void delay_ipi_task(rtems_task_argument variant)
{
  test_context *ctx = &test_instance;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  (void) level;

  _SMP_barrier_Wait(&ctx->barrier, &ctx->worker_barrier_state, CPU_COUNT);

  /*
   * Interrupts are disabled, so the inter-processor interrupt deleting us will
   * be delayed a bit.
   */
  rtems_counter_delay_nanoseconds(100000000);

  if (variant != 0) {
    _Thread_Disable_dispatch();
  }

  /*
   * We get deleted as a side effect of enabling the thread life protection or
   * later if we enable the thread dispatching.
   */
  _Thread_Set_life_protection(true);

  if (variant != 0) {
    _Thread_Enable_dispatch();
  }

  rtems_test_assert(0);
}

static void test_set_life_protection(rtems_task_argument variant)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  rtems_id id;
  rtems_resource_snapshot snapshot;

  rtems_resource_snapshot_take(&snapshot);

  sc = rtems_task_create(
    rtems_build_name('D', 'E', 'L', 'Y'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(id, delay_ipi_task, variant);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  _SMP_barrier_Wait(&ctx->barrier, &ctx->main_barrier_state, CPU_COUNT);

  sc = rtems_task_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));
}

static void delay_switch_task(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  (void) level;

  ctx->delay_switch_for_executing = _Thread_Get_executing();

  _SMP_barrier_Wait(&ctx->barrier, &ctx->worker_barrier_state, CPU_COUNT);

  sc = rtems_task_delete(RTEMS_SELF);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_wait_for_execution_stop(void)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  rtems_id id;
  rtems_resource_snapshot snapshot;

  rtems_resource_snapshot_take(&snapshot);

  sc = rtems_task_create(
    rtems_build_name('S', 'W', 'I', 'T'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(id, delay_switch_task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* Wait for delay switch task */
  _SMP_barrier_Wait(&ctx->barrier, &ctx->main_barrier_state, CPU_COUNT);

  /* Wait for delay switch extension */
  _SMP_barrier_Wait(&ctx->barrier, &ctx->main_barrier_state, CPU_COUNT);

  sc = rtems_task_create(
    rtems_build_name('W', 'A', 'I', 'T'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  if (rtems_get_processor_count() >= CPU_COUNT) {
    test_restart();
    test_delete();
    test_set_life_protection(0);
    test_set_life_protection(1);
    test_wait_for_execution_stop();
  }

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_SMP_APPLICATION

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_MAXIMUM_TASKS CPU_COUNT

#define CONFIGURE_INITIAL_EXTENSIONS \
  { \
    .thread_restart = restart_extension, \
    .thread_delete = delete_extension, \
    .thread_terminate = terminate_extension, \
    .thread_switch = switch_extension \
  }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
