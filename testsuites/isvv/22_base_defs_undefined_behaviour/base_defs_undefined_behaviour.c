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
 * @brief Tests impact of undefined behaviour when using RTEMS_ALIGN_DOWN
 *
 * This test case performs the calculation of the Mandelbrot set in parallel with
 * execution of RTEMS_ALIGN_DOWN directive calls with conditions which lead to
 * undefined behaviour
 * 
 */

#define ITOA_STR_SIZE (8 * sizeof(int) + 1)

#define MAX_TLS_SIZE RTEMS_ALIGN_UP(64, RTEMS_TASK_STORAGE_ALIGNMENT)

#define TASK_STORAGE_SIZE                      \
  RTEMS_TASK_STORAGE_SIZE(                     \
      MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE, \
      TASK_ATTRIBUTES)

#define TASK_ATTRIBUTES RTEMS_FLOATING_POINT

// One task for undefined task, and n-1 calculation tasks
#define TASK_COUNT TEST_PROCESSORS

// We want to create mandelbrot executions whilst our undefined calls are executing
#define CALCULATION_TASKS ((TEST_PROCESSORS)-1)

#define FUNCTION_CALLS 32
#define UNDEF_FUNC_CALLS (FUNCTION_CALLS / 2)

uint16_t align_value[FUNCTION_CALLS]      =   {0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16, 64, 127, 255, 256, 257, 511, 512, 513, 1022, 1023, 1024, 1025, 1026, 1027, 1028, 10000, 20000, 30000};
uint16_t size_target[FUNCTION_CALLS]      =   {1, 4, 2, 2, 2, 2, 2, 4, 8, 8, 2,  4,  8,  8,  8,  8,   16,  16,  16,  32,  32,  32,  32,   64,   64,   64,   128,  128,  128,  2,     4,     8    };
int64_t expected_results[FUNCTION_CALLS]  =   {0, 0, 0, 2, 2, 4, 4, 4, 0, 8, 10, 12, 8,  16, 64, 120, 240, 256, 256, 480, 512, 512, 992,  960,  1024, 1024, 1024, 1024, 1024, 10000, 20000, 30000};

int64_t undef_align_value[UNDEF_FUNC_CALLS] =   {0, 3, 10, 6, 16, 100, 128, 312, 10,   10,  2,    3                , 4         , 65535, 65537, 4294967295 };
int64_t undef_size_target[UNDEF_FUNC_CALLS] =   {0, 5, 6, 13, -1,  -8, 100,-128, 15,  -127, -128, (-2147483647 - 1), 2147483647, 65535, 65537, 4294967295 };

// Barrier to ensure calculation tasks and undefined behaviour tasks occur concurrently
static rtems_id start_barrier;
static rtems_id TASK_COUNTER_SEMAPHORE;

// Timing related variables to ensure concurrent executions
#define RELATIVE_SLEEP 0
#define FUNCTION_WAIT_MICROSECONDS 10
#define FUNCTION_WAIT_NANOSECONDS (FUNCTION_WAIT_MICROSECONDS * 1000)

typedef struct
{
  rtems_id calc_task_id[CALCULATION_TASKS];
  rtems_id undefined_task_id;
  int64_t results[FUNCTION_CALLS];
  uint32_t functions_start, functions_end;
} test_context;

// Create storage areas for each worker, using task construct forces
RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static char task_storage[TASK_COUNT][TASK_STORAGE_SIZE];

RTEMS_ALIGNED(CPU_STACK_ALIGNMENT)
static uint32_t task_before_mandelbrot[CALCULATION_TASKS];

