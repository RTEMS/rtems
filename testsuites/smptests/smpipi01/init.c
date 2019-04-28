/*
 * Copyright (c) 2014, 2019 embedded brains GmbH.  All rights reserved.
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

#include <rtems/score/smpimpl.h>
#include <rtems/score/smpbarrier.h>
#include <rtems.h>

#include <stdio.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPIPI 1";

#define CPU_COUNT 32

typedef struct {
  uint32_t value;
  uint32_t cache_line_separation[31];
} test_counter;

typedef struct {
  test_counter counters[CPU_COUNT];
  uint32_t copy_counters[CPU_COUNT];
  SMP_barrier_Control barrier;
  SMP_barrier_State main_barrier_state;
  SMP_barrier_State worker_barrier_state;
  Per_CPU_Job jobs[CPU_COUNT][2];
} test_context;

static test_context test_instance = {
  .barrier = SMP_BARRIER_CONTROL_INITIALIZER,
  .main_barrier_state = SMP_BARRIER_STATE_INITIALIZER,
  .worker_barrier_state = SMP_BARRIER_STATE_INITIALIZER
};

static void barrier(
  test_context *ctx,
  SMP_barrier_State *state
)
{
  _SMP_barrier_Wait(&ctx->barrier, state, 2);
}

static void barrier_1_handler(void *arg)
{
  test_context *ctx = arg;
  uint32_t cpu_index_self = _SMP_Get_current_processor();
  SMP_barrier_State *bs = &ctx->worker_barrier_state;

  ++ctx->counters[cpu_index_self].value;

  /* (D) */
  barrier(ctx, bs);
}

static const Per_CPU_Job_context barrier_1_job_context = {
  .handler = barrier_1_handler,
  .arg = &test_instance
};

static void barrier_0_handler(void *arg)
{
  test_context *ctx = arg;
  uint32_t cpu_index_self = _SMP_Get_current_processor();
  SMP_barrier_State *bs = &ctx->worker_barrier_state;

  ++ctx->counters[cpu_index_self].value;

  /* (A) */
  barrier(ctx, bs);

  /* (B) */
  barrier(ctx, bs);

  /* (C) */
  barrier(ctx, bs);

  ctx->jobs[0][1].context = &barrier_1_job_context;
  _Per_CPU_Add_job(_Per_CPU_Get(), &ctx->jobs[0][1]);
}

static const Per_CPU_Job_context barrier_0_job_context = {
  .handler = barrier_0_handler,
  .arg = &test_instance
};

static void test_send_message_while_processing_a_message(
  test_context *ctx,
  uint32_t cpu_index_self,
  uint32_t cpu_count
)
{
  SMP_barrier_State *bs = &ctx->main_barrier_state;
  uint32_t cpu_index;
  rtems_status_code sc;
  cpu_set_t cpuset;

  rtems_test_assert(cpu_index_self < CPU_SETSIZE);
  CPU_ZERO(&cpuset);
  CPU_SET((int) cpu_index_self, &cpuset);
  sc = rtems_task_set_affinity(RTEMS_SELF, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    if (cpu_index != cpu_index_self) {
      ctx->jobs[0][0].context = &barrier_0_job_context;
      _Per_CPU_Add_job(_Per_CPU_Get_by_index(cpu_index), &ctx->jobs[0][0]);
      _SMP_Send_message(cpu_index, SMP_MESSAGE_PERFORM_JOBS);

      /* (A) */
      barrier(ctx, bs);

      rtems_test_assert(ctx->counters[cpu_index].value == 1);
      _SMP_Send_message(cpu_index, SMP_MESSAGE_PERFORM_JOBS);

      /* (B) */
      barrier(ctx, bs);

      rtems_test_assert(ctx->counters[cpu_index].value == 1);

      /* (C) */
      barrier(ctx, bs);

      /* (D) */
      barrier(ctx, bs);

      rtems_test_assert(ctx->counters[cpu_index].value == 2);

      ctx->counters[cpu_index].value = 0;
    }
  }
}

static void counter_handler(void *arg, size_t next_job)
{
  test_context *ctx = arg;
  Per_CPU_Control *cpu_self = _Per_CPU_Get();
  uint32_t cpu_index_self = _Per_CPU_Get_index(cpu_self);

  ++ctx->counters[cpu_index_self].value;
  _Per_CPU_Add_job(cpu_self, &ctx->jobs[cpu_index_self][next_job]);
}

static void counter_0_handler(void *arg)
{
  counter_handler(arg, 1);
}

static const Per_CPU_Job_context counter_0_job_context = {
  .handler = counter_0_handler,
  .arg = &test_instance
};

static void counter_1_handler(void *arg)
{
  counter_handler(arg, 0);
}

static const Per_CPU_Job_context counter_1_job_context = {
  .handler = counter_1_handler,
  .arg = &test_instance
};

static void test_send_message_flood(
  test_context *ctx,
  uint32_t cpu_count
)
{
  uint32_t cpu_index_self = rtems_scheduler_get_processor();
  uint32_t cpu_index;

  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    Per_CPU_Control *cpu = _Per_CPU_Get_by_index(cpu_index);

    ctx->jobs[cpu_index][0].context = &counter_0_job_context;
    ctx->jobs[cpu_index][1].context = &counter_1_job_context;
    _Per_CPU_Add_job(cpu, &ctx->jobs[cpu_index][0]);
    _SMP_Send_message(cpu_index, SMP_MESSAGE_PERFORM_JOBS);
  }

  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    Per_CPU_Control *cpu_self;
    uint32_t i;

    cpu_self = _Thread_Dispatch_disable();
    _SMP_Synchronize();
    _Thread_Dispatch_enable(cpu_self);

    for (i = 0; i < cpu_count; ++i) {
      if (i != cpu_index) {
        ctx->copy_counters[i] = ctx->counters[i].value;
      }
    }

    for (i = 0; i < 100000; ++i) {
      _SMP_Send_message(cpu_index, SMP_MESSAGE_PERFORM_JOBS);
    }

    for (i = 0; i < cpu_count; ++i) {
      if (i != cpu_index) {
        rtems_test_assert(ctx->copy_counters[i] == ctx->counters[i].value);
      }
    }
  }

  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    rtems_test_assert(
      _Processor_mask_Is_set(_SMP_Get_online_processors(), cpu_index)
    );

    printf(
      "inter-processor interrupts for processor %"
        PRIu32 "%s: %" PRIu32 "\n",
      cpu_index,
      cpu_index == cpu_index_self ? " (main)" : "",
      ctx->counters[cpu_index].value
    );
  }

  for (; cpu_index < CPU_COUNT; ++cpu_index) {
    rtems_test_assert(
      !_Processor_mask_Is_set(_SMP_Get_online_processors(), cpu_index)
    );
  }
}

static void test(void)
{
  test_context *ctx = &test_instance;
  uint32_t cpu_count = rtems_scheduler_get_processor_maximum();
  uint32_t cpu_index_self;

  for (cpu_index_self = 0; cpu_index_self < cpu_count; ++cpu_index_self) {
    test_send_message_while_processing_a_message(ctx, cpu_index_self, cpu_count);
  }

  test_send_message_flood(ctx, cpu_count);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
