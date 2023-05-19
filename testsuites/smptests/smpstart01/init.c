/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2022 embedded brains GmbH & Co. KG
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

#include <rtems/score/percpu.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/smpbarrier.h>
#include <rtems.h>
#include <rtems/sysinit.h>

#include <tmacros.h>

const char rtems_test_name[] = "SMPSTART 1";

typedef struct {
  const rtems_tcb *main_tcb;
  SMP_barrier_Control barrier;
  Per_CPU_Job job;
} test_context;

static test_context test_instance;

static void barrier(test_context *ctx)
{
  SMP_barrier_State bs;

  _SMP_barrier_State_initialize(&bs);
  _SMP_barrier_Wait(&ctx->barrier, &bs, 2);
}

static void prepare_second_cpu(void *arg)
{
  test_context *ctx;
  Per_CPU_Control *cpu_self;

  ctx = arg;
  cpu_self = _Per_CPU_Get();

  barrier(ctx);

  while (cpu_self->heir != ctx->main_tcb) {
    RTEMS_COMPILER_MEMORY_BARRIER();
  }
}

static const Per_CPU_Job_context job_context = {
  .handler = prepare_second_cpu,
  .arg = &test_instance
};

static void submit_job(void)
{
  test_context *ctx;

  ctx = &test_instance;
  _SMP_barrier_Control_initialize(&ctx->barrier);
  ctx->job.context = &job_context;

  _Per_CPU_Submit_job(_Per_CPU_Get_by_index(1), &ctx->job);
}

RTEMS_SYSINIT_ITEM(
  submit_job,
  RTEMS_SYSINIT_LAST,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

static void test(void)
{
  test_context *ctx;
  rtems_status_code sc;
  rtems_id id;

  ctx = &test_instance;
  ctx->main_tcb = _Thread_Get_executing();

  barrier(ctx);

  sc = rtems_scheduler_ident_by_processor(1, &id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_scheduler(RTEMS_SELF, id, 1);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_scheduler_get_processor() == 1);
  rtems_test_assert(_ISR_Get_level() == 0);
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

#define CONFIGURE_MAXIMUM_PROCESSORS 2

#define CONFIGURE_SCHEDULER_EDF_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_EDF_SMP(a);

RTEMS_SCHEDULER_EDF_SMP(b);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(a, rtems_build_name('A', ' ', ' ', ' ')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(b, rtems_build_name('B', ' ', ' ', ' '))

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY)

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
