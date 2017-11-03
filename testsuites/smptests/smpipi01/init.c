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

#include <rtems/score/smpimpl.h>
#include <rtems/score/smpbarrier.h>
#include <rtems/counter.h>
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

static void barrier_handler(Per_CPU_Control *cpu_self)
{
  test_context *ctx = &test_instance;
  uint32_t cpu_index_self = _Per_CPU_Get_index(cpu_self);
  SMP_barrier_State *bs = &ctx->worker_barrier_state;

  ++ctx->counters[cpu_index_self].value;

  /* (A) */
  barrier(ctx, bs);

  /* (B) */
  barrier(ctx, bs);

  /* (C) */
  barrier(ctx, bs);
}

static void test_send_message_while_processing_a_message(
  test_context *ctx
)
{
  uint32_t cpu_count = rtems_get_processor_count();
  uint32_t cpu_index_self = rtems_get_current_processor();
  uint32_t cpu_index;
  SMP_barrier_State *bs = &ctx->main_barrier_state;

  _SMP_Set_test_message_handler(barrier_handler);

  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    if (cpu_index != cpu_index_self) {
      _SMP_Send_message(cpu_index, SMP_MESSAGE_TEST);

      /* (A) */
      barrier(ctx, bs);

      rtems_test_assert(ctx->counters[cpu_index].value == 1);
      _SMP_Send_message(cpu_index, SMP_MESSAGE_TEST);

      /* (B) */
      barrier(ctx, bs);

      rtems_test_assert(ctx->counters[cpu_index].value == 1);

      /* (C) */
      barrier(ctx, bs);

      /* (A) */
      barrier(ctx, bs);

      rtems_test_assert(ctx->counters[cpu_index].value == 2);

      /* (B) */
      barrier(ctx, bs);

      /* (C) */
      barrier(ctx, bs);

      ctx->counters[cpu_index].value = 0;
    }
  }
}

static void counter_handler(Per_CPU_Control *cpu_self)
{
  test_context *ctx = &test_instance;
  uint32_t cpu_index_self = _Per_CPU_Get_index(cpu_self);

  ++ctx->counters[cpu_index_self].value;
}

static void test_send_message_flood(
  test_context *ctx
)
{
  uint32_t cpu_count = rtems_get_processor_count();
  uint32_t cpu_index_self = rtems_get_current_processor();
  uint32_t cpu_index;

  _SMP_Set_test_message_handler(counter_handler);

  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    uint32_t i;

    /* Wait 1us so that all outstanding messages have been processed */
    rtems_counter_delay_nanoseconds(1000000);

    for (i = 0; i < cpu_count; ++i) {
      if (i != cpu_index) {
        ctx->copy_counters[i] = ctx->counters[i].value;
      }
    }

    for (i = 0; i < 100000; ++i) {
      _SMP_Send_message(cpu_index, SMP_MESSAGE_TEST);
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

  test_send_message_while_processing_a_message(ctx);
  test_send_message_flood(ctx);
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
