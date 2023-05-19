/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
