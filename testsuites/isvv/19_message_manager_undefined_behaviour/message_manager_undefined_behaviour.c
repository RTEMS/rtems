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

/**
 *
 * @brief Tests impact of undefined behaviours
 *
 * This test case performs the calculation of the Mandelbrot set in
 * parallel with execution of calls to message queues configured with
 * attributes with undefined behaviour.
 */

#define ITOA_STR_SIZE                           (8 * sizeof(int) + 1)

#define MAX_TLS_SIZE RTEMS_ALIGN_UP(64, RTEMS_TASK_STORAGE_ALIGNMENT)

#define TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define TASK_STORAGE_SIZE                                          \
  RTEMS_TASK_STORAGE_SIZE(MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE, \
                          TASK_ATTRIBUTES)

#define TEST_MESSAGE_QUEUES_COUNT 3

#define TASK_COUNT TEST_PROCESSORS
#define TEST_TASK (TASK_COUNT - 1)
#define MANDL_TASKS (TASK_COUNT - 1)
#define TOTAL_TILES 64

#define MAX_MESSAGE_QUEUES 2
#define MAX_MESSAGE_SIZE sizeof(uint8_t)
#define MAX_PENDING_MESSAGES TASK_COUNT

#define MESSAGE_TEST_SEND 12
#define MESSAGE_TASK_SEND 24

rtems_event_set event_send[] = {RTEMS_EVENT_1, RTEMS_EVENT_2, RTEMS_EVENT_3,
                                RTEMS_EVENT_4};

typedef struct
{
  rtems_id main_task;
  uint8_t ntiles;
  rtems_id task_id[TASK_COUNT];
  rtems_id tile_queue;
  rtems_id barrier_id;
  rtems_id msg_init_queue_id;
  rtems_id msg_task_queue_id;
} test_context;

RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static char calc_task_storage[TASK_COUNT][TASK_STORAGE_SIZE];

RTEMS_MESSAGE_QUEUE_BUFFER(MAX_MESSAGE_SIZE)
msg_tile_queue_storage[MAX_PENDING_MESSAGES];

RTEMS_MESSAGE_QUEUE_BUFFER(MAX_MESSAGE_SIZE)
msg_task_queue_storage[MAX_PENDING_MESSAGES];

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

  rtems_clock_get_uptime(&uptime);
  task_before_mandelbrot[task_idx] = (uint32_t) (uptime.tv_sec * 1000000 + uptime.tv_nsec/1000);

  for (int i = 0; i < MANDL_TASKS; i++)
  {
    if (ctx->task_id[i] == local_id)
    {
      task_idx = i;
      break;
    }
  }

  WaitAtBarrier(ctx->barrier_id);
  mandelbrot_tile(task_idx+1, ctx->ntiles);

  rtems_clock_get_uptime(&uptime);
  task_after_mandelbrot[task_idx] = (uint32_t) (uptime.tv_sec * 1000000 + uptime.tv_nsec/1000);
  
  SendEvents(ctx->main_task, event_send[task_idx]);
  
  SuspendSelf();
}

