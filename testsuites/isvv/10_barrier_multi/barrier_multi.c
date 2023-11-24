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

#include <stddef.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/bspIo.h>
#include "../shared/isvv_rtems_aux.h"
#include "../shared/utils.h"

/**
 *
 * @brief Demonstrate the use of barriers in SMP context.
 *
 * The Mandelbrot set tiles are distributed between worker tasks to perform the calculation. 
 * Before starting each set, worker tasks are freed by a manual barrier owned by a controlling 
 * thread that releases the barrier after distributing the set, one tile per core. 
 * 
 * Then a set of worker tasks perform the tile calculations in parallel. Only when all of them finish
 * the controlling task will distribute a new set. This is controlled by an automatic barrier that
 * releases when all of the worker tasks complete calculations.
 */

#define MAX_TLS_SIZE RTEMS_ALIGN_UP(64, RTEMS_TASK_STORAGE_ALIGNMENT)

#define TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define TASK_STORAGE_SIZE                                          \
  RTEMS_TASK_STORAGE_SIZE(MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE, \
                          TASK_ATTRIBUTES)

#define ITOA_STR_SIZE (8 * sizeof(int) + 1)

// test specific global vars
#define TASK_COUNT ISVV_TEST_PROCESSORS
#define TOTAL_TILES 64

rtems_event_set event_send[] = {RTEMS_EVENT_1,
                                 RTEMS_EVENT_2,
                                 RTEMS_EVENT_3,
                                 RTEMS_EVENT_4};

#define MAX_MESSAGE_QUEUES 5
#define MAX_MESSAGE_SIZE sizeof(uint8_t)
#define MAX_PENDING_MESSAGES TASK_COUNT

uint8_t count_process[TOTAL_TILES] = {0};

typedef struct
{
  rtems_id main_task;
  uint8_t ntiles;
  uint8_t next_tile;
  rtems_id task_id[TASK_COUNT];
  rtems_id tile_queue;
  rtems_id message_queue[TASK_COUNT];
  rtems_id auto_barrier_id;
  rtems_id manual_barrier_id;
} test_context;

RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static char calc_task_storage[TASK_COUNT][TASK_STORAGE_SIZE];

RTEMS_MESSAGE_QUEUE_BUFFER(MAX_MESSAGE_SIZE)
msg_tile_queue_storage[MAX_PENDING_MESSAGES];

RTEMS_MESSAGE_QUEUE_BUFFER(MAX_MESSAGE_SIZE)
msg_task_queue_storage[TASK_COUNT][MAX_PENDING_MESSAGES];

static void calc_task_function(rtems_task_argument arg)
{
  test_context *ctx;

  ctx = (test_context *)arg;

  uint8_t tile = 0;
  rtems_id local_id = TaskSelfId();
  uint8_t task_idx = 0;

  for (int i = 0; i < TASK_COUNT; i++)
  {
    if (ctx->task_id[i] == local_id)
    {
      task_idx = i;
      break;
    }
  }

  char ch = '0' + task_idx;

  rtems_message_queue_config msg_config = {
      .name = rtems_build_name('M', 'S', 'G', ch),
      .maximum_pending_messages = MAX_PENDING_MESSAGES,
      .maximum_message_size = MAX_MESSAGE_SIZE,
      .storage_size = sizeof(msg_task_queue_storage[task_idx]),
      .storage_area = &msg_task_queue_storage[task_idx],
      .attributes = RTEMS_FIFO | RTEMS_GLOBAL};
  ctx->message_queue[task_idx] = CreateMessageQueue(msg_config);

  SendMessage(ctx->tile_queue, &task_idx, sizeof(task_idx));
  ReceiveMessage(ctx->message_queue[task_idx], &tile);
  while (tile <= ctx->ntiles)
  {
    WaitAtBarrier(ctx->manual_barrier_id);

    count_process[tile - 1]++;
    mandelbrot_tile(tile, ctx->ntiles);

    WaitAtBarrier(ctx->auto_barrier_id);

    SendMessage(ctx->tile_queue, &task_idx, sizeof(task_idx));
    ReceiveMessage(ctx->message_queue[task_idx], &tile);
  }

  SendEvents(ctx->main_task, event_send[task_idx]);
  SuspendSelf();
}

