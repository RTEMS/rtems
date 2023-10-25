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
 * @brief Tests impact of undefined behaviour for task manager
 *
 * This test case performs the calculation of the Mandelbrot set in parallel with
 * execution of task related functions with conditions which may lead to
 * undefined behaviour, and seeks to clarify the impact of them.
 *
 */

#define ITOA_STR_SIZE (8 * sizeof(int) + 1)

#define MAX_TLS_SIZE RTEMS_ALIGN_UP(64, RTEMS_TASK_STORAGE_ALIGNMENT)

#define TASK_STORAGE_SIZE                      \
  RTEMS_TASK_STORAGE_SIZE(                     \
      MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE, \
      TASK_ATTRIBUTES)

#define TASK_ATTRIBUTES RTEMS_FLOATING_POINT

// We want to create mandelbrot executions whilst our task manager functions
// are executed on another core
#define TASK_COUNT TEST_PROCESSORS
#define CALCULATION_TASKS ((TASK_COUNT) - 1)

// Timing related variables to ensure concurrent executions
#define RELATIVE_SLEEP 0
#define FUNCTION_WAIT_MICROSECONDS 10
#define FUNCTION_WAIT_NANOSECONDS (FUNCTION_WAIT_MICROSECONDS * 1000)

static rtems_id start_barrier;
static rtems_id TASK_COUNTER_SEMAPHORE;

typedef struct
{
  rtems_id calc_task_id[CALCULATION_TASKS];
  rtems_id undefined_task_id;
  uint32_t functions_start, functions_end;
  bool expected_timeslice_calls;
  bool expected_ASR_calls;
  bool expected_preempt_calls;
  bool expected_interrupt_calls;
  bool expected_combination_calls;
  bool preempt_not_impl;
  bool interrupts_not_impl;
} test_context;

// Create storage areas for each worker, using task construct forces
RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static char task_storage[TASK_COUNT][TASK_STORAGE_SIZE];

// Storage space for timings
RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static uint32_t task_before_mandelbrot[CALCULATION_TASKS];

RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static uint32_t task_after_mandelbrot[CALCULATION_TASKS];

