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
#include <rtems/counter.h>

#include "../shared/isvv_rtems_aux.h"
#include "../shared/utils.h"

#include <stdlib.h>

/**
 * 
 * @brief Demonstrate the use of the Timer Service.
 * 
 * This test case uses a timer server to execute the service routines. The timer server is started 
 * at the default priority so that it interrupts the calculation tasks. This means that the service 
 * routine will not execute until the next calculation task completes.
 *  
 * The Mandelbrot calculation of the Mandelbrot set is divided into tiles which are distributed 
 * between the calculation tasks; when a task completes a tile, it sends a message to get the
 * next tile to be calculated.
 * 
 * Two timers are set up to increment counters at different rates. It is checked that the result 
 * of the parallel calculation is the same as when generated on a single-core and that the 
 * multi-core calculation is faster than the single-core calculation. The number of timer service
 * routines that have been executed is checked.
 */

#define MAX_TLS_SIZE RTEMS_ALIGN_UP(64, RTEMS_TASK_STORAGE_ALIGNMENT)

#define TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define TASK_STORAGE_SIZE                                                      \
  RTEMS_TASK_STORAGE_SIZE(MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE,             \
			  TASK_ATTRIBUTES)

#define ITOA_STR_SIZE (8 * sizeof(int) + 1)

// test specific global vars
#define TASK_COUNT ISVV_TEST_PROCESSORS

rtems_event_set event_send[4] = {RTEMS_EVENT_1,
				 RTEMS_EVENT_2,
				 RTEMS_EVENT_3,
				 RTEMS_EVENT_4
				 };

typedef struct {
    rtems_id id;
    uint16_t counter;
} timer;

typedef struct {
  rtems_id mutex_id;
  rtems_id main_task;
  uint8_t ntiles;
  uint8_t next_tile;
  rtems_id task_id[TASK_COUNT];
  timer fast_timer;
  timer slow_timer;
} test_context;

typedef test_context Context;

RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static char calc_task_storage[TASK_COUNT][TASK_STORAGE_SIZE];

static void calc_task_function(rtems_task_argument arg) {
  Context *ctx;

  ctx = (Context *)arg;

  uint8_t tile;
  rtems_id local_id = TaskSelfId();
  uint8_t task_idx = 255;
  char str;

  for (int i = 0; i < TASK_COUNT; i++) {
    if (ctx->task_id[i] == local_id) {
      task_idx = i;
      break;
    }
  }

  ObtainMutex(ctx->mutex_id);
  tile = ctx->next_tile;
  ctx->next_tile++;
  ReleaseMutex(ctx->mutex_id);

  while (tile <= ctx->ntiles) {
    mandelbrot_tile(tile, ctx->ntiles);

    ObtainMutex(ctx->mutex_id);
    tile = ctx->next_tile;
    ctx->next_tile++;
    ReleaseMutex(ctx->mutex_id);
  }

  SendEvents(ctx->main_task, event_send[task_idx]);
  SuspendSelf();
}


void service_timer( rtems_id timer_id,  void *user_data )
{
    timer *tmr = user_data;

    tmr->counter++;
    ResetTimer(timer_id);
}

static void Init( rtems_task_argument arg ) {
  (void)arg;
  rtems_status_code sc;
  test_context ctx;
  uint32_t start_time, end_time, elapsed_time;
  char ch;
  char str[ITOA_STR_SIZE];
  uint32_t current_cpu;
  bool allDone;
  rtems_task_argument tile;
  rtems_event_set received = 0;
  int total_events = 0;

  ctx.main_task = rtems_task_self();
  ctx.mutex_id = CreateMutex(rtems_build_name('T', 'S', 'M', '1'));
  ctx.ntiles = 64;
  ctx.next_tile = 1;

  sc = rtems_timer_initiate_server(RTEMS_TIMER_SERVER_DEFAULT_PRIORITY,
                                   RTEMS_MINIMUM_STACK_SIZE,
                                   RTEMS_DEFAULT_ATTRIBUTES);

  start_time = rtems_clock_get_ticks_since_boot();
  rtems_task_config calc_task_config = {
      .initial_priority = PRIO_NORMAL,
      .storage_size = TASK_STORAGE_SIZE,
      .maximum_thread_local_storage_size =
      MAX_TLS_SIZE,
      .initial_modes = RTEMS_DEFAULT_MODES,
      .attributes = TASK_ATTRIBUTES
  };

  ObtainMutex(ctx.mutex_id);

  for (uint32_t i = 0; i < TASK_COUNT; i++) {
    ch = '0' + i;

    calc_task_config.name = rtems_build_name('R', 'U', 'N', ch);
    calc_task_config.storage_area = &calc_task_storage[i][0];

    ctx.task_id[i] = DoCreateTask(calc_task_config);
    StartTask(ctx.task_id[i], calc_task_function, &ctx);
    total_events += event_send[i];
  }



  ctx.fast_timer.id = CreateTimer(rtems_build_name( 'F', 'T', 'M', 'R' ));
  ctx.fast_timer.counter = 0;
  ctx.slow_timer.id = CreateTimer(rtems_build_name( 'S', 'T', 'M', 'R' ));
  ctx.slow_timer.counter = 0;

  LaunchFunctionAfter(ctx.fast_timer.id, 11, service_timer, &ctx.fast_timer);
  LaunchFunctionAfter(ctx.slow_timer.id, 23, service_timer, &ctx.slow_timer);

  ReleaseMutex(ctx.mutex_id);

  while (ReceiveAvailableEvents() != total_events) {
  }

  print_string("printing results");
  rtems_putc('\n');
  end_time = rtems_clock_get_ticks_since_boot();

  print_string(itoa(ctx.fast_timer.counter, &str[0], 10));
  rtems_putc(' ');
  print_string(itoa(ctx.slow_timer.counter, &str[0], 10));
  rtems_putc('\n');

  
  elapsed_time = end_time - start_time;

  print_test_results();
  for (uint32_t i = 0; i < TASK_COUNT; i++) {
    DeleteTask(ctx.task_id[i]);
  }
  DeleteMutex(ctx.mutex_id);
  DeleteTimer(ctx.fast_timer.id);
  DeleteTimer(ctx.slow_timer.id);

  rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, 0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS ISVV_TEST_PROCESSORS

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_MAXIMUM_PERIODS 1

#define CONFIGURE_MAXIMUM_BARRIERS 1

#define CONFIGURE_MAXIMUM_TIMERS 2

#define CONFIGURE_MAXIMUM_TASKS (ISVV_TEST_PROCESSORS + 1)

#define CONFIGURE_SCHEDULER_EDF_SMP

#define CONFIGURE_MINIMUM_TASK_STACK_SIZE                                      \
  RTEMS_MINIMUM_STACK_SIZE + CPU_STACK_ALIGNMENT

#define CONFIGURE_EXTRA_TASK_STACKS RTEMS_MINIMUM_STACK_SIZE

#define CONFIGURE_IDLE_TASK_STORAGE_SIZE TASK_STORAGE_SIZE
#define CONFIGURE_INIT_TASK_CONSTRUCT_STORAGE_SIZE 2 * TASK_STORAGE_SIZE

#define CONFIGURE_MINIMUM_TASKS_WITH_USER_PROVIDED_STORAGE                     \
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
