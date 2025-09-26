/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2013, 2016 embedded brains GmbH & Co. KG
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

#include <rtems/test-info.h>
#include <rtems/score/assert.h>
#include <rtems.h>

static void stop_worker_timer(rtems_id timer_id, void *arg)
{
  (void) timer_id;

  rtems_test_parallel_context *ctx = arg;

  _Atomic_Store_ulong(&ctx->stop, 1, ATOMIC_ORDER_RELAXED);
}

static void start_worker_stop_timer(
  rtems_test_parallel_context *ctx,
  rtems_interval duration
)
{
  rtems_status_code sc;

  _Atomic_Store_ulong(&ctx->stop, 0, ATOMIC_ORDER_RELEASE);

  sc = rtems_timer_fire_after(
    ctx->stop_worker_timer_id,
    duration,
    stop_worker_timer,
    ctx
  );
  _Assert(sc == RTEMS_SUCCESSFUL);
  (void) sc;
}

static void run_tests(
  rtems_test_parallel_context *ctx,
  const rtems_test_parallel_job *jobs,
  size_t job_count
)
{
  SMP_barrier_State bs = SMP_BARRIER_STATE_INITIALIZER;
  size_t i;

  _SMP_barrier_Wait(&ctx->barrier, &bs, ctx->worker_count);

  for (i = 0; i < job_count; ++i) {
    const rtems_test_parallel_job *job = &jobs[i];
    size_t n = rtems_scheduler_get_processor_maximum();
    size_t j = job->cascade ? 0 : rtems_scheduler_get_processor_maximum() - 1;

    while (j < n) {
      size_t active_worker = j + 1;
      size_t worker_index;
      rtems_interrupt_level level;

      /*
       * Determine worker index via the current processor index to get
       * consistent job runs with respect to the cache topology.
       */
      rtems_interrupt_local_disable(level);
      _SMP_barrier_Wait(&ctx->barrier, &bs, ctx->worker_count);
      worker_index = rtems_scheduler_get_processor();
      rtems_interrupt_local_enable(level);

      _Assert(worker_index < ctx->worker_count);

      if (rtems_test_parallel_is_master_worker(worker_index)) {
        rtems_interval duration = (*job->init)(ctx, job->arg, active_worker);

        if (duration > 0) {
          start_worker_stop_timer(ctx, duration);
        }
      }

      _SMP_barrier_Wait(&ctx->barrier, &bs, ctx->worker_count);

      if (worker_index <= j) {
        (*job->body)(ctx, job->arg, active_worker, worker_index);
      }

      _SMP_barrier_Wait(&ctx->barrier, &bs, ctx->worker_count);

      if (rtems_test_parallel_is_master_worker(worker_index)) {
        (*job->fini)(ctx, job->arg, active_worker);
      }

      _SMP_barrier_Wait(&ctx->barrier, &bs, ctx->worker_count);

      ++j;
    }
  }
}

static void worker_task(rtems_task_argument arg)
{
  rtems_test_parallel_context *ctx = (rtems_test_parallel_context *) arg;
  rtems_status_code sc;

  (void) sc;

  run_tests(ctx, ctx->jobs, ctx->job_count);

  while (true) {
    /* Wait for delete by master worker */
  }
}

void rtems_test_parallel(
  rtems_test_parallel_context *ctx,
  rtems_test_parallel_worker_setup worker_setup,
  const rtems_test_parallel_job *jobs,
  size_t job_count
)
{
  rtems_status_code sc;
  size_t worker_index;
  rtems_task_priority worker_priority;

  _Atomic_Init_ulong(&ctx->stop, 0);
  _SMP_barrier_Control_initialize(&ctx->barrier);
  ctx->worker_count = rtems_scheduler_get_processor_maximum();
  ctx->worker_ids[0] = rtems_task_self();
  ctx->jobs = jobs;
  ctx->job_count = job_count;

  if (RTEMS_ARRAY_SIZE(ctx->worker_ids) < ctx->worker_count) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }

  sc = rtems_task_set_priority(
    RTEMS_SELF,
    RTEMS_CURRENT_PRIORITY,
    &worker_priority
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }

  sc = rtems_timer_create(
    rtems_build_name('S', 'T', 'O', 'P'),
    &ctx->stop_worker_timer_id
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }

  for (worker_index = 1; worker_index < ctx->worker_count; ++worker_index) {
    rtems_id worker_id;

    sc = rtems_task_create(
      rtems_build_name('W', 'O', 'R', 'K'),
      worker_priority,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &worker_id
    );
    if (sc != RTEMS_SUCCESSFUL) {
      rtems_fatal_error_occurred(0xdeadbeef);
    }

    ctx->worker_ids[worker_index] = worker_id;

    if (worker_setup != NULL) {
      (*worker_setup)(ctx, worker_index, worker_id);
    }

    sc = rtems_task_start(worker_id, worker_task, (rtems_task_argument) ctx);
    _Assert(sc == RTEMS_SUCCESSFUL);
  }

  run_tests(ctx, jobs, job_count);

  for (worker_index = 1; worker_index < ctx->worker_count; ++worker_index) {
    sc = rtems_task_delete(ctx->worker_ids[worker_index]);
    _Assert(sc == RTEMS_SUCCESSFUL);
  }

  sc = rtems_timer_delete(ctx->stop_worker_timer_id);
  _Assert(sc == RTEMS_SUCCESSFUL);
}
