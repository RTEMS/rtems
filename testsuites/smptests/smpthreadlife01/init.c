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
  rtems_id worker_id;
} test_context;

static test_context test_instance = {
  .barrier = SMP_BARRIER_CONTROL_INITIALIZER,
  .main_barrier_state = SMP_BARRIER_STATE_INITIALIZER,
  .worker_barrier_state = SMP_BARRIER_STATE_INITIALIZER
};

static void barrier(test_context *ctx, SMP_barrier_State *state)
{
  _SMP_barrier_Wait(&ctx->barrier, state, CPU_COUNT);
}

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

    /* (A) */
    barrier(ctx, &ctx->worker_barrier_state);

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

  /* (B) */
  barrier(ctx, &ctx->worker_barrier_state);

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

  /* (B) */
  barrier(ctx, &ctx->main_barrier_state);

  for (arg = 1; arg < 23; ++arg) {
    ctx->main_arg = arg;
    ctx->worker_arg = 0;

    sc = rtems_task_restart(id, arg);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    /* (B) */
    barrier(ctx, &ctx->main_barrier_state);

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

    /* (B) */
    barrier(ctx, &ctx->main_barrier_state);

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
  ISR_Level level;

  _ISR_Local_disable(level);

  /* (C) */
  barrier(ctx, &ctx->worker_barrier_state);

  /*
   * Interrupts are disabled, so the inter-processor interrupt deleting us will
   * be delayed a bit.
   */
  rtems_counter_delay_nanoseconds(100000000);

  if (variant != 0) {
    _Thread_Dispatch_disable();
  }

  _ISR_Local_enable(level);

  /*
   * We get deleted as a side effect of enabling the thread life protection or
   * later if we enable the thread dispatching.
   */
  _Thread_Set_life_protection( THREAD_LIFE_PROTECTED );

  if (variant != 0) {
    _Thread_Dispatch_enable( _Per_CPU_Get() );
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

  /* (C) */
  barrier(ctx, &ctx->main_barrier_state);

  sc = rtems_task_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));
}

static void delay_switch_task(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;

  ctx->delay_switch_for_executing = _Thread_Get_executing();

  /* (D) */
  barrier(ctx, &ctx->worker_barrier_state);

  rtems_task_exit();
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

  /* (D) */
  barrier(ctx, &ctx->main_barrier_state);

  /* (A) */
  barrier(ctx, &ctx->main_barrier_state);

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

typedef enum {
  TEST_OP_SUSPEND,
  TEST_OP_EVENT,
  TEST_OP_EVENT_SYSTEM
} test_op;

static void op_begin_suspend(void)
{
  rtems_status_code sc;

  sc = rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void op_begin_event(void)
{
  rtems_status_code sc;
  rtems_event_set events;

  events = 0;
  sc = rtems_event_receive(
    RTEMS_EVENT_0,
    RTEMS_EVENT_ALL | RTEMS_WAIT,
    RTEMS_NO_TIMEOUT,
    &events
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(events == RTEMS_EVENT_0);
}

static void op_begin_event_system(void)
{
  rtems_status_code sc;
  rtems_event_set events;

  events = 0;
  sc = rtems_event_system_receive(
    RTEMS_EVENT_0,
    RTEMS_EVENT_ALL | RTEMS_WAIT,
    RTEMS_NO_TIMEOUT,
    &events
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(events == RTEMS_EVENT_0);
}

static void (*const test_ops_begin[])(void) = {
  op_begin_suspend,
  op_begin_event,
  op_begin_event_system
};

static void op_end_suspend(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_task_resume(ctx->worker_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void op_end_event(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_event_send(ctx->worker_id, RTEMS_EVENT_0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void op_end_event_system(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_event_system_send(ctx->worker_id, RTEMS_EVENT_0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void (*const test_ops_end[])(test_context *) = {
  op_end_suspend,
  op_end_event,
  op_end_event_system
};

static void op_worker_task(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  test_op op = arg;
  Thread_Life_state previous_life_state;

  previous_life_state = _Thread_Set_life_protection(THREAD_LIFE_PROTECTED);

  /* (E) */
  barrier(ctx, &ctx->worker_barrier_state);

  /* (F) */
  barrier(ctx, &ctx->worker_barrier_state);

  (*test_ops_begin[op])();

  _Thread_Set_life_protection(previous_life_state);
  rtems_test_assert(0);
}

static void help_task(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  test_op op = arg;

  /* (F) */
  barrier(ctx, &ctx->main_barrier_state);

  rtems_counter_delay_nanoseconds(100000000);

  (*test_ops_end[op])(ctx);

  rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(0);
}

static void test_operation_with_delete_in_progress(test_op op)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  rtems_id help_id;
  rtems_resource_snapshot snapshot;

  rtems_resource_snapshot_take(&snapshot);

  sc = rtems_task_create(
    rtems_build_name('W', 'O', 'R', 'K'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->worker_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->worker_id, op_worker_task, op);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* (E) */
  barrier(ctx, &ctx->main_barrier_state);

  sc = rtems_task_create(
    rtems_build_name('H', 'E', 'L', 'P'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &help_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(help_id, help_task, op);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(ctx->worker_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(help_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  if (rtems_scheduler_get_processor_maximum() >= CPU_COUNT) {
    test_restart();
    test_delete();
    test_set_life_protection(0);
    test_set_life_protection(1);
    test_wait_for_execution_stop();
    test_operation_with_delete_in_progress(TEST_OP_SUSPEND);
    test_operation_with_delete_in_progress(TEST_OP_EVENT);
    test_operation_with_delete_in_progress(TEST_OP_EVENT_SYSTEM);
  }

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_MAXIMUM_TASKS (CPU_COUNT + 1)

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
