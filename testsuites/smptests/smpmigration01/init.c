/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "tmacros.h"

#include <stdio.h>
#include <inttypes.h>

#define CPU_COUNT 2

#define RUNNER_COUNT (CPU_COUNT + 1)

#define PRIO_STOP 2

#define PRIO_HIGH 3

#define PRIO_NORMAL 4

/* FIXME: Use atomic operations instead of volatile */

typedef struct {
  uint32_t counter;
  uint32_t unused_space_for_cache_line_alignment[7];
} cache_aligned_counter;

typedef struct {
  cache_aligned_counter tokens_per_cpu[CPU_COUNT];
  volatile cache_aligned_counter cycles_per_cpu[CPU_COUNT];
} test_counters;

typedef struct {
  test_counters counters[RUNNER_COUNT];
  volatile rtems_task_argument token;
  rtems_id runner_ids[RUNNER_COUNT];
} test_context;

CPU_STRUCTURE_ALIGNMENT static test_context ctx_instance;

static void change_prio(rtems_id task, rtems_task_priority prio)
{
  rtems_status_code sc;
  rtems_task_priority unused;

  sc = rtems_task_set_priority(task, prio, &unused);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void runner(rtems_task_argument self)
{
  test_context *ctx = &ctx_instance;
  rtems_task_argument next = (self + 1) % RUNNER_COUNT;
  rtems_id next_runner = ctx->runner_ids[next];
  test_counters *counters = &ctx->counters[self];
  test_counters *next_counters = &ctx->counters[next];

  while (true) {
    uint32_t current_cpu = rtems_smp_get_current_processor();

    ++counters->cycles_per_cpu[current_cpu].counter;

    if (ctx->token == self) {
      uint32_t other_cpu = (current_cpu + 1) % CPU_COUNT;
      uint32_t snapshot;

      ++counters->tokens_per_cpu[current_cpu].counter;

      change_prio(next_runner, PRIO_HIGH);

      snapshot = next_counters->cycles_per_cpu[other_cpu].counter;
      while (next_counters->cycles_per_cpu[other_cpu].counter == snapshot) {
        /* Wait for other thread to resume execution */
      }

      ctx->token = next;

      change_prio(RTEMS_SELF, PRIO_NORMAL);
    }
  }
}

static void stopper(rtems_task_argument arg)
{
  (void) arg;

  while (true) {
    /* Do nothing */
  }
}

static void test(void)
{
  test_context *ctx = &ctx_instance;
  rtems_status_code sc;
  rtems_task_argument runner_index;
  rtems_id stopper_id;
  uint32_t expected_tokens;
  uint32_t total_delta;

  sc = rtems_task_create(
    rtems_build_name('S', 'T', 'O', 'P'),
    PRIO_STOP,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &stopper_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (runner_index = 0; runner_index < RUNNER_COUNT; ++runner_index) {
    sc = rtems_task_create(
      rtems_build_name('R', 'U', 'N', (char) ('0' + runner_index)),
      PRIO_HIGH + runner_index,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &ctx->runner_ids[runner_index]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  for (runner_index = 0; runner_index < RUNNER_COUNT; ++runner_index) {
    sc = rtems_task_start(ctx->runner_ids[runner_index], runner, runner_index);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  sc = rtems_task_wake_after(10 * rtems_clock_get_ticks_per_second());
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(stopper_id, stopper, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (runner_index = 0; runner_index < RUNNER_COUNT; ++runner_index) {
    test_counters *counters = &ctx->counters[runner_index];
    size_t cpu;

    printf("runner %" PRIuPTR "\n", runner_index);

    for (cpu = 0; cpu < CPU_COUNT; ++cpu) {
      printf(
        "\tcpu %zu tokens %" PRIu32 "\n"
        "\tcpu %zu cycles %" PRIu32 "\n",
        cpu,
        counters->tokens_per_cpu[cpu].counter,
        cpu,
        counters->cycles_per_cpu[cpu].counter
      );
    }
  }

  expected_tokens = ctx->counters[0].tokens_per_cpu[0].counter;
  total_delta = 0;
  for (runner_index = 0; runner_index < RUNNER_COUNT; ++runner_index) {
    test_counters *counters = &ctx->counters[runner_index];
    size_t cpu;

    for (cpu = 0; cpu < CPU_COUNT; ++cpu) {
      uint32_t tokens = counters->tokens_per_cpu[cpu].counter;
      uint32_t delta = tokens > expected_tokens ?
        tokens - expected_tokens : expected_tokens - tokens;

      rtems_test_assert(delta <= 1);

      total_delta += delta;
    }
  }

  rtems_test_assert(total_delta <= (RUNNER_COUNT * CPU_COUNT - 1));
}

static void Init(rtems_task_argument arg)
{
  puts("\n\n*** TEST SMPMIGRATION 1 ***");

  if (rtems_smp_get_processor_count() >= 2) {
    test();
  }

  puts("*** END OF TEST SMPMIGRATION 1 ***");

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_SMP_APPLICATION

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_MAXIMUM_TASKS (2 + RUNNER_COUNT)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
