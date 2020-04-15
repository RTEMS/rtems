/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#include <rtems.h>
#include <rtems/libcsupport.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPMUTEX 2";

#define MTX_PER_CPU 12

#define WORKER_PER_CPU 4

#define CPU_COUNT 32

#define MTX_COUNT (CPU_COUNT * MTX_PER_CPU)

#define WORKER_COUNT (CPU_COUNT * WORKER_PER_CPU)

typedef struct {
  uint32_t obtain_counter;
  uint32_t deadlock_counter;
  uint32_t timeout_counter;
  uint32_t release_counter;
  uint32_t max_nest_level;
} test_stats;

typedef struct {
  uint32_t cpu_count;
  uint32_t mtx_count;
  rtems_id worker_ids[CPU_COUNT][WORKER_PER_CPU];
  rtems_id scheduler_ids[CPU_COUNT];
  rtems_id mtx_ids[MTX_COUNT];
  rtems_id counting_sem_id;
  volatile bool stop_worker[WORKER_COUNT];
  test_stats stats[WORKER_COUNT];
} test_context;

static test_context test_instance;

static uint32_t simple_random(uint32_t v)
{
  v *= 1664525;
  v += 1013904223;

  return v;
}

typedef struct {
  uint32_t guide;
  size_t mtx_stack[MTX_COUNT];
  bool mtx_owned[MTX_COUNT];
  size_t nest_level;
  test_stats stats;
} worker_context;

