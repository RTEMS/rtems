/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2014, 2024 embedded brains GmbH & Co. KG
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
  Per_CPU_Job sync_jobs[2];
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
      Per_CPU_Control *cpu_self;

      ctx->jobs[0][0].context = &barrier_0_job_context;
      _Per_CPU_Submit_job(_Per_CPU_Get_by_index(cpu_index), &ctx->jobs[0][0]);

      /* (A) */
      barrier(ctx, bs);

      rtems_test_assert(ctx->counters[cpu_index].value == 1);
      _SMP_Send_message(
        _Per_CPU_Get_by_index(cpu_index),
        SMP_MESSAGE_PERFORM_JOBS
      );

      /* (B) */
      barrier(ctx, bs);

      rtems_test_assert(ctx->counters[cpu_index].value == 1);

      /* (C) */
      barrier(ctx, bs);

      /* (D) */
      barrier(ctx, bs);

      rtems_test_assert(ctx->counters[cpu_index].value == 2);

      ctx->counters[cpu_index].value = 0;

      /* Ensure that the second job is done and can be reused */
      cpu_self = _Thread_Dispatch_disable();
      _Per_CPU_Wait_for_job(_Per_CPU_Get_by_index(cpu_index), &ctx->jobs[0][1]);
      _Thread_Dispatch_enable(cpu_self);
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

static void sync_0_handler(void *arg)
{
  test_context *ctx = arg;

  _Per_CPU_Submit_job(_Per_CPU_Get(), &ctx->sync_jobs[1]);

  /* (E) */
  barrier(ctx, &ctx->worker_barrier_state);
}

static void sync_1_handler(void *arg)
{
  test_context *ctx = arg;

  /* (F) */
  barrier(ctx, &ctx->worker_barrier_state);
}

static const Per_CPU_Job_context sync_0_context = {
  .handler = sync_0_handler,
  .arg = &test_instance
};

static const Per_CPU_Job_context sync_1_context = {
  .handler = sync_1_handler,
  .arg = &test_instance
};

static void wait_for_ipi_done(test_context *ctx, Per_CPU_Control *cpu)
{
  unsigned long done;

  ctx->sync_jobs[0].context = &sync_0_context;
  ctx->sync_jobs[1].context = &sync_1_context;
  _Per_CPU_Submit_job(cpu, &ctx->sync_jobs[0]);

  /*
   * (E)
   *
   * At this point, the IPI is currently serviced.  Depending on the target and
   * timing conditions, the IPI may be active and pending.  The main processor
   * will no longer make this IPI pending after this point.  Let the
   * sync_0_handler() make it pending again to go to (F).
   */
  barrier(ctx, &ctx->main_barrier_state);

  /* (F) */
  barrier(ctx, &ctx->main_barrier_state);

  /* Make sure that a potential counter_handler() finished */
  while (cpu->isr_nest_level != 0) {
    RTEMS_COMPILER_MEMORY_BARRIER();
  }

  done = _Atomic_Load_ulong( &ctx->sync_jobs[1].done, ATOMIC_ORDER_ACQUIRE );
  rtems_test_assert( done == PER_CPU_JOB_DONE );
}

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
  }

  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    Per_CPU_Control *cpu;
    uint32_t i;

    cpu = _Per_CPU_Get_by_index(cpu_index);

    for (i = 0; i < cpu_count; ++i) {
      if (i != cpu_index) {
        ctx->copy_counters[i] = ctx->counters[i].value;
      }
    }

    for (i = 0; i < 100000; ++i) {
      _SMP_Send_message(cpu, SMP_MESSAGE_PERFORM_JOBS);
    }

    if (cpu_index != cpu_index_self) {
      wait_for_ipi_done(ctx, cpu);
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
  (void) arg;

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