static void calc_task_function(rtems_task_argument *tile)
{
  uint8_t tile_n = (uint8_t) *tile;
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

static void undefined_behaviour_task(rtems_task_argument arg)
{
  test_context *ctx = (test_context *)arg;
  struct timespec uptime, function_wait;

  WaitAtBarrier(start_barrier);

  // Wait for 10us to ensure concurrent executions
  function_wait.tv_sec = 0;
  function_wait.tv_nsec = FUNCTION_WAIT_NANOSECONDS;
  clock_nanosleep(CLOCK_MONOTONIC, 0, &function_wait, NULL);

  rtems_clock_get_uptime(&uptime);
  ctx->functions_start = (uint32_t)(uptime.tv_sec * 1000000 + uptime.tv_nsec / 1000);

  /*
   * Start execution of task-manager related functions following formula of:
   *
   * Defined behaviour
   * Undefined behaviour
   * Defined behaviour
   *
   * NOTE REGARDING SPECIFICATION:
   * This test of undefined behaviour shall be conducted for:
   *  - rtems_task_mode()
   * This test of undefined behaviour shall NOT be conducted for:
   *  - rtems_task_create()
   * As confirmed in RTEMS-SMP-VAL-001 implementation in the space profile for this directive
   * is not allowed, and therefore testing is defunct.
   *
   * To check the current status of the task execution mode, we must call rtems_task_mode
   * passing in RTEMS_CURRENT_MODE as the mask, which causes the first parameter to be ignored
   * and returns the state in the third parameter. As such we will use this to check possible
   * invalidation of state from undefined behaviour calls with appropriate assertions.
   *
   * When we set mutually exclusive options which are combined with a bitwise OR, what we see
   * is the non-default option being applied. Where setting certain options isn't possible due
   * to the SMP and space profile configuration, we will assert that appropriate errors being
   * thrown, and that our configuration hasn't been invalidated or corrupted.
   */

  // Define test specific variables
  rtems_mode mode_set;
  rtems_status_code sc;

  const rtems_mode TIMESLICE_MODE = RTEMS_TIMESLICE;
  const rtems_mode NO_ASR_MODE = RTEMS_NO_ASR;
  const rtems_mode NO_ASR_TIMESLICE_MODE = RTEMS_TIMESLICE | RTEMS_NO_ASR;

  /*
   * TIMESLICE
   */

  SetTaskMode(RTEMS_NO_TIMESLICE, RTEMS_TIMESLICE_MASK, mode_set);
  mode_set = GetTaskMode();
  ctx->expected_timeslice_calls &= ASSERT_TASK_MODES_EQ(mode_set, RTEMS_DEFAULT_MODES);

  // INVALID CALL
  SetTaskMode(RTEMS_NO_TIMESLICE | RTEMS_TIMESLICE, RTEMS_TIMESLICE_MASK, mode_set);
  mode_set = GetTaskMode();
  ctx->expected_timeslice_calls &= ASSERT_TASK_MODES_EQ(mode_set, TIMESLICE_MODE);

  SetTaskMode(RTEMS_NO_TIMESLICE, RTEMS_TIMESLICE_MASK, mode_set);
  mode_set = GetTaskMode();
  ctx->expected_timeslice_calls &= ASSERT_TASK_MODES_EQ(mode_set, RTEMS_DEFAULT_MODES);

  /*
   * Asynchronous Signal Routine (ASR)
   */

  // First with ASR being set

  SetTaskMode(RTEMS_ASR, RTEMS_ASR_MASK, mode_set);
  mode_set = GetTaskMode();
  ctx->expected_ASR_calls &= ASSERT_TASK_MODES_EQ(mode_set, RTEMS_DEFAULT_MODES);

  // INVALID CALL
  SetTaskMode(RTEMS_NO_ASR | RTEMS_ASR, RTEMS_ASR_MASK, mode_set);
  mode_set = GetTaskMode();
  ctx->expected_ASR_calls &= ASSERT_TASK_MODES_EQ(mode_set, NO_ASR_MODE);

  SetTaskMode(RTEMS_ASR, RTEMS_ASR_MASK, mode_set);
  mode_set = GetTaskMode();
  ctx->expected_ASR_calls &= ASSERT_TASK_MODES_EQ(mode_set, RTEMS_DEFAULT_MODES);

  // Then with NO_ASR being set

  SetTaskMode(RTEMS_NO_ASR, RTEMS_ASR_MASK, mode_set);
  mode_set = GetTaskMode();
  ctx->expected_ASR_calls &= ASSERT_TASK_MODES_EQ(mode_set, NO_ASR_MODE);

  // INVALID CALL
  SetTaskMode(RTEMS_NO_ASR | RTEMS_ASR, RTEMS_ASR_MASK, mode_set);
  mode_set = GetTaskMode();
  ctx->expected_ASR_calls &= ASSERT_TASK_MODES_EQ(mode_set, NO_ASR_MODE);

  SetTaskMode(RTEMS_ASR, RTEMS_ASR_MASK, mode_set);
  mode_set = GetTaskMode();
  ctx->expected_ASR_calls &= ASSERT_TASK_MODES_EQ(mode_set, RTEMS_DEFAULT_MODES);

  /*
   * PREEMPT
   *
   * Disabling Preemption on SMP systems is not possible, and any attempt to do so results in an
   * RTEMS_NOT_IMPLEMENTED error.
   */

  SetTaskMode(RTEMS_PREEMPT, RTEMS_PREEMPT_MASK, mode_set);
  mode_set = GetTaskMode();
  ctx->expected_preempt_calls &= ASSERT_TASK_MODES_EQ(mode_set, RTEMS_DEFAULT_MODES);

  // INVALID CALL
  // We are expecting RTEMS_NOT_IMPLEMENTED due to interrupts not being enabled so don't assert success
  sc = rtems_task_mode(RTEMS_PREEMPT | RTEMS_NO_PREEMPT, RTEMS_PREEMPT_MASK, &mode_set);
  ctx->preempt_not_impl = (RTEMS_NOT_IMPLEMENTED == sc);
  mode_set = GetTaskMode();
  ctx->expected_preempt_calls &= ASSERT_TASK_MODES_EQ(mode_set, RTEMS_DEFAULT_MODES);

  SetTaskMode(RTEMS_PREEMPT, RTEMS_PREEMPT_MASK, mode_set);
  mode_set = GetTaskMode();
  ctx->expected_preempt_calls &= ASSERT_TASK_MODES_EQ(mode_set, RTEMS_DEFAULT_MODES);

  /*
   * INTERRUPT LEVEL
   *
   * RTEMS allows up to 256 values depending on processor architecture to set the interrupt level,
   * where a level of 0 indicates all interrupts are disabled (identical regardless of architecture)
   * and progressive levels indicating more and more interrupts enabled.
   *
   * According to RTEMS CPU Architecture Documentation, maskable interrupts allowed range for the
   * LEON3 / LEON4 processors is from 0 to 15 (16 levels, where 0 indicates all interrupts disabled).
   *
   * However in the RTEMS Qualification SRS for both GR712 and GR740, there exists a constraint
   * (spec:/constraint/interrupts-disabled-smp) which states that 'Where the system was built with
   * SMP support enabled, maskable interrupts are disabled for the executing thread.'
   *
   * Therefore as we are using SMP configuration for these tests, trying to combine mutually exclusive
   * attributes, in this case enabling various interrupt levels through the maskable interrupt passed into
   * the mode_set for this task with values e.g. 0 | 1 | 2 violates this constant and will result in an
   * RTEMS_NOT_IMPLEMENTED error. This is the extent to which we can provide
   * assertions on the correctness of this undefined behaviour.
   */

  SetTaskMode(RTEMS_INTERRUPT_LEVEL(0), RTEMS_INTERRUPT_MASK, mode_set);
  mode_set = GetTaskMode();
  ctx->expected_interrupt_calls &= ASSERT_TASK_MODES_EQ(mode_set, RTEMS_DEFAULT_MODES);

  // INVALID CALL
  // We are expecting RTEMS_NOT_IMPLEMENTED due to interrupts not being enabled so don't assert success
  sc = rtems_task_mode(RTEMS_INTERRUPT_LEVEL(1) | RTEMS_INTERRUPT_LEVEL(2) | RTEMS_INTERRUPT_LEVEL(3), RTEMS_INTERRUPT_MASK, &mode_set);
  ctx->interrupts_not_impl = (RTEMS_NOT_IMPLEMENTED == sc);
  mode_set = GetTaskMode();
  ctx->expected_interrupt_calls &= ASSERT_TASK_MODES_EQ(mode_set, RTEMS_DEFAULT_MODES);

  SetTaskMode(RTEMS_INTERRUPT_LEVEL(0), RTEMS_INTERRUPT_MASK, mode_set);
  mode_set = GetTaskMode();
  ctx->expected_interrupt_calls &= ASSERT_TASK_MODES_EQ(mode_set, RTEMS_DEFAULT_MODES);

  /*
   * COMBINATIONS OF MUTUALLY EXCLUSIVE & MUTUALLY INCLUSIVE ATTRIBUTES
   */

  SetTaskMode(RTEMS_DEFAULT_MODES, RTEMS_ALL_MODE_MASKS, mode_set);
  mode_set = GetTaskMode();
  ctx->expected_combination_calls &= ASSERT_TASK_MODES_EQ(mode_set, RTEMS_DEFAULT_MODES);

  // INVALID CALL
  SetTaskMode(RTEMS_NO_ASR | RTEMS_ASR | RTEMS_TIMESLICE | RTEMS_NO_TIMESLICE, RTEMS_ASR_MASK | RTEMS_TIMESLICE_MASK, mode_set);
  mode_set = GetTaskMode();
  ctx->expected_combination_calls &= ASSERT_TASK_MODES_EQ(mode_set, NO_ASR_TIMESLICE_MODE);

  SetTaskMode(RTEMS_DEFAULT_MODES, RTEMS_ALL_MODE_MASKS, mode_set);
  mode_set = GetTaskMode();
  ctx->expected_combination_calls &= ASSERT_TASK_MODES_EQ(mode_set, RTEMS_DEFAULT_MODES);

  /*
   * FINISHED EXECUTION OF TASK RELATED FUNCTIONS
   */

  rtems_clock_get_uptime(&uptime);
  ctx->functions_end = (uint32_t)(uptime.tv_sec * 1000000 + uptime.tv_nsec / 1000);

  // Only once this task has finished do we know we can progress
  ReleaseCounterSemaphore(TASK_COUNTER_SEMAPHORE);
  rtems_task_exit();
}

static void Init(rtems_task_argument arg)
{
  (void)arg;
  test_context ctx;
  uint8_t wait = 0;
  uint32_t args[CALCULATION_TASKS] = {0};

  TASK_COUNTER_SEMAPHORE = CreateCounterSemaphore(rtems_build_name('T', 'C', 'S', '0'), 0);

  // Config for our mandelbrot tile function
  rtems_task_config calc_task_config = {
      .initial_priority = PRIO_NORMAL,
      .storage_size = TASK_STORAGE_SIZE,
      .maximum_thread_local_storage_size = MAX_TLS_SIZE,
      .initial_modes = RTEMS_DEFAULT_MODES,
      .attributes = TASK_ATTRIBUTES};

  // Create our barrier for mandelbrot tasks + undefined behaviour tasks
  start_barrier = CreateAutomaticBarrier(TASK_COUNT);

  // Finish config for our mandelbrot tile functions and start them
  for (uint32_t i = 0; i < CALCULATION_TASKS; i++)
  {
    calc_task_config.name = rtems_build_name('R', 'U', 'N', (char)i);
    calc_task_config.storage_area = &task_storage[i][0];

    ctx.calc_task_id[i] = DoCreateTask(calc_task_config);
    args[i] = (i + 1);

    StartTask(ctx.calc_task_id[i], (void *)calc_task_function, (void *)&args[i]);
  }

  // Set initial properties to check validity directives and results
  ctx.expected_timeslice_calls = ctx.expected_ASR_calls = ctx.expected_preempt_calls = ctx.expected_interrupt_calls = ctx.expected_combination_calls = TRUE;

  // Config our undefined behaviour task and start it
  rtems_task_config undef_task_config = {
      .name = rtems_build_name('U', 'N', 'D', 'F'),
      .initial_priority = PRIO_NORMAL,
      .storage_size = TASK_STORAGE_SIZE,
      .maximum_thread_local_storage_size = MAX_TLS_SIZE,
      .initial_modes = RTEMS_DEFAULT_MODES,
      .attributes = TASK_ATTRIBUTES,
      .storage_area = &task_storage[TASK_COUNT-1][0]};

  ctx.undefined_task_id = DoCreateTask(undef_task_config);
  StartTask(ctx.undefined_task_id, undefined_behaviour_task, (void *)&ctx);

  // Wait for all tasks to complete
  while (wait < TASK_COUNT)
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

  print_string("\n\n");
  if ((ctx.functions_start >= latest_task_start && ctx.functions_end <= earliest_task_finish))
  {
    print_string("Functions and mandelbrot executed concurrently: true\n\n");
  }
  else
  {
    print_string("Functions and mandelbrot executed concurrently: false\n\n");
  }

  // Check all directives yielded expected results
  if (ctx.expected_timeslice_calls && ctx.expected_ASR_calls && ctx.expected_preempt_calls && ctx.expected_timeslice_calls && ctx.expected_combination_calls)
  {
    print_string("Directives all yielded expected results and configuration is valid: true\n");
  }
  else
  {
    print_string("Directives all yielded expected results and configuration is valid: false\n");
  }

  // Check individual directives themselves
  (ctx.expected_timeslice_calls)    ? print_string("Timeslice directives: SUCCEEDED\n")   : print_string("Timeslice directives: FAILED\n");
  (ctx.expected_ASR_calls)          ? print_string("ASR directives: SUCCEEDED\n")         : print_string("ASR directives: FAILED\n");
  (ctx.expected_preempt_calls)      ? print_string("Preempt directives: SUCCEEDED\n")     : print_string("Preempt directives: FAILED\n");
  (ctx.expected_interrupt_calls)    ? print_string("Interrupt directives: SUCCEEDED\n")   : print_string("Interrupt directives: FAILED\n");
  (ctx.expected_combination_calls)  ? print_string("Combination directives: SUCCEEDED\n") : print_string("Combination directives: FAILED\n");

  // Check status codes returned are expected i.e. disallowed by config
  if (ctx.preempt_not_impl) 
  {
    print_string("Status code returned for RTEMS_PREEMPT is expected: true\n");
  }
  else 
  {
    print_string("Status code returned for RTEMS_PREEMPT is expected: false\n");
  }

  if (ctx.interrupts_not_impl) 
  {
    print_string("Status code returned for RTEMS_INTERRUPT_LEVEL > 0 is expected: true\n");
  }
  else 
  {
    print_string("Status code returned for RTEMS_INTERRUPT_LEVEL > 0 is expected: false\n");
  }

  // Print mandelbrot set
  print_test_results();

  // Perform tidy up operations
  rtems_barrier_delete(start_barrier);
  rtems_semaphore_delete(TASK_COUNTER_SEMAPHORE);
  rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, 0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS TEST_PROCESSORS

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

// Reduce the priority of the Init task so all other tasks can be schedules to all other cores
#define CONFIGURE_INIT_TASK_PRIORITY PRIO_LOW

#define CONFIGURE_MAXIMUM_BARRIERS 1

#define CONFIGURE_MAXIMUM_TASKS (TEST_PROCESSORS + 1)

#define CONFIGURE_SCHEDULER_EDF_SMP

#define CONFIGURE_MINIMUM_TASK_STACK_SIZE RTEMS_MINIMUM_STACK_SIZE + CPU_STACK_ALIGNMENT

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

#define CONFIGURE_INIT_TASK_INITIAL_MODE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
