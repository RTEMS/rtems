/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2022 Critical Software SA
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

#include <rtems.h>
#include <rtems/bspIo.h>
#include "../shared/isvv_rtems_aux.h"
#include "../shared/utils.h"

/**
- * @brief Assert RTEMS SMP under the stress of multiple core workloads with periodic tasks and 
- * accessing protected data.
- *
- * This test case performs the calculation of the mandelbrot set divided by tiles and distributed
- * with periodic tasks. The result is stored as a reference set and the elapsed time is measured.
*/

#define MAX_TLS_SIZE RTEMS_ALIGN_UP(64, RTEMS_TASK_STORAGE_ALIGNMENT)

#define TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define TASK_STORAGE_SIZE                                          \
  RTEMS_TASK_STORAGE_SIZE(MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE, \
                          TASK_ATTRIBUTES)

#define ITOA_STR_SIZE (8 * sizeof(int) + 1)

/* Given the board, define rate monotonic period length
 to approx. the mean time needed to calculate a single tile */
#ifdef gr740
  const rtems_interval PERIOD_LENGTH = 800;
#endif
#ifdef gr712rc
  const rtems_interval PERIOD_LENGTH = 2500;
#endif

// test specific global vars
#define TASK_COUNT TEST_PROCESSORS
#define TOTAL_TILES 64

rtems_event_set event_send[4] = {RTEMS_EVENT_1,
                                 RTEMS_EVENT_2,
                                 RTEMS_EVENT_3,
                                 RTEMS_EVENT_4};

uint8_t count_process[TOTAL_TILES] = {0};

typedef struct
{
  rtems_id mutex_id;
  rtems_id main_task;
  rtems_id monoperiod;
  uint8_t ntiles;
  uint8_t next_tile;
  rtems_id task_id[TASK_COUNT];
  uint16_t periods_executed;
  uint16_t deadlines_missed;
  rtems_id manual_barrier_id;
} test_context;

typedef test_context Context;

RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static char calc_task_storage[TASK_COUNT][TASK_STORAGE_SIZE];

static void calc_task_function(rtems_task_argument arg)
{
  Context *ctx;
  ctx = (Context *)arg;

  uint8_t tile = 0;
  rtems_id local_id = TaskSelfId();
  uint8_t task_idx = 255;

  for (int i = 0; i < TASK_COUNT; i++)
  {
    if (ctx->task_id[i] == local_id)
    {
      task_idx = i;
      break;
    }
  }

  while (tile < ctx->ntiles)
  {
    WaitAtBarrier(ctx->manual_barrier_id);

    ObtainMutex(ctx->mutex_id);
    tile = ctx->next_tile;
    count_process[tile - 1]++;
    ctx->next_tile++;
    ReleaseMutex(ctx->mutex_id);

    mandelbrot_tile(tile, ctx->ntiles);

    SendEvents(ctx->main_task, event_send[task_idx]);
  }
  SuspendSelf();
}

