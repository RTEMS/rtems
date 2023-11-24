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

#include "../shared/isvv_rtems_aux.h"
#include "../shared/utils.h"
#include <rtems.h>
#include <rtems/bspIo.h>

/**
 *
 * @brief Tests impact of undefined behaviours
 *
 * This test case performs the calculation of the Mandelbrot set in
 * parallel with execution of calls to semaphores configured with
 * attributes with undefined behaviour
 *
 */

#define ITOA_STR_SIZE (8 * sizeof(int) + 1)

#define MAX_TLS_SIZE RTEMS_ALIGN_UP(64, RTEMS_TASK_STORAGE_ALIGNMENT)

#define TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define TASK_STORAGE_SIZE                                                      \
  RTEMS_TASK_STORAGE_SIZE(MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE,             \
                          TASK_ATTRIBUTES)

// test specific global vars
#define TASK_COUNT ISVV_TEST_PROCESSORS

#define MANDL_TASKS (TASK_COUNT - 1)
#define LAST_TASK (TASK_COUNT - 1)
#define TOTAL_TILES 64

#define NUMBER_OF_SEM_CREATE_TESTS 10
#define CREATE_TEST_MUTEX_FAILED 0xfffffffe
#define NUMBER_OF_SEM_SET_PRIO_TESTS 4
#define LOOPS_PER_TEST 3
#define RELATIVE_SLEEP 0
#define FUNCTION_WAIT_MICROSECONDS 8
#define FUNCTION_WAIT_NANOSECONDS (FUNCTION_WAIT_MICROSECONDS * 1000)

rtems_event_set event_send[] = {RTEMS_EVENT_1, RTEMS_EVENT_2, RTEMS_EVENT_3,
                                RTEMS_EVENT_4};
// MUST NOT overlap event_send[]
#define FLUSH_EVENT RTEMS_EVENT_8

typedef struct {
  rtems_attribute att_config;
  char str_output[133];
} semaphore_attribute;

typedef struct {
  rtems_id main_task;
  uint8_t ntiles;
  rtems_id task_id[TASK_COUNT];
  rtems_id barrier_id;
  rtems_id sem_id;
  bool sem_obtained;
  rtems_attribute sem_obtain_attributes;
} test_context;

static semaphore_attribute sem_create_attributes[NUMBER_OF_SEM_CREATE_TESTS] =
{
    {RTEMS_LOCAL | RTEMS_GLOBAL, "RTEMS_LOCAL | RTEMS_GLOBAL"},
    {RTEMS_BINARY_SEMAPHORE | RTEMS_FIFO | RTEMS_PRIORITY, "RTEMS_BINARY_SEMAPHORE | RTEMS_FIFO | RTEMS_PRIORITY"},
    {RTEMS_COUNTING_SEMAPHORE | RTEMS_BINARY_SEMAPHORE, "RTEMS_COUNTING_SEMAPHORE | RTEMS_BINARY_SEMAPHORE"},
    {RTEMS_COUNTING_SEMAPHORE | RTEMS_SIMPLE_BINARY_SEMAPHORE, "RTEMS_COUNTING_SEMAPHORE | RTEMS_SIMPLE_BINARY_SEMAPHORE"},
    {RTEMS_BINARY_SEMAPHORE | RTEMS_SIMPLE_BINARY_SEMAPHORE, "RTEMS_BINARY_SEMAPHORE | RTEMS_SIMPLE_BINARY_SEMAPHORE"},
    {RTEMS_COUNTING_SEMAPHORE | RTEMS_BINARY_SEMAPHORE | RTEMS_SIMPLE_BINARY_SEMAPHORE, "RTEMS_COUNTING_SEMAPHORE | RTEMS_BINARY_SEMAPHORE | " "RTEMS_SIMPLE_BINARY_SEMAPHORE"},
    {RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_PRIORITY_CEILING | RTEMS_INHERIT_PRIORITY, "RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_PRIORITY_CEILING | " "RTEMS_INHERIT_PRIORITY"},
    {RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_PRIORITY_CEILING | RTEMS_MULTIPROCESSOR_RESOURCE_SHARING, "RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_PRIORITY_CEILING | " "RTEMS_MULTIPROCESSOR_RESOURCE_SHARING"},
    {RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY | RTEMS_MULTIPROCESSOR_RESOURCE_SHARING, "RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY | " "RTEMS_MULTIPROCESSOR_RESOURCE_SHARING"},
    {RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY_CEILING | RTEMS_MULTIPROCESSOR_RESOURCE_SHARING, "RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY | " "RTEMS_PRIORITY_CEILING | RTEMS_MULTIPROCESSOR_RESOURCE_SHARING"}
};