RTEMS_ALIGNED(CPU_STACK_ALIGNMENT)
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
  test_context *ctx;
  ctx = (test_context *)arg;
  struct timespec uptime, function_wait;

  WaitAtBarrier(start_barrier);

  // Wait for 10us to ensure concurrent executions
  function_wait.tv_sec = 0;
  function_wait.tv_nsec = FUNCTION_WAIT_NANOSECONDS;
  clock_nanosleep(CLOCK_MONOTONIC, 0, &function_wait, NULL);

  rtems_clock_get_uptime(&uptime);
  ctx->functions_start = (uint32_t)(uptime.tv_sec * 1000000 + uptime.tv_nsec / 1000);

  /* Start execution of events related functions following formula of:
   *
   * Defined behaviour
   * Undefined behaviour
   * Defined behaviour
   *
   * The alignment shall be a power of two, otherwise the returned value is undefined.
   *
   * Check that RTEMS_ALIGN_DOWN() calculates the expected result and is
   * side-effect free.
   *
   * Alignment downwards aligns this value * down to the nearest multiple of a given size. 
   * This is to proper utilization of a given amount of memory and improve performance.
   *
   * The target of this alignment is for computer memory, which for RTEMS is always a power of two,
   * however the result of the function RTEMS_ALIGN_DOWN should be ensured to be used carefully in
   * actual program construction as other numbers can be used for the size.
   * 
   * NOTE: There is a comment in the definition of the RTEMS_ALIGN_DOWN which states that 'the 
   * alignment parameter is evaluated twice.' however this is not the case. This is the case, however,
   * for RTEMS_ALIGN_UP which performs two evaluations of the alignment parameter and therefore 
   * potentially the cause for the miscommunication.
   */

  int32_t target;
  int32_t size;

  for (uint8_t i = 0; i < FUNCTION_CALLS; i+=2) {
    ctx->results[i] = RTEMS_ALIGN_DOWN(align_value[i], size_target[i]);

    target = undef_align_value[i / 2];
    size = undef_size_target[i / 2];
    (void)RTEMS_ALIGN_DOWN(target, size);

    ctx->results[i + 1] = RTEMS_ALIGN_DOWN(align_value[i + 1], size_target[i + 1]);
  }

  rtems_clock_get_uptime(&uptime);
  ctx->functions_end = (uint32_t)(uptime.tv_sec * 1000000 + uptime.tv_nsec / 1000);

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
  uint32_t args[CALCULATION_TASKS] = {0};

  TASK_COUNTER_SEMAPHORE = CreateCounterSemaphore(rtems_build_name('T', 'C', 'S', '0'), 0);

  start_barrier = CreateAutomaticBarrier(TASK_COUNT);

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
    
    args[i] = (i + 1);
    StartTask(ctx.calc_task_id[i], (void *) calc_task_function, (void *) &args[i]);
  }

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

  // Check concurrent execution
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
    print_string("Functions and mandelbrot executed concurrently: true\n\n");
  }
  else
  {
    print_string("Functions and mandelbrot executed concurrently: false\n\n");
  }

  // Now check validity of all calls
  bool all_correct_flag = TRUE;
  for (uint8_t i = 0; i < FUNCTION_CALLS; i++)
  {
    // If our result isn't true like we expect, print out which are unexpected
    if (ctx.results[i] != expected_results[i])
    {
      all_correct_flag = FALSE;
      print_string("RTEMS_ALIGN_DOWN unexpected result detected: #");
      print_string(itoa(i, &str[0], 10));
      print_string(" is not correct and what was received was: ");
      print_string(itoa(ctx.results[i], &str[0], 10));
      print_string(" and should have been: ");
      print_string(itoa(expected_results[i], &str[0], 10));
      print_string(" for given value: ");
      print_string(itoa(align_value[i], &str[0], 10));
      print_string(", and size: ");
      print_string(itoa(size_target[i], &str[0], 10));
      print_string("\n");
    }
  }

  if (all_correct_flag) 
  {
    print_string("All alignment calls return expected results: true\n\n");
  } 
  else 
  {
    print_string("All alignment calls return expected results: false\n\n");
  }

  // Print results of mandelbrot test
  print_test_results();

  rtems_barrier_delete(start_barrier);
  rtems_semaphore_delete(TASK_COUNTER_SEMAPHORE);
  rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, 0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS TEST_PROCESSORS

// Reduce the priority of the Init task so all other tasks can be schedules to all other cores
#define CONFIGURE_INIT_TASK_PRIORITY PRIO_LOW

#define CONFIGURE_MAXIMUM_BARRIERS 1

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

// Number of Tasks configured + 1 for the INIT task
#define CONFIGURE_MAXIMUM_TASKS ((TASK_COUNT) + 1)

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