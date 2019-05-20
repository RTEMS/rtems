/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019 embedded brains GmbH
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

#include <rtems/score/smpimpl.h>
#include <rtems/score/atomic.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/sysinit.h>
#include <rtems.h>

#include <string.h>

#include <t.h>
#include <tmacros.h>

#define CPU_COUNT 32

const char rtems_test_name[] = "SMPMULTICAST 1";

static const T_config config = {
  .name = "SMPMultiCast",
  .putchar = T_putchar_default,
  .verbosity = T_VERBOSE,
  .now = T_now
};

typedef struct {
  rtems_test_parallel_context base;
  Atomic_Uint id[CPU_COUNT][CPU_COUNT];
} test_context;

static test_context test_instance;

static void clear_ids_by_worker(test_context *ctx, size_t worker_index)
{
  memset(&ctx->id[worker_index][0], 0, sizeof(ctx->id[worker_index]));
}

static void unicast_action_irq_disabled(
  uint32_t cpu_index,
  SMP_Action_handler handler,
  void *arg
)
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  _SMP_Unicast_action(cpu_index, handler, arg);
  rtems_interrupt_local_enable(level);
}

static void unicast_action_dispatch_disabled(
  uint32_t cpu_index,
  SMP_Action_handler handler,
  void *arg
)
{
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();
  _SMP_Unicast_action(cpu_index, handler, arg);
  _Thread_Dispatch_enable(cpu_self);
}

static void multicast_action_irq_disabled(
  const Processor_mask *targets,
  SMP_Action_handler handler,
  void *arg
)
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  _SMP_Multicast_action(targets, handler, arg);
  rtems_interrupt_local_enable(level);
}

static void multicast_action_dispatch_disabled(
  const Processor_mask *targets,
  SMP_Action_handler handler,
  void *arg
)
{
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();
  _SMP_Multicast_action(targets, handler, arg);
  _Thread_Dispatch_enable(cpu_self);
}

static void broadcast_action_irq_disabled(
  SMP_Action_handler handler,
  void *arg
)
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  _SMP_Broadcast_action(handler, arg);
  rtems_interrupt_local_enable(level);
}

static void broadcast_action_dispatch_disabled(
  SMP_Action_handler handler,
  void *arg
)
{
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();
  _SMP_Broadcast_action(handler, arg);
  _Thread_Dispatch_enable(cpu_self);
}

static void action(void *arg)
{
  Atomic_Uint *id;
  uint32_t self;
  unsigned expected;
  bool success;

  id = arg;
  self = rtems_scheduler_get_processor();
  expected = 0;
  success = _Atomic_Compare_exchange_uint(
    &id[self],
    &expected,
    self + 1,
    ATOMIC_ORDER_RELAXED,
    ATOMIC_ORDER_RELAXED
  );
  T_quiet_true(success, "set CPU identifier failed");
}

static void test_unicast(
  test_context *ctx,
  void (*unicast_action)(uint32_t, SMP_Action_handler, void *)
)
{
  uint32_t step;
  uint32_t i;
  uint32_t n;

  T_plan(1);
  step = 0;
  n = rtems_scheduler_get_processor_maximum();

  for (i = 0; i < n; ++i) {
    uint32_t j;

    clear_ids_by_worker(ctx, 0);

    (*unicast_action)(i, action, &ctx->id[0][0]);

    for (j = 0; j < n; ++j) {
      unsigned id;

      ++step;
      id = _Atomic_Load_uint(&ctx->id[0][j], ATOMIC_ORDER_RELAXED);

      if (j == i) {
        T_quiet_eq_uint(j + 1, id);
      } else {
        T_quiet_eq_uint(0, id);
      }
    }
  }

  T_step_eq_u32(0, step, n * n);
}

static void test_multicast(
  test_context *ctx,
  void (*multicast_action)(const Processor_mask *, SMP_Action_handler, void *)
)
{
  uint32_t step;
  uint32_t i;
  uint32_t n;

  T_plan(1);
  step = 0;
  n = rtems_scheduler_get_processor_maximum();

  for (i = 0; i < n; ++i) {
    Processor_mask cpus;
    uint32_t j;

    clear_ids_by_worker(ctx, 0);

    _Processor_mask_Zero(&cpus);
    _Processor_mask_Set(&cpus, i);
    (*multicast_action)(&cpus, action, &ctx->id[0][0]);

    for (j = 0; j < n; ++j) {
      unsigned id;

      ++step;
      id = _Atomic_Load_uint(&ctx->id[0][j], ATOMIC_ORDER_RELAXED);

      if (j == i) {
        T_quiet_eq_uint(j + 1, id);
      } else {
        T_quiet_eq_uint(0, id);
      }
    }
  }

  T_step_eq_u32(0, step, n * n);
}