static void Init(rtems_task_argument arg)
{
  (void)arg;
  test_context ctx;
  uint32_t start_time, end_time, elapsed_time;
  char ch;
  uint8_t task = 255;
  rtems_event_set received = 0;
  rtems_event_set total_events = 0;
  bool correctly_processed = true;
  char str[ITOA_STR_SIZE];

  rtems_message_queue_config msg_config = {
      .name = rtems_build_name('M', 'S', 'G', 'T'),
      .maximum_pending_messages = RTEMS_ARRAY_SIZE(msg_tile_queue_storage),
      .maximum_message_size = MAX_MESSAGE_SIZE,
      .storage_size = sizeof(msg_tile_queue_storage),
      .storage_area = &msg_tile_queue_storage,
      .attributes = RTEMS_FIFO | RTEMS_GLOBAL};

  ctx.main_task = rtems_task_self();
  ctx.tile_queue = CreateMessageQueue(msg_config);
  ctx.ntiles = TOTAL_TILES;
  ctx.next_tile = 1;
  uint8_t nwaiting = TASK_COUNT;
  ctx.auto_barrier_id = CreateAutomaticBarrier(nwaiting);
  ctx.manual_barrier_id = CreateManualBarrier();

  start_time = rtems_clock_get_ticks_since_boot();
  rtems_task_config calc_task_config = {
      .initial_priority = PRIO_NORMAL,
      .storage_size = TASK_STORAGE_SIZE,
      .maximum_thread_local_storage_size =
          MAX_TLS_SIZE,
      .initial_modes = RTEMS_DEFAULT_MODES,
      .attributes = TASK_ATTRIBUTES};

  for (uint32_t i = 0; i < TASK_COUNT; i++)
  {
    ch = '0' + i;
    calc_task_config.name = rtems_build_name('R', 'U', 'N', ch);
    calc_task_config.storage_area = &calc_task_storage[i][0];

    ctx.task_id[i] = DoCreateTask(calc_task_config);
    StartTask(ctx.task_id[i], calc_task_function, &ctx);
    total_events += event_send[i];
  }

  while (ctx.next_tile <= ctx.ntiles + TASK_COUNT)
  {
    ReceiveMessage(ctx.tile_queue, &task);
    SendMessage(ctx.message_queue[task], &ctx.next_tile, sizeof(ctx.next_tile));

    if (ctx.next_tile <= ctx.ntiles)
    {
      if (ctx.next_tile % TASK_COUNT == 0)
      {
        ReleaseManualBarrier(ctx.manual_barrier_id, TASK_COUNT);
      }
      else if (ctx.next_tile == ctx.ntiles)
      {
        ReleaseManualBarrier(ctx.manual_barrier_id, ctx.next_tile % TASK_COUNT);
      }
    }

    ctx.next_tile++;
  }

  while (received != total_events)
  {
    received |= ReceiveAllEvents(total_events);
  }
  
  end_time = rtems_clock_get_ticks_since_boot();

  elapsed_time = end_time - start_time;

  print_string("\n");
  print_string("Multicore Elapsed Time -");
  print_string(itoa(elapsed_time, &str[0], 10));
  print_string("\n");

  print_test_results();

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

  for (uint32_t i = 0; i < TASK_COUNT; i++)
  {
    DeleteTask(ctx.task_id[i]);
    DeleteMessageQueue(ctx.message_queue[i]);
  }
  DeleteMessageQueue(ctx.tile_queue);
  DeleteBarrier(ctx.auto_barrier_id);
  DeleteBarrier(ctx.manual_barrier_id);

  rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, 0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS ISVV_TEST_PROCESSORS

#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES MAX_MESSAGE_QUEUES

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_MAXIMUM_BARRIERS 2

#define CONFIGURE_MAXIMUM_TASKS (ISVV_TEST_PROCESSORS + 1)

#define CONFIGURE_SCHEDULER_EDF_SMP

#define CONFIGURE_MINIMUM_TASK_STACK_SIZE \
  RTEMS_MINIMUM_STACK_SIZE + CPU_STACK_ALIGNMENT

#define CONFIGURE_EXTRA_TASK_STACKS RTEMS_MINIMUM_STACK_SIZE

#define CONFIGURE_IDLE_TASK_STORAGE_SIZE TASK_STORAGE_SIZE
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