static void Init(rtems_task_argument arg)
{
  (void)arg;
  test_context ctx;
  uint32_t start_time, end_time, elapsed_time;
  char ch;
  char str[ITOA_STR_SIZE];
  int total_events = 0;
  bool correctly_processed = true;

  rtems_event_set received = 0;

  ctx.main_task = rtems_task_self();
  ctx.mutex_id = CreateMutex(rtems_build_name('R', 'M', 'O', '1'));
  ctx.ntiles = TOTAL_TILES;
  ctx.next_tile = 1;
  ctx.monoperiod = CreateRateMonotonic();
  ctx.periods_executed = 0;
  ctx.deadlines_missed = 0;

  ctx.manual_barrier_id = CreateManualBarrier();

  rtems_id main_sched_id;
  rtems_task_get_scheduler(ctx.main_task, &main_sched_id);

  rtems_task_priority init_task_priority;
  rtems_task_get_priority(ctx.main_task, main_sched_id, &init_task_priority);
 
 
  rtems_task_config calc_task_config = {
      .initial_priority = init_task_priority,
      .storage_size = TASK_STORAGE_SIZE,
      .maximum_thread_local_storage_size =
          MAX_TLS_SIZE,
      .initial_modes = RTEMS_DEFAULT_MODES,
      .attributes = TASK_ATTRIBUTES};

  ObtainMutex(ctx.mutex_id);

  for (uint32_t i = 0; i < TASK_COUNT; i++)
  {
    ch = '0' + i;

    calc_task_config.name = rtems_build_name('R', 'U', 'N', ch);
    calc_task_config.storage_area = &calc_task_storage[i][0];

    ctx.task_id[i] = DoCreateTask(calc_task_config);
    StartTask(ctx.task_id[i], calc_task_function, &ctx);
    total_events += event_send[i];
  }
  start_time = rtems_clock_get_ticks_since_boot();

  ReleaseMutex(ctx.mutex_id);

  WaitPeriod(ctx.monoperiod, PERIOD_LENGTH);
  uint8_t tile = 0;
  while (tile < ctx.ntiles )
  {
    ReleaseManualBarrier(ctx.manual_barrier_id, TASK_COUNT);

    while (received != total_events)
    {
      received |= ReceiveAllEvents(total_events);
    }

    if (DoesPeriodTimeOut(ctx.monoperiod, PERIOD_LENGTH))
    {
      ctx.deadlines_missed++;
    }

    ctx.periods_executed++;
    ObtainMutex(ctx.mutex_id);
    tile = ctx.next_tile-1;
    ReleaseMutex(ctx.mutex_id);
    received = 0;
  }

  for (uint32_t i = 0; i < TASK_COUNT; i++)
  {
    DeleteTask(ctx.task_id[i]);
  }
  DeleteMutex(ctx.mutex_id);
  DeleteRateMonotonic(ctx.monoperiod);
  end_time = rtems_clock_get_ticks_since_boot();

  elapsed_time = end_time - start_time;

  print_string("\n");
  print_string("Multicore Elapsed Time = ");
  print_string(itoa(elapsed_time, &str[0], 10));
  print_string("\n\r");

  for (uint8_t i = 0; i < ctx.ntiles; i++)
  {
    if (count_process[i] != 1)
    {
      correctly_processed = false;
      break;
    }
  }

  if (correctly_processed)
  {
    print_string("Each tile only processed once: true\n");
  }
  else
  {
    print_string("Each tile only processed once: false\n");
  }

  print_test_results();

  print_string("Number of rate monotonic periods executed: ");
  print_string(itoa(ctx.periods_executed, &str[0], 10));
  print_string("\n");
  print_string("Number of rate monotonic deadlines missed: ");
  print_string(itoa(ctx.deadlines_missed, &str[0], 10));
  print_string("\n");

  rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, 0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS TEST_PROCESSORS

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_MAXIMUM_PERIODS 1

#define CONFIGURE_MAXIMUM_BARRIERS 1

#define CONFIGURE_MAXIMUM_TASKS (TEST_PROCESSORS + 1)

#define CONFIGURE_SCHEDULER_EDF_SMP

#define CONFIGURE_MINIMUM_TASK_STACK_SIZE \
  RTEMS_MINIMUM_STACK_SIZE + CPU_STACK_ALIGNMENT

#define CONFIGURE_EXTRA_TASK_STACKS RTEMS_MINIMUM_STACK_SIZE

#define CONFIGURE_INIT_TASK_CONSTRUCT_STORAGE_SIZE 2 * TASK_STORAGE_SIZE

#define CONFIGURE_MINIMUM_TASKS_WITH_USER_PROVIDED_STORAGE \
  CONFIGURE_MAXIMUM_TASKS

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 0

#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE MAX_TLS_SIZE

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES (RTEMS_SYSTEM_TASK | TASK_ATTRIBUTES)

#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
