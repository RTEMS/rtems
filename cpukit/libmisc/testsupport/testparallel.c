/*
 * Copyright (c) 2013-2015 embedded brains GmbH.  All rights reserved.
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

#include <rtems/test.h>
#include <rtems/score/assert.h>
#include <rtems.h>

static void stop_worker_timer(rtems_id timer_id, void *arg)
{
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
  size_t job_count,
  size_t worker_index
)
{
  SMP_barrier_State bs = SMP_BARRIER_STATE_INITIALIZER;
  size_t i;

  for (i = 0; i < job_count; ++i) {
    const rtems_test_parallel_job *job = &jobs[i];
    size_t n = rtems_get_processor_count();
    size_t j = job->cascade ? 0 : rtems_get_processor_count() - 1;

    while (j < n) {
      size_t active_worker = j + 1;

      if (rtems_test_parallel_is_master_worker(worker_index)) {
        rtems_interval duration = (*job->init)(ctx, job->arg, active_worker);

        start_worker_stop_timer(ctx, duration);
      }

      _SMP_barrier_Wait(&ctx->barrier, &bs, ctx->worker_count);

      if (worker_index <= j) {
        (*job->body)(ctx, job->arg, active_worker, worker_index);
      }

      _SMP_barrier_Wait(&ctx->barrier, &bs, ctx->worker_count);

      if (rtems_test_parallel_is_master_worker(worker_index)) {
        (*job->fini)(ctx, job->arg, active_worker);
      }

      ++j;
    }
  }
}

typedef struct {
  rtems_test_parallel_context *ctx;
  const rtems_test_parallel_job *jobs;
  size_t job_count;
  size_t worker_index;
} worker_arg;

static void worker_task(rtems_task_argument arg)
{
  worker_arg warg = *(worker_arg *) arg;
  rtems_status_code sc;

  sc = rtems_event_transient_send(warg.ctx->worker_ids[0]);
  _Assert(sc == RTEMS_SUCCESSFUL);
  (void) sc;

  run_tests(warg.ctx, warg.jobs, warg.job_count, warg.worker_index);

  while (true) {
    /* Wait for delete by master worker */
  }
}

static char digit(size_t i, size_t pos)
{
  return '0' + (i / pos) % 10;
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
  ctx->worker_count = rtems_get_processor_count();
  ctx->worker_ids[0] = rtems_task_self();

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
    worker_arg warg = {
      .ctx = ctx,
      .jobs = jobs,
      .job_count = job_count,
      .worker_index = worker_index
    };
    rtems_id worker_id;

    sc = rtems_task_create(
      rtems_build_name(
        'W',
        digit(worker_index, 100),
        digit(worker_index, 10),
        digit(worker_index, 1)
      ),
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

    sc = rtems_task_start(worker_id, worker_task, (rtems_task_argument) &warg);
    _Assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    _Assert(sc == RTEMS_SUCCESSFUL);
  }

  run_tests(ctx, jobs, job_count, 0);

  for (worker_index = 1; worker_index < ctx->worker_count; ++worker_index) {
    sc = rtems_task_delete(ctx->worker_ids[worker_index]);
    _Assert(sc == RTEMS_SUCCESSFUL);
  }

  sc = rtems_timer_delete(ctx->stop_worker_timer_id);
  _Assert(sc == RTEMS_SUCCESSFUL);
}
