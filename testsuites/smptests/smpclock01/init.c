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

#include "tmacros.h"

#include <rtems.h>
#include <rtems/libcsupport.h>
#include <rtems/score/percpu.h>
#include <rtems/score/smpbarrier.h>

#include <test_support.h>

#define CPU_COUNT 2

#define SCHEDULER_A rtems_build_name(' ', ' ', ' ', 'A')

#define SCHEDULER_B rtems_build_name(' ', ' ', ' ', 'B')

const char rtems_test_name[] = "SMPCLOCK 1";

typedef struct {
  SMP_barrier_Control barrier;
  SMP_barrier_State delay_barrier_state;
  SMP_barrier_State timer_barrier_state;
} test_context;

static test_context test_instance = {
  .barrier = SMP_BARRIER_CONTROL_INITIALIZER,
  .delay_barrier_state = SMP_BARRIER_STATE_INITIALIZER,
  .timer_barrier_state = SMP_BARRIER_STATE_INITIALIZER
};

static void wait(test_context *ctx, SMP_barrier_State *bs)
{
  _SMP_barrier_Wait(&ctx->barrier, bs, CPU_COUNT);
}

static void timer_isr(rtems_id id, void *arg)
{
  test_context *ctx = arg;

  /* (B) */
  wait(ctx, &ctx->timer_barrier_state);
}

static void timer_task(rtems_task_argument arg)
{
  test_context *ctx = (test_context *) arg;
  rtems_status_code sc;
  rtems_id timer_id;

  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  sc = rtems_timer_create(SCHEDULER_B, &timer_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* (A) */
  wait(ctx, &ctx->timer_barrier_state);

  sc = rtems_timer_fire_after(timer_id, 1, timer_isr, ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_wake_after(1);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_timer_delete(timer_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* (C) */
  wait(ctx, &ctx->timer_barrier_state);

  while (true) {
    /* Wait for deletion */
  }
}

static void delay_clock_tick(test_context *ctx)
{
  rtems_interrupt_level level;
  const Per_CPU_Control *cpu_self = _Per_CPU_Get_by_index(0);
  const Per_CPU_Control *cpu_other = _Per_CPU_Get_by_index(1);
  uint64_t ticks;

  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  rtems_test_spin_until_next_tick();
  ticks = cpu_self->Watchdog.ticks;

  rtems_interrupt_local_disable(level);

  /* (A) */
  wait(ctx, &ctx->delay_barrier_state);

  /* (B) */
  wait(ctx, &ctx->delay_barrier_state);

  rtems_test_assert(cpu_self->Watchdog.ticks == ticks);
  rtems_test_assert(cpu_other->Watchdog.ticks == ticks + 1);

  rtems_interrupt_local_enable(level);

  rtems_test_assert(cpu_self->Watchdog.ticks == ticks + 1);
  rtems_test_assert(cpu_other->Watchdog.ticks == ticks + 1);

  /* (C) */
  wait(ctx, &ctx->delay_barrier_state);
}

static void test(void)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  rtems_id scheduler_b_id;
  rtems_id task_id;

  sc = rtems_scheduler_ident(SCHEDULER_B, &scheduler_b_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    SCHEDULER_B,
    255,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_scheduler(task_id, scheduler_b_id, 1);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(task_id, timer_task, (rtems_task_argument) ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  delay_clock_tick(ctx);

  sc = rtems_task_delete(task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void Init(rtems_task_argument arg)
{
  rtems_resource_snapshot snapshot;

  TEST_BEGIN();

  rtems_resource_snapshot_take(&snapshot);

  if (rtems_scheduler_get_processor_maximum() == CPU_COUNT) {
    test();
  }

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_SCHEDULER_SIMPLE_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_SIMPLE_SMP(a);
RTEMS_SCHEDULER_SIMPLE_SMP(b);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(a, SCHEDULER_A), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(b, SCHEDULER_B)

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL)

#define CONFIGURE_MAXIMUM_TASKS CPU_COUNT

#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