static void test_broadcast(
  test_context *ctx,
  void (*broadcast_action)(SMP_Action_handler, void *)
)
{
  uint32_t step;
  uint32_t i;
  uint32_t n;

  T_plan(1);
  step = 0;
  n = rtems_scheduler_get_processor_maximum();

  for (i = 0; i < n; ++i) {
    uint32_t j;

    clear_ids_by_worker(ctx, 0);

    (*broadcast_action)(action, &ctx->id[0][0]);

    for (j = 0; j < n; ++j) {
      unsigned id;

      ++step;
      id = _Atomic_Load_uint(&ctx->id[0][j], ATOMIC_ORDER_RELAXED);
      T_quiet_eq_uint(j + 1, id);
    }
  }

  T_step_eq_u32(0, step, n * n);
}

static rtems_interval test_duration(void)
{
  return rtems_clock_get_ticks_per_second();
}

static rtems_interval test_broadcast_init(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  return test_duration();
}

static void test_broadcast_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx;

  ctx = (test_context *) base;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    Per_CPU_Control *cpu_self;

    clear_ids_by_worker(ctx, worker_index);
    cpu_self = _Thread_Dispatch_disable();
    _SMP_Multicast_action(NULL, action, &ctx->id[worker_index][0]);
    _Thread_Dispatch_enable(cpu_self);
  }
}

static void test_broadcast_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  /* Do nothing */
}

static const rtems_test_parallel_job test_jobs[] = {
  {
    .init = test_broadcast_init,
    .body = test_broadcast_body,
    .fini = test_broadcast_fini,
    .cascade = true
  }
};

T_TEST_CASE(ParallelBroadcast)
{
  rtems_test_parallel(
    &test_instance.base,
    NULL,
    &test_jobs[0],
    RTEMS_ARRAY_SIZE(test_jobs)
  );
}

static void test_before_multitasking(void)
{
  test_context *ctx;

  ctx = &test_instance;

  T_case_begin("UnicastBeforeMultitasking", NULL);
  test_unicast(ctx, _SMP_Unicast_action);
  T_case_end();

  T_case_begin("UnicastBeforeMultitaskingIRQDisabled", NULL);
  test_unicast(ctx, unicast_action_irq_disabled);
  T_case_end();

  T_case_begin("UnicastBeforeMultitaskingDispatchDisabled", NULL);
  test_unicast(ctx, unicast_action_dispatch_disabled);
  T_case_end();

  T_case_begin("MulticastBeforeMultitasking", NULL);
  test_multicast(ctx, _SMP_Multicast_action);
  T_case_end();

  T_case_begin("MulticastBeforeMultitaskingIRQDisabled", NULL);
  test_multicast(ctx, multicast_action_irq_disabled);
  T_case_end();

  T_case_begin("MulticastBeforeMultitaskingDispatchDisabled", NULL);
  test_multicast(ctx, multicast_action_dispatch_disabled);
  T_case_end();

  T_case_begin("BroadcastBeforeMultitasking", NULL);
  test_broadcast(ctx, _SMP_Broadcast_action);
  T_case_end();

  T_case_begin("BroadcastBeforeMultitaskingIRQDisabled", NULL);
  test_broadcast(ctx, broadcast_action_irq_disabled);
  T_case_end();

  T_case_begin("BroadcastBeforeMultitaskingDispatchDisabled", NULL);
  test_broadcast(ctx, broadcast_action_dispatch_disabled);
  T_case_end();
}

static void after_drivers(void)
{
  TEST_BEGIN();
  T_run_initialize(&config);
  test_before_multitasking();
}

RTEMS_SYSINIT_ITEM(
  after_drivers,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_LAST
);

static void set_wrong_cpu_state(void *arg)
{
  Per_CPU_Control *cpu_self;

  cpu_self = arg;
  T_step_eq_ptr(0, cpu_self, _Per_CPU_Get());
  cpu_self->state = 123;

  while (true) {
    /* Do nothing */
  }
}

static void test_wrong_cpu_state_to_perform_jobs(void)
{
  Per_CPU_Control *cpu_self;
  rtems_interrupt_level level;
  Processor_mask targets;
  uint32_t cpu_index;

  T_case_begin("WrongCPUStateToPerformJobs", NULL);
  T_plan(4);
  cpu_self = _Thread_Dispatch_disable();

  cpu_index = _Per_CPU_Get_index(cpu_self);
  cpu_index = (cpu_index + 1) % rtems_scheduler_get_processor_maximum();
  _Processor_mask_Zero(&targets);
  _Processor_mask_Set(&targets, cpu_index);

  rtems_interrupt_local_disable(level);

  _SMP_Multicast_action(
    &targets,
    set_wrong_cpu_state,
    _Per_CPU_Get_by_index(cpu_index)
  );

  /* If everything is all right, we don't end up here */
  rtems_interrupt_local_enable(level);
  _Thread_Dispatch_enable(cpu_self);
  rtems_fatal(RTEMS_FATAL_SOURCE_APPLICATION, 0);
}

#define TEST_JOB_ORDER_JOBS 3

