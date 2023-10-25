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
#include "../shared/utils.h"
#include "../shared/isvv_rtems_aux.h"

/**
 *
 * @brief Tests impact of undefined behaviours for events manager
 *
 * This test case performs the calculation of the Mandelbrot set in parallel with
 * execution of various events-related functions with conditions which lead to
 * undefined behaviour
 */

#define ITOA_STR_SIZE (4 * sizeof(int) + 1)

#define NUM_TEST_FUNCTIONS 15
#define RELATIVE_SLEEP 0
#define FUNCTION_WAIT_MICROSECONDS 100
#define FUNCTION_WAIT_NANOSECONDS (FUNCTION_WAIT_MICROSECONDS * 1000)

#define MAX_TLS_SIZE RTEMS_ALIGN_UP(64, RTEMS_TASK_STORAGE_ALIGNMENT)

#define TASK_STORAGE_SIZE                      \
  RTEMS_TASK_STORAGE_SIZE(                     \
      MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE, \
      TASK_ATTRIBUTES)

#define TASK_ATTRIBUTES RTEMS_FLOATING_POINT

// test specific global vars
#define SEND_EVENTS_PERIOD 2

// One task for sending events, one task for receiving events, and n-1 calculation tasks
#define TASK_COUNT (TEST_PROCESSORS) + 1

// We want to create mandelbrot executions whilst our events manager functions execute
#define CALCULATION_TASKS (TEST_PROCESSORS) - 1

#define EVENT_CALLS 15
const rtems_event_set TEST_EVENTS[EVENT_CALLS] = {RTEMS_EVENT_0,
                                                  RTEMS_EVENT_1,
                                                  RTEMS_EVENT_2,
                                                  RTEMS_EVENT_3,
                                                  RTEMS_EVENT_4,
                                                  RTEMS_EVENT_5,
                                                  RTEMS_EVENT_6,
                                                  RTEMS_EVENT_7,
                                                  RTEMS_EVENT_8,
                                                  RTEMS_EVENT_9,
                                                  RTEMS_EVENT_10,
                                                  RTEMS_EVENT_11,
                                                  RTEMS_EVENT_12,
                                                  RTEMS_EVENT_13,
                                                  RTEMS_EVENT_14};

// Barrier to ensure calculation tasks and undefined behaviour tasks occur concurrently
static rtems_id start_barrier;
static rtems_id TASK_COUNTER_SEMAPHORE;

typedef struct
{
  rtems_id calc_task_id[CALCULATION_TASKS];
  rtems_id undefined_task_id;
  rtems_id send_events_task_id;
  uint32_t functions_start, functions_end;
  bool is_expected_events;
  rtems_event_set received_events_set;
} test_context;

// Create storage areas for each worker, using task construct forces
RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static char task_storage[TASK_COUNT][TASK_STORAGE_SIZE];

// Storage space for timings
RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static uint32_t task_before_mandelbrot[CALCULATION_TASKS];

RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static uint32_t task_after_mandelbrot[CALCULATION_TASKS];

static void calc_task_function(rtems_task_argument tile)
{
  uint8_t tile_n = (uint8_t)tile;
  struct timespec uptime;

  WaitAtBarrier(start_barrier);

  // Check time function begins execution of mandelbrot executions
  rtems_clock_get_uptime(&uptime);
  task_before_mandelbrot[tile_n - 1] = (uint32_t)(uptime.tv_sec * 1000000 + uptime.tv_nsec / 1000);

  mandelbrot_tile(tile_n, CALCULATION_TASKS);

  // Check time function finishes execution of mandelbrot executions
  rtems_clock_get_uptime(&uptime);
  task_after_mandelbrot[tile_n - 1] = (uint32_t)(uptime.tv_sec * 1000000 + uptime.tv_nsec / 1000);

  // Task has finished so increment counter
  ReleaseCounterSemaphore(TASK_COUNTER_SEMAPHORE);
  rtems_task_exit();
}

// Task to send an event to test the rtems_receive_event directive under scrutiny
static void send_events_task(rtems_task_argument arg)
{
  test_context *ctx = (test_context *)arg;

  rtems_id period = CreateRateMonotonic();

  for (uint8_t i = 0; i < EVENT_CALLS; i++)
  {
    WaitPeriod(period, SEND_EVENTS_PERIOD);
    SendEvents(ctx->undefined_task_id, TEST_EVENTS[i]);
  }

  // Once all the events that should be sent are sent, perform cleanup activities
  rtems_rate_monotonic_delete(period);
  rtems_task_exit();
}

// When using a directive with NO_WAIT, we need to do ensure an event has been provided from our
// send event task without consuming the event which we want to test with the directive
inline void WaitUntilPendingEvent(void) {
  while (!QueryPendingEvents()) {}
}

