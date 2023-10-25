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
#include <stdint.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/bspIo.h>
#include "../shared/isvv_rtems_aux.h"
#include "../shared/utils.h"
#include <string.h>

/**
 *
 * @brief Tests impact of undefined behaviours
 *
 * This test case performs the calculation of the Mandelbrot set in
 * parallel with execution of calls to barriers configured with
 * attributes with undefined behaviour.
 */

#define ITOA_STR_SIZE                           (8 * sizeof(int) + 1)

#define MAX_TLS_SIZE RTEMS_ALIGN_UP(64, RTEMS_TASK_STORAGE_ALIGNMENT)

#define TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define TASK_STORAGE_SIZE                                          \
  RTEMS_TASK_STORAGE_SIZE(MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE, \
                          TASK_ATTRIBUTES)

// test specific global vars
#define TEST_BARRIERS_COUNT 3

#define TASK_COUNT TEST_PROCESSORS
#define TOTAL_TILES 64

#define EVENT_BARRIER RTEMS_EVENT_5

// Timing related variables to ensure concurrent executions
#define FUNCTION_WAIT_MICROSECONDS 10
#define FUNCTION_WAIT_NANOSECONDS (FUNCTION_WAIT_MICROSECONDS * 1000)

rtems_event_set event_send[] = {RTEMS_EVENT_1, RTEMS_EVENT_2, RTEMS_EVENT_3,
                                RTEMS_EVENT_4};

typedef struct
{
  rtems_id main_task;
  uint8_t ntiles;
  uint8_t next_tile;
  rtems_id task_id[TASK_COUNT];
  rtems_id tile_queue;
  rtems_id problem_barrier_id;
  rtems_id test_barriers[TEST_BARRIERS_COUNT];
  uint8_t barrier_task_x;
  uint8_t barrier_results[TEST_BARRIERS_COUNT];
} test_context;

RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static char calc_task_storage[TASK_COUNT][TASK_STORAGE_SIZE];

RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static uint32_t task_before_mandelbrot[TASK_COUNT-1];

RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static uint32_t task_after_mandelbrot[TASK_COUNT-1];

static void calc_task_function(rtems_task_argument arg)
{
  test_context *ctx;

  ctx = (test_context *)arg;
  struct timespec uptime;

  rtems_id local_id = TaskSelfId();
  uint8_t task_idx = 0;

  for (int i = 0; i < TASK_COUNT - 1; i++)
  {
    if (ctx->task_id[i] == local_id)
    {
      task_idx = i;
      break;
    }
  }

  WaitAtBarrier(ctx->problem_barrier_id);

  rtems_clock_get_uptime(&uptime);
  task_before_mandelbrot[task_idx] = (uint32_t) (uptime.tv_sec * 1000000 + uptime.tv_nsec/1000);

  mandelbrot_tile(task_idx+1, ctx->ntiles);

  rtems_clock_get_uptime(&uptime);
  task_after_mandelbrot[task_idx] = (uint32_t) (uptime.tv_sec * 1000000 + uptime.tv_nsec/1000);
  
  SendEvents(ctx->main_task, event_send[task_idx]);
  
  SuspendSelf();
}

static void barrier_test_task(rtems_task_argument arg)
{
    test_context *ctx;
    rtems_status_code sc;

    ctx = (test_context *)arg;
    /* char str[ITOA_STR_SIZE]; */
    /* print_string(itoa(ctx->barrier_task_x, &str, 10)); */
    sc = rtems_barrier_wait(ctx->test_barriers[ctx->barrier_task_x], BARRIER_TIMEOUT);
    if(sc == RTEMS_SUCCESSFUL)
    {
      ctx->barrier_results[ctx->barrier_task_x] = 1;
    }
    else
    {
      ctx->barrier_results[ctx->barrier_task_x] = 0;
    }

    SendEvents(ctx->main_task, EVENT_BARRIER);
    SuspendSelf();
}