#define NO_LOCKING_PROTOCOL ( RTEMS_NO_INHERIT_PRIORITY | RTEMS_NO_PRIORITY_CEILING | RTEMS_NO_MULTIPROCESSOR_RESOURCE_SHARING )
// These are the only attribute set configurations that are both allowed by rtems_semaphore_create() and
// are not explicitly not undefined behaviour for rtems_semaphore_set_priority().
static semaphore_attribute sem_set_prio_attributes[NUMBER_OF_SEM_SET_PRIO_TESTS] =
{
  {RTEMS_COUNTING_SEMAPHORE | RTEMS_PRIORITY | NO_LOCKING_PROTOCOL, "RTEMS_COUNTING_SEMAPHORE | RTEMS_PRIORITY | NO_LOCKING_PROTOCOL"},
  {RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | NO_LOCKING_PROTOCOL, "RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | NO_LOCKING_PROTOCOL"},
  {RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY, "RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY"},
  {RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_PRIORITY | NO_LOCKING_PROTOCOL, "RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_PRIORITY | NO_LOCKING_PROTOCOL"}
};

RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static char calc_task_storage[TASK_COUNT][TASK_STORAGE_SIZE];

RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static uint32_t task_before_mandelbrot[TASK_COUNT - 1];

RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static uint32_t task_after_mandelbrot[TASK_COUNT - 1];

static void calc_task_function(rtems_task_argument arg) {
  test_context *ctx;

  ctx = (test_context *)arg;
  struct timespec uptime;

  rtems_id local_id = TaskSelfId();
  uint8_t task_idx = 0;

  for (int i = 0; i < MANDL_TASKS; i++) {
    if (ctx->task_id[i] == local_id) {
      task_idx = i;
      break;
    }
  }

  WaitAtBarrier(ctx->barrier_id);

  rtems_clock_get_uptime(&uptime);
  task_before_mandelbrot[task_idx] =
      (uint32_t)(uptime.tv_sec * 1000000 + uptime.tv_nsec / 1000);

  mandelbrot_tile(task_idx + 1, ctx->ntiles);

  rtems_clock_get_uptime(&uptime);
  task_after_mandelbrot[task_idx] =
      (uint32_t)(uptime.tv_sec * 1000000 + uptime.tv_nsec / 1000);

  SendEvents(ctx->main_task, event_send[task_idx]);

  SuspendSelf();
}

static void semaphore_test_task(rtems_task_argument arg)
{
  test_context *ctx;

  ctx = (test_context *)arg;

  ObtainMutex(ctx->sem_id);
  ctx->sem_obtained = true;
  ReleaseMutex(ctx->sem_id);

  SendEvents(ctx->main_task, event_send[LAST_TASK]);
  SuspendSelf();
}

static void semaphore_create_test_task(rtems_task_argument arg)
{
  semaphore_test_task(arg);
}

static void semaphore_set_prio_run_task(rtems_task_argument arg)
{
  semaphore_test_task(arg);
}