static void undefined_behaviour_task(rtems_task_argument arg)
{
  test_context *ctx = (test_context *)arg;
  struct timespec uptime, function_wait;

  function_wait.tv_sec = 0;
  function_wait.tv_nsec = FUNCTION_WAIT_NANOSECONDS;

  WaitAtBarrier(start_barrier);

  // Wait FUNCTION_WAIT_MICROSECONDS microseconds to ensure function calls are concurrent with mandelbrot calculation
  clock_nanosleep(CLOCK_MONOTONIC, RELATIVE_SLEEP, &function_wait, NULL);

  rtems_clock_get_uptime(&uptime);
  ctx->functions_start = (uint32_t)(uptime.tv_sec * 1000000 + uptime.tv_nsec / 1000);

  /* Start execution of events related functions following formula of:
   *
   * Defined behaviour
   * Undefined behaviour
   * Defined behaviour
   *
   * Where option_set has certain values combined with a bitwise OR. Some options are mutually exclusive. 
   * If mutually exclusive options are combined, the behaviour is undefined
   *
   * What we see when combining these values is the non-default options take precedence and are applied.
   * When not provided to the directive, default options are applied.
   * However we will specify these regardless for greater clarity.
   * 
   * WaitUntilPendingEvent() is used before directives to ensure an event from our periodic send event
   * is ready to be consumed
   */

  // Define test relevant variables
  rtems_status_code sc;
  rtems_event_set received;
  rtems_event_set total_events = 0;

  /*
   * Testing RTEMS_EVENT_ALL | RTEMS_EVENT_ANY with RTEMS_WAIT
   */

  total_events |= ReceiveAllEvents(TEST_EVENTS[0]);

  // INVALID CALL
  WaitUntilPendingEvent();
  sc = rtems_event_receive(TEST_EVENTS[1], RTEMS_WAIT | RTEMS_EVENT_ALL | RTEMS_EVENT_ANY, RTEMS_NO_TIMEOUT, &received);
  ASSERT_SUCCESS_WITHOUT_FAILING(sc);
  total_events |= received;

  total_events |= ReceiveAllEvents(TEST_EVENTS[2]);

  /*
   * Testing RTEMS_EVENT_ALL | RTEMS_EVENT_ANY with RTEMS_NO_WAIT
   */

  total_events |= ReceiveAllEvents(TEST_EVENTS[3]);

  // INVALID CALL
  WaitUntilPendingEvent();
  sc = rtems_event_receive(TEST_EVENTS[4], RTEMS_NO_WAIT | RTEMS_EVENT_ALL | RTEMS_EVENT_ANY, RTEMS_NO_TIMEOUT, &received);
  ASSERT_SUCCESS_WITHOUT_FAILING(sc);
  total_events |= received;

  total_events |= ReceiveAllEvents(TEST_EVENTS[5]);

  /*
   * Testing RTEMS_NO_WAIT | RTEMS_WAIT with RTEMS_EVENT_ALL
   */

  total_events |= ReceiveAllEvents(TEST_EVENTS[6]);

  // INVALID CALL
  WaitUntilPendingEvent();
  sc = rtems_event_receive(TEST_EVENTS[7], RTEMS_NO_WAIT | RTEMS_WAIT | RTEMS_EVENT_ALL, RTEMS_NO_TIMEOUT, &received);
  ASSERT_SUCCESS_WITHOUT_FAILING(sc);
  total_events |= received;

  total_events |= ReceiveAllEvents(TEST_EVENTS[8]);

  /*
   * Testing RTEMS_NO_WAIT | RTEMS_WAIT with RTEMS_EVENT_ANY
   */

  total_events |= ReceiveAllEvents(TEST_EVENTS[9]);

  // INVALID CALL
  WaitUntilPendingEvent();
  sc = rtems_event_receive(TEST_EVENTS[10], RTEMS_NO_WAIT | RTEMS_WAIT | RTEMS_EVENT_ANY, RTEMS_NO_TIMEOUT, &received);
  ASSERT_SUCCESS_WITHOUT_FAILING(sc);
  total_events |= received;

  total_events |= ReceiveAllEvents(TEST_EVENTS[11]);

  /*
   * Testing RTEMS_NO_WAIT | RTEMS_WAIT | RTEMS_EVENT_ALL | RTEMS_EVENT_ANY
   */

  total_events |= ReceiveAllEvents(TEST_EVENTS[12]);

  // INVALID CALL
  WaitUntilPendingEvent();
  sc = rtems_event_receive(TEST_EVENTS[13], RTEMS_NO_WAIT | RTEMS_WAIT | RTEMS_EVENT_ALL | RTEMS_EVENT_ANY, RTEMS_NO_TIMEOUT, &received);
  ASSERT_SUCCESS_WITHOUT_FAILING(sc);
  total_events |= received;

  total_events |= ReceiveAllEvents(TEST_EVENTS[14]);

  // DEBUG
  // print_string("FINISHED EXECUTION OF EVENTS DIRECTIVES\n");

  // Check total received events against expected events and pass back to main task
  ctx->is_expected_events = (total_events == (power(2, EVENT_CALLS) - 1));
  ctx->received_events_set = total_events;

  rtems_clock_get_uptime(&uptime);
  ctx->functions_end = (uint32_t)(uptime.tv_sec * 1000000 + uptime.tv_nsec / 1000);

  // Only once this task has finished do we know we can progress
  // Task has finished so increment counter
  ReleaseCounterSemaphore(TASK_COUNTER_SEMAPHORE);
  rtems_task_exit();
}