static void Init(rtems_task_argument arg)
{
  (void)arg;
  test_context ctx;
  uint32_t start_time, end_time, elapsed_time;
  char ch;
  rtems_event_set received = 0;
  rtems_event_set total_events = 0;
  char str[ITOA_STR_SIZE];
  rtems_status_code sc;
  struct timespec uptime;
  uint32_t functions_start = 0, functions_end = 0;
  struct timespec function_wait;

  function_wait.tv_sec = 0;
  function_wait.tv_nsec = FUNCTION_WAIT_NANOSECONDS;

  memset(ctx.barrier_results, 0, sizeof(ctx.barrier_results)/sizeof(ctx.barrier_results[0]));
  ctx.main_task = rtems_task_self();
  ctx.ntiles = TASK_COUNT - 1;
  ctx.next_tile = 1;
  uint8_t nwaiting = ctx.ntiles;
  // create a normal automatic barrier
  ctx.problem_barrier_id = CreateAutomaticBarrier(nwaiting);

  start_time = rtems_clock_get_ticks_since_boot();
  rtems_task_config calc_task_config = {
      .initial_priority = PRIO_NORMAL,
      .storage_size = TASK_STORAGE_SIZE,
      .maximum_thread_local_storage_size =
          MAX_TLS_SIZE,
      .initial_modes = RTEMS_DEFAULT_MODES,
      .attributes = TASK_ATTRIBUTES};

  // create the mandlebrot set using all available cores minus one
  for (uint32_t i = 0; i < TASK_COUNT - 1; i++)
  {
    ch = '0' + i;
    calc_task_config.name = rtems_build_name('R', 'U', 'N', ch);
    calc_task_config.storage_area = &calc_task_storage[i][0];

    ctx.task_id[i] = DoCreateTask(calc_task_config);
    StartTask(ctx.task_id[i], calc_task_function, &ctx);
    total_events += event_send[i];
  }

  // Wait for 10us to ensure concurrent executions
  clock_nanosleep(CLOCK_MONOTONIC, 0, &function_wait, NULL);
  
  rtems_name barrier_name;
  rtems_attribute barrier_attributes;
  // with the last available core test the undefined behaviour barrier  
  for (uint32_t i = 0; i < TEST_BARRIERS_COUNT; i++) {

    ctx.barrier_task_x = i;
    ch = '0' + i;

    // create a normal automatic barrier, a barrier with undefined behaviour
    // and again a normal automatic barrier
    if (i==1) {
      barrier_attributes = RTEMS_BARRIER_MANUAL_RELEASE | RTEMS_BARRIER_AUTOMATIC_RELEASE;
    }
    else {
      barrier_attributes = RTEMS_BARRIER_AUTOMATIC_RELEASE;
    }
    
    barrier_name = rtems_build_name('B', 'A', 'R', ch);

    sc = rtems_barrier_create(barrier_name, barrier_attributes, 1,
                              &ctx.test_barriers[i]);
    if(sc == RTEMS_SUCCESSFUL)
      ctx.barrier_results[ctx.barrier_task_x] = 1;
    
    calc_task_config.name = rtems_build_name('T', 'A', 'B', ch);
    calc_task_config.storage_area = &calc_task_storage[TASK_COUNT - 1][0];

    ctx.task_id[TASK_COUNT - 1] = DoCreateTask(calc_task_config);
    // execute task that will be using the created barriers
    if (i == 0) {
      rtems_clock_get_uptime(&uptime);
      functions_start = (uint32_t)(uptime.tv_sec * 1000000 + uptime.tv_nsec / 1000);
    }

    StartTask(ctx.task_id[TASK_COUNT - 1], barrier_test_task, &ctx);
    ReceiveAllEvents(EVENT_BARRIER);
    DeleteTask(ctx.task_id[TASK_COUNT - 1]);
    DeleteBarrier(ctx.test_barriers[i]);
  }
  rtems_clock_get_uptime(&uptime);
  functions_end = (uint32_t) (uptime.tv_sec* 1000000 + uptime.tv_nsec/1000) ;

  while (received != total_events)
  {
    received |= ReceiveAllEvents(total_events);
  }
  
  end_time = rtems_clock_get_ticks_since_boot();

  elapsed_time = end_time - start_time;

  uint32_t latest_task_start = 0;
  uint32_t earliest_task_finish = -1;
  for (uint32_t task = 0; task < TASK_COUNT - 1; task++) {
    if (task_before_mandelbrot[task] > latest_task_start)
      latest_task_start = task_before_mandelbrot[task];
    if (task_after_mandelbrot[task] < earliest_task_finish)
      earliest_task_finish = task_after_mandelbrot[task];
  }

  print_string("\n\n");
  if (functions_start > latest_task_start && functions_end < earliest_task_finish) {
    print_string("Functions and normal Mandelbrot executed concurrently: true\n\n");
  } else {
    print_string("Functions and normal Mandelbrot executed concurrently: false\n\n");
  }

  print_string("Multicore Elapsed Time normal barrier - ");
  print_string(itoa(elapsed_time, &str[0], 10));
  print_string("\n");

  if(ctx.barrier_results[0] == 1 && ctx.barrier_results[1] == 1 && ctx.barrier_results[2] == 1)
  {
    print_string("Normal Mandelbrot task barrier functions executed correctly: true\n");
  }
  else
  {
    print_string("Normal Mandelbrot task barrier functions executed correctly: false\n");
  }

  print_test_results();

  for (uint32_t i = 0; i < TASK_COUNT - 1; i++)
  {
    DeleteTask(ctx.task_id[i]);
  }
  
  DeleteBarrier(ctx.problem_barrier_id);
  // Reset results for second run
  memset(ctx.barrier_results, 0, sizeof(ctx.barrier_results)/sizeof(ctx.barrier_results[0]));
  latest_task_start = 0;
  earliest_task_finish = -1;
  total_events = 0;
  received = 0;

  // repeat the previous process, but the main task uses a barrier with undefined behaviour
  sc = rtems_barrier_create(rtems_build_name('B', 'A', 'R', 'P'),
                            RTEMS_BARRIER_MANUAL_RELEASE | RTEMS_BARRIER_AUTOMATIC_RELEASE,
                            nwaiting, &ctx.problem_barrier_id);
  ASSERT_SUCCESS(sc);

  start_time = rtems_clock_get_ticks_since_boot();
  for (uint32_t i = 0; i < TASK_COUNT - 1; i++)
  {
    ch = '0' + i;
    calc_task_config.name = rtems_build_name('R', 'U', 'N', ch);
    calc_task_config.storage_area = &calc_task_storage[i][0];

    ctx.task_id[i] = DoCreateTask(calc_task_config);
    StartTask(ctx.task_id[i], calc_task_function, &ctx);
    total_events += event_send[i];
  }

  // Wait for 10us to ensure concurrent executions
  clock_nanosleep(CLOCK_MONOTONIC, 0, &function_wait, NULL);

  for (uint32_t i = 0; i < TEST_BARRIERS_COUNT; i++) {

    ctx.barrier_task_x = i;
    ch = '0' + i;

    // create a normal automatic barrier, a barrier with undefined behaviour
    // and again a normal automatic barrier
    if (i==1) {
	    barrier_attributes = RTEMS_BARRIER_MANUAL_RELEASE | RTEMS_BARRIER_AUTOMATIC_RELEASE;
    } else {
    	barrier_attributes = RTEMS_BARRIER_AUTOMATIC_RELEASE;
    }
    
    barrier_name = rtems_build_name('B', 'A', 'R', ch);

    sc = rtems_barrier_create(barrier_name, barrier_attributes, 1,
                              &ctx.test_barriers[i]);
    if(sc == RTEMS_SUCCESSFUL)
      ctx.barrier_results[ctx.barrier_task_x] = 1;
    
    calc_task_config.name = rtems_build_name('T', 'A', 'B', ch);
    calc_task_config.storage_area = &calc_task_storage[TASK_COUNT - 1][0];

    ctx.task_id[TASK_COUNT - 1] = DoCreateTask(calc_task_config);
    // execute task that will be using the created barriers
    if (i == 0) {
      rtems_clock_get_uptime(&uptime);
      functions_start = (uint32_t)(uptime.tv_sec * 1000000 + uptime.tv_nsec / 1000);
    }

    StartTask(ctx.task_id[TASK_COUNT - 1], barrier_test_task, &ctx);
    ReceiveAllEvents(EVENT_BARRIER);
    DeleteTask(ctx.task_id[TASK_COUNT - 1]);
    DeleteBarrier(ctx.test_barriers[i]);
  }

  rtems_clock_get_uptime(&uptime);
  functions_end = (uint32_t) (uptime.tv_sec* 1000000 + uptime.tv_nsec/1000) ;

  while (received != total_events)
  {
    received |= ReceiveAllEvents(total_events);
  }
  
  end_time = rtems_clock_get_ticks_since_boot();

  elapsed_time = end_time - start_time;

  for (uint32_t task = 0; task < TASK_COUNT - 1; task++) {
    if (task_before_mandelbrot[task] > latest_task_start)
      latest_task_start = task_before_mandelbrot[task];
    if (task_after_mandelbrot[task] < earliest_task_finish)
      earliest_task_finish = task_after_mandelbrot[task];
  }

  print_string("\n\n");
  if (functions_start > latest_task_start && functions_end < earliest_task_finish) {
    print_string("Functions and undefined Mandelbrot executed concurrently: true\n\n");
  } else {
    print_string("Functions and undefined Mandelbrot executed concurrently: false\n\n");
  }

  print_string("\n");
  print_string("Multicore Elapsed Time undefined behaviour barrier - ");
  print_string(itoa(elapsed_time, &str[0], 10));
  print_string("\n");

  if(ctx.barrier_results[0] == 1 && ctx.barrier_results[1] == 1 && ctx.barrier_results[2] == 1)
  {
    print_string("Undefined Mandelbrot task barrier functions executed correctly: true\n");
  }
  else
  {
    print_string("Undefined Mandelbrot task barrier functions executed correctly: false\n");
  }

  print_test_results();
  
  /* ############################################ */
  for (uint32_t i = 0; i < TASK_COUNT - 1; i++)
  {
    DeleteTask(ctx.task_id[i]);
  }
  DeleteBarrier(ctx.problem_barrier_id);

  rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, 0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS TEST_PROCESSORS

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_MAXIMUM_BARRIERS 5

#define CONFIGURE_MAXIMUM_TASKS (TEST_PROCESSORS + 1)

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