static void semaphore_obtain_test_task(rtems_task_argument arg) {
  test_context *ctx;

  ctx = (test_context *)arg;
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(ctx->sem_id, ctx->sem_obtain_attributes, RTEMS_NO_TIMEOUT);
  if(sc == RTEMS_SUCCESSFUL)
  {
    ctx->sem_obtained = true;
    ReleaseMutex(ctx->sem_id);
  }

  SendEvents(ctx->main_task, event_send[LAST_TASK]);
  SuspendSelf();
}

static void semaphore_flush_test_task(rtems_task_argument arg) {
  test_context *ctx;
  ctx = (test_context *)arg;

  ObtainMutex(ctx->sem_id);
  // Let the main task know this task has been started and owns the mutex.
  SendEvents(ctx->main_task, FLUSH_EVENT);
  // Wait for event from main task to ensure this function still holds the
  // mutex when rtems_semaphore_flush() is called.
  ReceiveAllEvents(FLUSH_EVENT);
  ReleaseMutex(ctx->sem_id);

  SendEvents(ctx->main_task, event_send[LAST_TASK]);
  
  SuspendSelf();
}

static rtems_id CreateTestMutex(rtems_attribute attr) {
  rtems_status_code sc;
  rtems_id id;

  id = INVALID_ID;
  sc = rtems_semaphore_create(rtems_build_name('S', 'E', 'M', '0'), 1, attr, 0, &id);
  if (sc == RTEMS_NOT_DEFINED)
    return id;

  if(sc != RTEMS_SUCCESSFUL)
    return CREATE_TEST_MUTEX_FAILED;

  return id;
}