static void Init(rtems_task_argument arg)
{
  (void)arg;
  test_context ctx;
  char str[ITOA_STR_SIZE];
  uint8_t wait = 0;

  TASK_COUNTER_SEMAPHORE = CreateCounterSemaphore(rtems_build_name('T', 'C', 'S', '0'), 0);

  start_barrier = CreateAutomaticBarrier(CALCULATION_TASKS + 1);

  // Config for our mandelbrot tile function, with priority high as we don't want this to be preempted
  rtems_task_config calc_task_config = {
      .initial_priority = PRIO_HIGH,
      .storage_size = TASK_STORAGE_SIZE,
      .maximum_thread_local_storage_size = MAX_TLS_SIZE,
      .initial_modes = RTEMS_DEFAULT_MODES,
      .attributes = TASK_ATTRIBUTES};

  // Finish config for our mandelbrot tile functions and start them
  for (uint32_t i = 0; i < CALCULATION_TASKS; i++)
  {
    calc_task_config.name = rtems_build_name('R', 'U', 'N', (char)i);
    calc_task_config.storage_area = &task_storage[i][0];

    ctx.calc_task_id[i] = DoCreateTask(calc_task_config);
    StartTask(ctx.calc_task_id[i], calc_task_function, (void *)(i + 1));
  }

  // Config our undefined behaviour task and start it
  rtems_task_config undef_task_config = {
      .name = rtems_build_name('U', 'N', 'D', 'F'),
      .initial_priority = PRIO_NORMAL,
      .storage_size = TASK_STORAGE_SIZE,
      .maximum_thread_local_storage_size = MAX_TLS_SIZE,
      .initial_modes = RTEMS_DEFAULT_MODES,
      .attributes = TASK_ATTRIBUTES,
      .storage_area = &task_storage[TASK_COUNT - 2][0]};

  ctx.undefined_task_id = DoCreateTask(undef_task_config);
  StartTask(ctx.undefined_task_id, undefined_behaviour_task, (void *)&ctx);

  // Config our event sending task and start it
  rtems_task_config send_events_task_config = {
      .name = rtems_build_name('T', 'S', 'T', 'E'),
      .initial_priority = PRIO_NORMAL,
      .storage_size = TASK_STORAGE_SIZE,
      .maximum_thread_local_storage_size = MAX_TLS_SIZE,
      .initial_modes = RTEMS_DEFAULT_MODES,
      .attributes = TASK_ATTRIBUTES,
      .storage_area = &task_storage[TASK_COUNT - 1][0]};

  ctx.send_events_task_id = DoCreateTask(send_events_task_config);
  StartTask(ctx.send_events_task_id, send_events_task, (void *)&ctx);

  // Wait for all tasks to complete
  while (wait < CALCULATION_TASKS + 1)
  {
    ObtainCounterSemaphore(TASK_COUNTER_SEMAPHORE);
    wait++;
  }

  // Check concurrent executions
  uint32_t latest_task_start = 0;
  // Wraps around to effectively be maximum allowed value
  uint32_t earliest_task_finish = -1;

  for (uint8_t task = 0; task < CALCULATION_TASKS; task++)
  {
    latest_task_start = MAX(latest_task_start, task_before_mandelbrot[task]);
    earliest_task_finish = MIN(earliest_task_finish, task_after_mandelbrot[task]);
  }

  if ((ctx.functions_start >= latest_task_start && ctx.functions_end <= earliest_task_finish))
  {
    print_string("Functions and Mandelbrot executed concurrently: true\n\n");
  }
  else
  {
    print_string("Functions and Mandelbrot executed concurrently: false\n\n");
  }

  (ctx.is_expected_events) ? print_string("EXPECTED EVENTS: TRUE\n")
                           : print_string("EXPECTED EVENTS: FALSE\n");
  
  // Where each binary digit represents a passing / failing test
  print_string("CUMULATIVE TOTAL OF EVENTS RECEIVED IN BINARY: ");
  print_string(itoa(ctx.received_events_set, &str[0], 2));
  print_string("\n");

  print_test_results();

  rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, 0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS TEST_PROCESSORS

// Reduce the priority of the Init task so all other tasks can be scheduled to all other cores
#define CONFIGURE_INIT_TASK_PRIORITY PRIO_LOW

#define CONFIGURE_MAXIMUM_BARRIERS 1

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_MAXIMUM_PERIODS 1

// Number of Tasks configured + 1 for the INIT task
#define CONFIGURE_MAXIMUM_TASKS (TASK_COUNT) + 1

#define CONFIGURE_SCHEDULER_EDF_SMP

#define CONFIGURE_MINIMUM_TASK_STACK_SIZE RTEMS_MINIMUM_STACK_SIZE + CPU_STACK_ALIGNMENT

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

#define CONFIGURE_INIT_TASK_INITIAL_MODE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
