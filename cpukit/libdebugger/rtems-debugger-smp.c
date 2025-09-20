/*
 * Copyright (c) 2021 Kinsey Moore
 * Copyright (c) 2025 Chris Johns
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>

#include <rtems.h>
#include <rtems/score/cpu.h>
#include <rtems/score/threadimpl.h>

#include "rtems-debugger-smp.h"
#include "rtems-debugger-target.h"
#include "rtems-debugger-threads.h"

/*
 * Structure used to manage a task executing a function on available
 * cores on a scheduler.
 */
typedef struct {
  rtems_id all_cpus_barrier;
  rtems_task_entry worker;
  rtems_task_argument arg;
  rtems_status_code sc;
} rtems_debugger_cpu_run_context;

/*
 * The function that runs as the body of the task which moves itself
 * among the various cores registered to a scheduler.
 */
static rtems_task
rtems_debugger_cpu_run_body(rtems_task_argument arg)
{
  uint32_t                       released = 0;
  rtems_status_code              sc;
  rtems_debugger_cpu_run_context *ctx = (rtems_debugger_cpu_run_context *) arg;
  cpu_set_t                      set;
  cpu_set_t                      scheduler_set;
  rtems_id                       scheduler_id;

  sc = rtems_task_get_scheduler(RTEMS_SELF, &scheduler_id);

  if (sc != RTEMS_SUCCESSFUL) {
    ctx->sc = sc;
    rtems_task_exit();
  }

  CPU_ZERO(&scheduler_set);
  sc = rtems_scheduler_get_processor_set(
    scheduler_id, sizeof(scheduler_set), &scheduler_set);

  if (sc != RTEMS_SUCCESSFUL) {
    ctx->sc = sc;
    rtems_task_exit();
  }

  for (int cpu_index = 0;
       cpu_index < rtems_scheduler_get_processor_maximum();
       cpu_index++) {
    if (!CPU_ISSET(cpu_index, &scheduler_set)) {
      continue;
    }

    CPU_ZERO(&set);
    CPU_SET(cpu_index, &set);
    sc = rtems_task_set_affinity(RTEMS_SELF, sizeof(set), &set);

    if (sc != RTEMS_SUCCESSFUL) {
      ctx->sc = sc;
      rtems_task_exit();
    }

    /* execute task on selected CPU */
    ctx->worker(ctx->arg);
  }

  sc = rtems_barrier_release(ctx->all_cpus_barrier, &released);

  if (sc != RTEMS_SUCCESSFUL) {
    ctx->sc = sc;
  }

  rtems_task_exit();
}

/*
 * The function used to run a provided function with arbitrary argument across
 * all cores registered to the current scheduler. This is similar to the Linux
 * kernel's on_each_cpu() call and always waits for the task to complete before
 * returning.
 */
rtems_status_code
rtems_debugger_cpu_run_all(rtems_task_entry task_entry, rtems_task_argument arg)
{
  rtems_status_code              sc;
  rtems_id                       task_id;
  rtems_debugger_cpu_run_context ctx;

  memset(&ctx, 0, sizeof(ctx));

  ctx.worker = task_entry;
  ctx.arg = arg;
  ctx.sc = RTEMS_SUCCESSFUL;

  sc = rtems_barrier_create(
    rtems_build_name('D', 'B', 'b', 'r'), RTEMS_BARRIER_MANUAL_RELEASE, 2,
    &ctx.all_cpus_barrier);

  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  sc = rtems_task_create(
    rtems_build_name('D', 'B', 't', 'k'), 1, RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_DEFAULT_MODES, RTEMS_FLOATING_POINT | RTEMS_DEFAULT_ATTRIBUTES, &task_id);

  if (sc != RTEMS_SUCCESSFUL) {
    rtems_barrier_delete(ctx.all_cpus_barrier);
    return sc;
  }

  sc = rtems_task_start(
    task_id, rtems_debugger_cpu_run_body, (rtems_task_argument) &ctx);

  if (sc != RTEMS_SUCCESSFUL) {
    rtems_task_delete(task_id);
    rtems_barrier_delete(ctx.all_cpus_barrier);
    return sc;
  }

  /* wait on task */
  sc = rtems_barrier_wait(ctx.all_cpus_barrier, RTEMS_NO_TIMEOUT);

  if (sc != RTEMS_SUCCESSFUL) {
    rtems_task_delete(task_id);
    rtems_barrier_delete(ctx.all_cpus_barrier);
    return sc;
  }

  rtems_barrier_delete(ctx.all_cpus_barrier);

  if (ctx.sc != RTEMS_SUCCESSFUL) {
    return ctx.sc;
  }

  return sc;
}