static Per_CPU_Job job_order_jobs[TEST_JOB_ORDER_JOBS];

static void job_order_handler_0(void *arg)
{
  T_step(1, "invalid job order");
}

static void job_order_handler_1(void *arg)
{
  T_step(2, "invalid job order");
}

static void job_order_handler_2(void *arg)
{
  T_step(3, "invalid job order");
}

static const Per_CPU_Job_context job_order_contexts[TEST_JOB_ORDER_JOBS] = {
  { .handler = job_order_handler_0 },
  { .handler = job_order_handler_1 },
  { .handler = job_order_handler_2 }
};

T_TEST_CASE(JobOrder)
{
  Per_CPU_Control *cpu_self;
  size_t i;

  T_plan(4);
  cpu_self = _Thread_Dispatch_disable();

  for (i = 0; i < TEST_JOB_ORDER_JOBS; ++i) {
    job_order_jobs[i].context = &job_order_contexts[i];
    _Per_CPU_Add_job(cpu_self, &job_order_jobs[i]);
  }

  T_step(0, "wrong job processing time");
  _SMP_Send_message(_Per_CPU_Get_index(cpu_self), SMP_MESSAGE_PERFORM_JOBS);
  _Thread_Dispatch_enable(cpu_self);
}

#define TEST_ADD_JOB_IN_JOB_JOBS 3

static Per_CPU_Job add_job_in_job_jobs[TEST_ADD_JOB_IN_JOB_JOBS];

static void add_job_in_job_handler_0(void *arg)
{
  T_step(1, "invalid job order");
  _Per_CPU_Add_job(_Per_CPU_Get(), &add_job_in_job_jobs[1]);
}

static void add_job_in_job_handler_1(void *arg)
{
  T_step(3, "invalid job order");
}

static const Per_CPU_Job_context
add_job_in_job_contexts[TEST_ADD_JOB_IN_JOB_JOBS] = {
  { .handler = add_job_in_job_handler_0 },
  { .handler = add_job_in_job_handler_1 }
};

T_TEST_CASE(AddJobInJob)
{
  Per_CPU_Control *cpu_self;
  size_t i;

  T_plan(4);
  cpu_self = _Thread_Dispatch_disable();

  for (i = 0; i < TEST_ADD_JOB_IN_JOB_JOBS; ++i) {
    add_job_in_job_jobs[i].context = &add_job_in_job_contexts[i];
  }

  _Per_CPU_Add_job(cpu_self, &add_job_in_job_jobs[0]);
  T_step(0, "wrong job processing time");
  _SMP_Send_message(_Per_CPU_Get_index(cpu_self), SMP_MESSAGE_PERFORM_JOBS);
  T_step(2, "wrong job processing time");
  _SMP_Send_message(_Per_CPU_Get_index(cpu_self), SMP_MESSAGE_PERFORM_JOBS);
  _Thread_Dispatch_enable(cpu_self);
}

T_TEST_CASE(UnicastDuringMultitaskingIRQDisabled)
{
  test_unicast(&test_instance, unicast_action_irq_disabled);
}

T_TEST_CASE(UnicastDuringMultitaskingDispatchDisabled)
{
  test_unicast(&test_instance, unicast_action_dispatch_disabled);
}

T_TEST_CASE(MulticastDuringMultitaskingIRQDisabled)
{
  test_multicast(&test_instance, multicast_action_irq_disabled);
}

T_TEST_CASE(MulticastDuringMultitaskingDispatchDisabled)
{
  test_multicast(&test_instance, multicast_action_dispatch_disabled);
}

T_TEST_CASE(BroadcastDuringMultitaskingIRQDisabled)
{
  test_broadcast(&test_instance, broadcast_action_irq_disabled);
}

T_TEST_CASE(BroadcastDuringMultitaskingDispatchDisabled)
{
  test_broadcast(&test_instance, broadcast_action_dispatch_disabled);
}

static void Init(rtems_task_argument arg)
{
  T_register();
  T_run_all();

  if (rtems_scheduler_get_processor_maximum() > 1) {
    test_wrong_cpu_state_to_perform_jobs();
  } else {
    rtems_fatal(RTEMS_FATAL_SOURCE_APPLICATION, 0);
  }
}

static void fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code code
)
{
  bool ok;

  if (source == RTEMS_FATAL_SOURCE_SMP) {
    T_step_eq_int(1, source, RTEMS_FATAL_SOURCE_SMP);
    T_step_false(2, always_set_to_false, "unexpected argument value");
    T_step_eq_int(3, code, SMP_FATAL_WRONG_CPU_STATE_TO_PERFORM_JOBS);
    T_case_end();

    ok = T_run_finalize();
    rtems_test_assert(ok);
    TEST_END();
  } else if (source == RTEMS_FATAL_SOURCE_APPLICATION) {
    ok = T_run_finalize();
    rtems_test_assert(ok);
    TEST_END();
  }
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS CPU_COUNT

#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