static void release(test_context *ctx, worker_context *wc, size_t nest_level)
{
  rtems_status_code sc;
  size_t i;

  --wc->nest_level;
  ++wc->stats.release_counter;

  i = wc->mtx_stack[wc->nest_level];
  wc->mtx_owned[i] = false;

  sc = rtems_semaphore_release(ctx->mtx_ids[i]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void release_all(test_context *ctx, worker_context *wc)
{
  while (wc->nest_level > 0) {
    release(ctx, wc, wc->nest_level);
  }
}

static void worker(rtems_task_argument index)
{
  test_context *ctx;
  worker_context wc;
  rtems_status_code sc;

  ctx = &test_instance;
  memset(&wc, 0, sizeof(wc));
  wc.guide = index;

  while (!ctx->stop_worker[index]) {
    uint32_t action;
    uint32_t i;

    if (wc.nest_level < ctx->mtx_count) {
      action = (wc.guide >> 23) % 2;
    } else {
      action = UINT32_MAX;
    }

    i = (wc.guide >> 13) % ctx->mtx_count;

    switch (action) {
      case 0:
        if ( !wc.mtx_owned[i] ) {
          sc = rtems_semaphore_obtain(ctx->mtx_ids[i], RTEMS_WAIT, 1);

          if (sc == RTEMS_SUCCESSFUL) {
            wc.mtx_owned[i] = true;
            wc.mtx_stack[wc.nest_level] = i;
            ++wc.nest_level;
            ++wc.stats.obtain_counter;

            if (wc.nest_level > wc.stats.max_nest_level) {
              wc.stats.max_nest_level = wc.nest_level;
            }
          } else if (sc == RTEMS_INCORRECT_STATE) {
            ++wc.stats.deadlock_counter;
            release_all(ctx, &wc);
          } else if (sc == RTEMS_TIMEOUT) {
            ++wc.stats.timeout_counter;
            release_all(ctx, &wc);
          } else {
            rtems_test_assert(0);
          }
        }

        break;
      default:
        if (wc.nest_level > 0) {
          release(ctx, &wc, wc.nest_level);
        }

        break;
    }

    wc.guide = simple_random(wc.guide);
  }

  release_all(ctx, &wc);

  ctx->stats[index] = wc.stats;

  sc = rtems_semaphore_release(ctx->counting_sem_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(0);
}

static void set_up(test_context *ctx)
{
  rtems_status_code sc;
  uint32_t i;

  ctx->cpu_count = rtems_scheduler_get_processor_maximum();
  ctx->mtx_count = MTX_PER_CPU * ctx->cpu_count;

  sc = rtems_semaphore_create(
    rtems_build_name('S', 'Y', 'N', 'C'),
    0,
    RTEMS_COUNTING_SEMAPHORE,
    0,
    &ctx->counting_sem_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (i = 0; i < ctx->mtx_count; ++i) {
    sc = rtems_semaphore_create(
      rtems_build_name('M', 'U', 'T', 'X'),
      1,
      RTEMS_BINARY_SEMAPHORE
        | RTEMS_PRIORITY
        | RTEMS_INHERIT_PRIORITY,
      0,
      &ctx->mtx_ids[i]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  for (i = 0; i < ctx->cpu_count; ++i) {
    size_t j;

    sc = rtems_scheduler_ident(i, &ctx->scheduler_ids[i]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    for (j = 0; j < WORKER_PER_CPU; ++j) {
      sc = rtems_task_create(
        rtems_build_name('W', 'O', 'R', 'K'),
        255,
        RTEMS_MINIMUM_STACK_SIZE,
        RTEMS_DEFAULT_MODES,
        RTEMS_DEFAULT_ATTRIBUTES,
        &ctx->worker_ids[i][j]
      );
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      sc = rtems_task_set_scheduler(
        ctx->worker_ids[i][j],
        ctx->scheduler_ids[i],
        2 + j
      );
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      sc = rtems_task_start(
        ctx->worker_ids[i][j],
        worker,
        i * WORKER_PER_CPU + j
      );
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    }
  }
}

static void run(test_context *ctx)
{
  rtems_status_code sc;
  uint32_t i;

  sc = rtems_task_wake_after(10 * rtems_clock_get_ticks_per_second());
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (i = 0; i < WORKER_PER_CPU * ctx->cpu_count; ++i) {
    ctx->stop_worker[i] = true;
  }

  for (i = 0; i < WORKER_PER_CPU * ctx->cpu_count; ++i) {
    sc = rtems_semaphore_obtain(
      ctx->counting_sem_id,
      RTEMS_WAIT,
      RTEMS_NO_TIMEOUT
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void tear_down(test_context *ctx)
{
  rtems_status_code sc;
  uint32_t i;

  for (i = 0; i < ctx->cpu_count; ++i) {
    size_t j;

    for (j = 0; j < WORKER_PER_CPU; ++j) {
      sc = rtems_task_delete(ctx->worker_ids[i][j]);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    }
  }

  for (i = 0; i < ctx->mtx_count; ++i) {
    sc = rtems_semaphore_delete(ctx->mtx_ids[i]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  sc = rtems_semaphore_delete(ctx->counting_sem_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (i = 0; i < WORKER_PER_CPU * ctx->cpu_count; ++i) {
    const test_stats *stats;

    stats = &ctx->stats[i];

    printf("worker[%" PRIu32 "][%" PRIu32 "]\n"
      "\tobtain counter   = %" PRIu32 "\n"
      "\tdeadlock counter = %" PRIu32 "\n"
      "\ttimeout counter  = %" PRIu32 "\n"
      "\trelease counter  = %" PRIu32 "\n"
      "\tmax nest level   = %" PRIu32 "\n",
      i / WORKER_PER_CPU,
      i % WORKER_PER_CPU,
      stats->obtain_counter,
      stats->deadlock_counter,
      stats->timeout_counter,
      stats->release_counter,
      stats->max_nest_level
    );
  }
}

static void Init(rtems_task_argument arg)
{
  test_context *ctx;
  rtems_resource_snapshot snapshot;

  TEST_BEGIN();
  rtems_resource_snapshot_take(&snapshot);
  ctx = &test_instance;

  set_up(ctx);
  run(ctx);
  tear_down(ctx);

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));
  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS (1 + WORKER_COUNT)
#define CONFIGURE_MAXIMUM_SEMAPHORES (1 + MTX_COUNT)

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_SCHEDULER_SIMPLE_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_SIMPLE_SMP(0);
RTEMS_SCHEDULER_SIMPLE_SMP(1);
RTEMS_SCHEDULER_SIMPLE_SMP(2);
RTEMS_SCHEDULER_SIMPLE_SMP(3);
RTEMS_SCHEDULER_SIMPLE_SMP(4);
RTEMS_SCHEDULER_SIMPLE_SMP(5);
RTEMS_SCHEDULER_SIMPLE_SMP(6);
RTEMS_SCHEDULER_SIMPLE_SMP(7);
RTEMS_SCHEDULER_SIMPLE_SMP(8);
RTEMS_SCHEDULER_SIMPLE_SMP(9);
RTEMS_SCHEDULER_SIMPLE_SMP(10);
RTEMS_SCHEDULER_SIMPLE_SMP(11);
RTEMS_SCHEDULER_SIMPLE_SMP(12);
RTEMS_SCHEDULER_SIMPLE_SMP(13);
RTEMS_SCHEDULER_SIMPLE_SMP(14);
RTEMS_SCHEDULER_SIMPLE_SMP(15);
RTEMS_SCHEDULER_SIMPLE_SMP(16);
RTEMS_SCHEDULER_SIMPLE_SMP(17);
RTEMS_SCHEDULER_SIMPLE_SMP(18);
RTEMS_SCHEDULER_SIMPLE_SMP(19);
RTEMS_SCHEDULER_SIMPLE_SMP(20);
RTEMS_SCHEDULER_SIMPLE_SMP(21);
RTEMS_SCHEDULER_SIMPLE_SMP(22);
RTEMS_SCHEDULER_SIMPLE_SMP(23);
RTEMS_SCHEDULER_SIMPLE_SMP(24);
RTEMS_SCHEDULER_SIMPLE_SMP(25);
RTEMS_SCHEDULER_SIMPLE_SMP(26);
RTEMS_SCHEDULER_SIMPLE_SMP(27);
RTEMS_SCHEDULER_SIMPLE_SMP(28);
RTEMS_SCHEDULER_SIMPLE_SMP(29);
RTEMS_SCHEDULER_SIMPLE_SMP(30);
RTEMS_SCHEDULER_SIMPLE_SMP(31);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(0, 0), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(1, 1), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(2, 2), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(3, 3), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(4, 4), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(5, 5), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(6, 6), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(7, 7), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(8, 8), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(9, 9), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(10, 10), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(11, 11), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(12, 12), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(13, 13), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(14, 14), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(15, 15), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(16, 16), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(17, 17), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(18, 18), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(19, 19), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(20, 20), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(21, 21), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(22, 22), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(23, 23), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(24, 24), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(25, 25), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(26, 26), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(27, 27), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(28, 28), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(29, 29), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(30, 30), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(31, 31)

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(2, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(3, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(4, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(5, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(6, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(7, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(8, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(9, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(10, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(11, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(12, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(13, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(14, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(15, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(16, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(17, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(18, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(19, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(20, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(21, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(22, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(23, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(24, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(25, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(26, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(27, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(28, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(29, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(30, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(31, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