static void Init(rtems_task_argument arg) {
  (void)arg;
  test_context ctx;
  char ch;
  rtems_event_set received = 0;
  rtems_event_set total_events = 0;
  char str[ITOA_STR_SIZE];
  struct timespec uptime, function_wait;
  uint32_t functions_start, functions_end;
  rtems_status_code sc;
  bool sem_create_results[NUMBER_OF_SEM_CREATE_TESTS * LOOPS_PER_TEST] = {false};
  uint8_t sem_create_index = 0;
  bool sem_obtain_results[LOOPS_PER_TEST] = {false};
  bool sem_flush_results[LOOPS_PER_TEST] = {false};
  bool sem_set_prio_results[NUMBER_OF_SEM_SET_PRIO_TESTS * LOOPS_PER_TEST] = {false};
  uint8_t sem_set_prio_index = 0;

  SetSelfPriority(PRIO_VERY_ULTRA_HIGH);

  function_wait.tv_sec = 0;
  function_wait.tv_nsec = FUNCTION_WAIT_NANOSECONDS;
  ctx.main_task = rtems_task_self();
  ctx.ntiles = MANDL_TASKS;
  uint8_t nwaiting = ctx.ntiles;

  ctx.barrier_id = CreateAutomaticBarrier(nwaiting);

  rtems_task_config calc_task_config = {.initial_priority = PRIO_NORMAL,
                                        .storage_size = TASK_STORAGE_SIZE,
                                        .maximum_thread_local_storage_size =
                                            MAX_TLS_SIZE,
                                        .initial_modes = RTEMS_DEFAULT_MODES,
                                        .attributes = TASK_ATTRIBUTES};

  // create the mandlebrot set using all available cores minus one
  for (uint32_t i = 0; i < MANDL_TASKS; i++) {
    ch = '0' + i;
    calc_task_config.name = rtems_build_name('R', 'U', 'N', ch);
    calc_task_config.storage_area = &calc_task_storage[i][0];

    ctx.task_id[i] = DoCreateTask(calc_task_config);
    StartTask(ctx.task_id[i], calc_task_function, &ctx);
    total_events += event_send[i];
  }

  // with the last available core test undefined behaviour semaphores

  // Wait FUNCTION_WAIT_MICROSECONDS microseconds to ensure function calls are concurrent with mandelbrot calculation
  clock_nanosleep(CLOCK_MONOTONIC, RELATIVE_SLEEP, &function_wait, NULL);
  rtems_clock_get_uptime(&uptime);
  functions_start = (uint32_t)(uptime.tv_sec * 1000000 + uptime.tv_nsec / 1000);

  // #################### rtems_semaphore_create ####################
  // run each trio of tests
  for (uint32_t tested_sem = 0; tested_sem < NUMBER_OF_SEM_CREATE_TESTS; tested_sem++)
  {
    // the first and last run are with a normal configuration, the middle run
    // is with configuration with undefined behaviour
    for (uint32_t i = 0; i < LOOPS_PER_TEST; i++)
    {
      ctx.sem_obtained = false;

      if (i == 1)
      {
        ctx.sem_id = CreateTestMutex(sem_create_attributes[tested_sem].att_config);
      }
      else
      {
        ctx.sem_id = CreateTestMutex(RTEMS_DEFAULT_ATTRIBUTES);
      }

      if (ctx.sem_id == INVALID_ID)
      {
        // If rtems_semaphore_create() returns RTEMS_NOT_DEFINED for a bad
        // attribute set this is okay.
        if(i == 1)
        {
          sem_create_results[sem_create_index] = true;
        }
        
        sem_create_index++;
        continue;
      }
      else if(ctx.sem_id == CREATE_TEST_MUTEX_FAILED)
      {
        sem_create_index++;
        continue;
      }

      calc_task_config.name = rtems_build_name('T', 'S', 'E', 'M');
      calc_task_config.storage_area = &calc_task_storage[LAST_TASK][0];

      ctx.task_id[LAST_TASK] = DoCreateTask(calc_task_config);
      // execute task that uses the created semaphores
      ObtainMutex(ctx.sem_id);
      StartTask(ctx.task_id[LAST_TASK], semaphore_create_test_task, &ctx);
      ReleaseMutex(ctx.sem_id);

      ReceiveAllEvents(event_send[LAST_TASK]);
      DeleteMutex(ctx.sem_id);
      DeleteTask(ctx.task_id[LAST_TASK]);

      // If we get to this point the attribute set was usable and we have not
      // been able to detect an error from previous attribute sets.
		  if(ctx.sem_obtained == true)
		  {
		    sem_create_results[sem_create_index] = true;
		  }

      sem_create_index++;
    }
  }

  // #################### rtems_semaphore_obtain ####################
  for (uint32_t i = 0; i < LOOPS_PER_TEST; i++)
  {
    // test the obtain semaphore function with undefined behaviour attributes
    ctx.sem_obtained = false;
    ctx.sem_obtain_attributes = RTEMS_WAIT;
    ctx.sem_id = CreateMutex(rtems_build_name('S', 'E', 'M', '1'));

    if (i == 1)
    {
      ctx.sem_obtain_attributes |= RTEMS_NO_WAIT;
    }

    calc_task_config.name = rtems_build_name('T', 'S', 'E', 'M');
    calc_task_config.storage_area = &calc_task_storage[LAST_TASK][0];

    ctx.task_id[LAST_TASK] = DoCreateTask(calc_task_config);
    // execute task that uses the created semaphores
    StartTask(ctx.task_id[LAST_TASK], semaphore_obtain_test_task, &ctx);
    ReceiveAllEvents(event_send[LAST_TASK]);
    if(ctx.sem_obtained == true)
    {
      sem_obtain_results[i] = true;
    }

    DeleteMutex(ctx.sem_id);
    DeleteTask(ctx.task_id[LAST_TASK]);
  }

  // #################### rtems_semaphore_flush ####################
  for (uint32_t i = 0; i < LOOPS_PER_TEST; i++)
  {
    if (i == 1) {
      ctx.sem_id = CreateMutexMrsP(rtems_build_name('S', 'E', 'M', '2'));
    } else {
      ctx.sem_id = CreateMutex(rtems_build_name('S', 'E', 'M', '2'));
    }

    calc_task_config.name = rtems_build_name('T', 'S', 'E', 'M');
    calc_task_config.storage_area = &calc_task_storage[LAST_TASK][0];

    ctx.task_id[LAST_TASK] = DoCreateTask(calc_task_config);
    // execute task that uses the created semaphores
    StartTask(ctx.task_id[LAST_TASK], semaphore_flush_test_task, &ctx);

    // Ensure flush task has the mutex.
    ReceiveAllEvents(FLUSH_EVENT);
    sc = rtems_semaphore_flush(ctx.sem_id);
    // Allow flush task to continue after we have flushed.
    SendEvents(ctx.task_id[LAST_TASK], FLUSH_EVENT);
    if(i == 1)
    {
      // We expect RTEMS_NOT_DEFINED for MrsP.
      if(sc == RTEMS_NOT_DEFINED)
        sem_flush_results[i] = true;
    }
    else
    {
      if(sc == RTEMS_SUCCESSFUL)
        sem_flush_results[i] = true;
    }

    ReceiveAllEvents(event_send[LAST_TASK]);
    DeleteMutex(ctx.sem_id);
    DeleteTask(ctx.task_id[LAST_TASK]);
  }

  rtems_task_priority old_prio;
  rtems_id sched_id;
  // #################### rtems_semaphore_set_priority ####################
  // run each trio of tests
  for (uint32_t tested_sem = 0; tested_sem < NUMBER_OF_SEM_SET_PRIO_TESTS; tested_sem++)
  {
    for (uint32_t i = 0; i < LOOPS_PER_TEST; i++)
    {
      ctx.sem_obtained = false;
      // test the obtain semaphore function with undefined behaviour attributes
      if(i == 1)
      {
        ctx.sem_id = CreateTestMutex(sem_set_prio_attributes[tested_sem].att_config);
      }
      else
      {
        // Documentation explicitly states this attribute set makes for a semaphore with configurable priority.
        ctx.sem_id = CreateTestMutex(RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_PRIORITY_CEILING);
      }

      if(ctx.sem_id == INVALID_ID || ctx.sem_id == CREATE_TEST_MUTEX_FAILED)
      {
        // Failed to create semaphore with given attribute set so it cannot be tested.
        sem_set_prio_index++;
        continue;
      }

      sc = rtems_task_get_scheduler(RTEMS_SELF, &sched_id);
      ASSERT_SUCCESS(sc);

      calc_task_config.name = rtems_build_name('T', 'S', 'E', 'M');
      calc_task_config.storage_area = &calc_task_storage[LAST_TASK][0];

      ctx.task_id[LAST_TASK] = DoCreateTask(calc_task_config);
      SetScheduler(ctx.task_id[LAST_TASK], sched_id, PRIO_LOW);

      sc = rtems_semaphore_set_priority(ctx.sem_id, sched_id, PRIO_ULTRA_HIGH, &old_prio);
      if(sc == RTEMS_NOT_DEFINED)
      {
        if(i == 1)
        {
          sem_set_prio_results[sem_set_prio_index] = true;
        }
      }
      else if(sc == RTEMS_SUCCESSFUL)
      {
        StartTask(ctx.task_id[LAST_TASK], semaphore_set_prio_run_task, &ctx);
        ReceiveAllEvents(event_send[LAST_TASK]);
        if(ctx.sem_obtained == true)
        {
          sem_set_prio_results[sem_set_prio_index] = true;
        }
      }

      DeleteMutex(ctx.sem_id);
      DeleteTask(ctx.task_id[LAST_TASK]);
      sem_set_prio_index++;
    }
  }

  rtems_clock_get_uptime(&uptime);
  functions_end = (uint32_t)(uptime.tv_sec * 1000000 + uptime.tv_nsec / 1000);

  while (received != total_events) {
    received |= ReceiveAllEvents(total_events);
  }

  uint32_t latest_task_start = 0;
  uint32_t earliest_task_finish = -1;
  for (uint32_t task = 0; task < MANDL_TASKS; task++) {
    if (task_before_mandelbrot[task] > latest_task_start)
      latest_task_start = task_before_mandelbrot[task];
    if (task_after_mandelbrot[task] < earliest_task_finish)
      earliest_task_finish = task_after_mandelbrot[task];
  }

  print_string("Functions and mandelbrot executed concurrently: ");
  if (functions_start > latest_task_start && functions_end < earliest_task_finish)
  {
    print_string("true\n");
  }
  else
  {
    print_string("false\n");

    print_string("functions start (");
    print_string(itoa(functions_start, &str[0], 10));
    print_string(") must be greater than latest task start (");
    print_string(itoa(latest_task_start, &str[0], 10));
    print_string(") and functions end (");
    print_string(itoa(functions_end, &str[0], 10));
    print_string(") must be less than earliest task finish (");
    print_string(itoa(earliest_task_finish, &str[0], 10));
    print_string(")\n");
  }


  // If any of the rtems_semaphore_create() calls went wrong the test failed.
  bool create_success = true;
  for(uint8_t i = 0; i < sizeof(sem_create_results)/sizeof(sem_create_results[0]); i++)
  {
    if(sem_create_results[i] == false)
    {
      print_string("First failing sem create index: ");
      print_string(itoa(i, &str[0], 10));
      print_string("\n");
      print_string("Test attribute set of failing trio: ");
      print_string(sem_create_attributes[i / LOOPS_PER_TEST].str_output);
      print_string("\n");
      create_success = false;
      break;
    }
  }

  print_string("rtems_semaphore_create() unaffected: ");
  if(create_success)
  {
    print_string("true\n");
  }
  else
  {
    print_string("false\n");
  }

  // If any of the rtems_semaphore_obtain() calls went wrong the test failed.
  bool obtain_success = true;
  for(uint8_t i = 0; i < sizeof(sem_obtain_results)/sizeof(sem_obtain_results[0]); i++)
  {
    if(sem_obtain_results[i] == false)
    {
      print_string("First failing sem obtain index: ");
      print_string(itoa(i, &str[0], 10));
      print_string("\n");
      obtain_success = false;
      break;
    }
  }

  print_string("rtems_semaphore_obtain() unaffected: ");
  if(obtain_success)
  {
    print_string("true\n");
  }
  else
  {
    print_string("false\n");
  }

  // If any of the rtems_semaphore_flush() calls went wrong the test failed.
  bool flush_success = true;
  for(uint8_t i = 0; i < sizeof(sem_flush_results)/sizeof(sem_flush_results[0]); i++)
  {
    if(sem_flush_results[i] == false)
    {
      print_string("First failing sem flush index: ");
      print_string(itoa(i, &str[0], 10));
      print_string("\n");
      flush_success = false;
      break;
    }
  }

  print_string("rtems_semaphore_flush() unaffected: ");
  if(flush_success)
  {
    print_string("true\n");
  }
  else
  {
    print_string("false\n");
  }

  // If any of the rtems_semaphore_set_priority() calls went wrong the test failed.
  bool set_prio_success = true;
  for(uint8_t i = 0; i < sizeof(sem_set_prio_results)/sizeof(sem_set_prio_results[0]); i++)
  {
    if(sem_set_prio_results[i] == false)
    {
      print_string("First failing sem set priority index: ");
      print_string(itoa(i, &str[0], 10));
      print_string("\n");
      print_string("Test attribute set of failing trio: ");
      print_string(sem_set_prio_attributes[i / LOOPS_PER_TEST].str_output);
      print_string("\n");
      set_prio_success = false;
      break;
    }
  }

  print_string("rtems_semaphore_set_priority() unaffected: ");
  if(set_prio_success)
  {
    print_string("true\n");
  }
  else
  {
    print_string("false\n");
  }

  print_test_results();

  for (uint32_t i = 0; i < MANDL_TASKS; i++) {
    DeleteTask(ctx.task_id[i]);
  }
  DeleteBarrier(ctx.barrier_id);

  rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, 0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS ISVV_TEST_PROCESSORS

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_MAXIMUM_BARRIERS 1

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