static void message_test_task(rtems_task_argument arg)
{
    test_context *ctx;

    ctx = (test_context *)arg;

    uint8_t val_receive = 0, val_send = 0;

    ReceiveMessage(ctx->msg_task_queue_id, &val_receive);
    if (val_receive == MESSAGE_TASK_SEND) {
      val_send = MESSAGE_TEST_SEND;
      SendMessage(ctx->msg_init_queue_id, &val_send, sizeof(uint8_t));
    }
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
  bool defined_process = false;
  bool undefined_process = false;
  char str[ITOA_STR_SIZE];
  struct timespec uptime;
  uint32_t functions_start = 0, functions_end;

  ctx.main_task = rtems_task_self();
  ctx.ntiles = MANDL_TASKS;
  uint8_t nwaiting = ctx.ntiles;
  // create an automatic barrier
  ctx.barrier_id = CreateAutomaticBarrier(nwaiting);

  start_time = rtems_clock_get_ticks_since_boot();
  rtems_task_config calc_task_config = {
      .initial_priority = PRIO_NORMAL,
      .storage_size = TASK_STORAGE_SIZE,
      .maximum_thread_local_storage_size =
          MAX_TLS_SIZE,
      .initial_modes = RTEMS_DEFAULT_MODES,
      .attributes = TASK_ATTRIBUTES};

  // create the mandlebrot set using all available cores minus one
  for (uint32_t i = 0; i < MANDL_TASKS; i++)
  {
    ch = '0' + i;
    calc_task_config.name = rtems_build_name('R', 'U', 'N', ch);
    calc_task_config.storage_area = &calc_task_storage[i][0];

    ctx.task_id[i] = DoCreateTask(calc_task_config);
    StartTask(ctx.task_id[i], calc_task_function, &ctx);
    total_events += event_send[i];
  }

  // with the last available core test the undefined behaviour message queue
  rtems_message_queue_config msg_config = {
      .name = rtems_build_name('M', 'S', 'G', 'T'),
      .maximum_pending_messages = RTEMS_ARRAY_SIZE(msg_tile_queue_storage),
      .maximum_message_size = MAX_MESSAGE_SIZE,
      .storage_size = sizeof(msg_tile_queue_storage),
      .storage_area = &msg_tile_queue_storage,
      .attributes = RTEMS_FIFO | RTEMS_GLOBAL};

  // create a normal message queue to send messages to the tasks that will
  // receive messages in the undefined behaviour message queues

  uint8_t val_send = 0, val_receive = 0;
  for (uint32_t i = 0; i < TEST_MESSAGE_QUEUES_COUNT; i++)
  {
    ctx.msg_task_queue_id = CreateMessageQueue(msg_config);
    // create a normal message queue, a message queue with undefined
    // behaviour and again a normal message queue
    if (i==1)
    {
	    msg_config.name = rtems_build_name('M', 'S', 'G', '0');
	    msg_config.attributes = RTEMS_FIFO | RTEMS_LOCAL | RTEMS_GLOBAL;
    }
    else
    {
      msg_config.name = rtems_build_name('M', 'S', 'G', '1');
      msg_config.attributes = RTEMS_FIFO | RTEMS_GLOBAL;
    }

    ctx.msg_init_queue_id = CreateMessageQueue(msg_config);

    ch = '0' + i;
    calc_task_config.name = rtems_build_name('T', 'M', 'S', ch);
    calc_task_config.storage_area = &calc_task_storage[TEST_TASK][0];

    ctx.task_id[TEST_TASK] = DoCreateTask(calc_task_config);
    // execute task that receives and sends messages 
    if (i == 0) {
      rtems_clock_get_uptime(&uptime);
      functions_start = (uint32_t)(uptime.tv_sec * 1000000 + uptime.tv_nsec / 1000);
    }
    StartTask(ctx.task_id[TEST_TASK], message_test_task, &ctx);

    val_send = MESSAGE_TASK_SEND;

    SendMessage(ctx.msg_task_queue_id, &val_send, sizeof(val_send));
    ReceiveMessage(ctx.msg_init_queue_id, &val_receive);

    if (i==1) {
    	undefined_process = (val_receive == MESSAGE_TEST_SEND);
    } else {
	    defined_process = (val_receive == MESSAGE_TEST_SEND);
    }

    DeleteTask(ctx.task_id[TEST_TASK]);
    DeleteMessageQueue(ctx.msg_init_queue_id);
    DeleteMessageQueue(ctx.msg_task_queue_id);
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
  for (uint32_t task = 0; task < MANDL_TASKS; task++) {
      if (task_before_mandelbrot[task] > latest_task_start)
      latest_task_start = task_before_mandelbrot[task];
    if (task_after_mandelbrot[task] < earliest_task_finish)
        earliest_task_finish = task_after_mandelbrot[task];
  }

  print_string("\n");
  if (functions_start > latest_task_start && functions_end < earliest_task_finish) {
    print_string("Functions and mandelbrot executed concurrently: true\n\n");
  } else {
    print_string("Functions and mandelbrot executed concurrently: false\n\n");
  }

  print_string("Mandelbrot task and defined message functions executed correctly: ");
  if (defined_process) {
    print_string("true\n");
  } else {
    print_string("false\n");
  }

  print_string("Mandelbrot task and undefined message functions executed correctly: ");
  if (undefined_process) {
    print_string("true\n");
  } else {
    print_string("false\n");
  }

  print_string("Multicore Elapsed Time - ");
  print_string(itoa(elapsed_time, &str[0], 10));
  print_string("\n");

  print_test_results();

  for (uint32_t i = 0; i < MANDL_TASKS; i++)
  {
    DeleteTask(ctx.task_id[i]);
  }
  DeleteBarrier(ctx.barrier_id);

  rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, 0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS TEST_PROCESSORS

#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES MAX_MESSAGE_QUEUES

#define CONFIGURE_MAXIMUM_BARRIERS 1

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
