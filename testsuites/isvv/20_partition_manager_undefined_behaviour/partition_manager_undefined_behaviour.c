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
 * @brief Tests impact of undefined behaviour for partition manager
 *
 * This test case performs the calculation of the Mandelbrot set in parallel with
 * execution of partition related functions with conditions which lead to
 * undefined behaviour
 * @{
 */

#define ITOA_STR_SIZE (8 * sizeof(int) + 1)

#define MAX_TLS_SIZE RTEMS_ALIGN_UP(64, RTEMS_TASK_STORAGE_ALIGNMENT)

#define TASK_STORAGE_SIZE                      \
  RTEMS_TASK_STORAGE_SIZE(                     \
      MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE, \
      TASK_ATTRIBUTES)

#define TASK_ATTRIBUTES RTEMS_FLOATING_POINT

// test specific global vars

// We want to create mandelbrot executions whilst our manager functions
// Are executed on another core
#define TASK_COUNT TEST_PROCESSORS
#define CALCULATION_TASKS ((TEST_PROCESSORS) - 1)

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
  bool valid_call_RES_IN_USE_1, valid_call_RES_IN_USE_2, invalid_call_RES_IN_USE;
} test_context;

// Create storage areas from which to create our partitions
RTEMS_ALIGNED(CPU_STACK_ALIGNMENT)
static char partition_storage[65536];

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

  /* Start execution of partition related functions following formula of:
   * Defined behaviour
   * Undefined behaviour
   * Defined behaviour
   *
   * Where attribute_set has certain values combined with a bitwise OR. Values combined that are mutually
   * exclusive is categorised as undefined behaviour. What we see when combining these values is the
   * non-default option takes precedence and is applied, in this case that is the RTEMS_GLOBAL attribute.
   *
   * According to the RTEMS documentation, setting the global attribute in a single node system has no effect.
   * As we are restricted to a single-node system with this qualification pack, and the goal is to assert
   * SMP functionality (rather than MPCI functionality) we will not be testing multiple nodes when using the
   * RTEMS_GLOBAL attribute.
   *
   * Therefore to check the functionality and correctness for the directive rtems_create_partition, we will
   * perform an assertion on the returned status code, as well as both request and return a buffer
   * of a size pre-defined in the _create call. Before returning the buffer, we will attempt to delete
   * the partition. In correct program function, this should not be allowed, and will allow us to
   * match correct program behaviour when using undefined behaviour.
   */

  // Define test relevant variables
  rtems_status_code sc;
  rtems_id partition_id;
  void *starting_address;
  void *buffer_ptr;

  // We will reuse the same starting address as the memory from previous partition SHOULD have been returned to RTEMS
  starting_address = &partition_storage;

  /*
   * VALID CALL - attribute_set = RTEMS_LOCAL
   */

  // Create our partition in the designated aligned space
  ASSERT_SUCCESS(rtems_partition_create(rtems_build_name('P', 'R', 'T', '1'), &starting_address, 128, 32, RTEMS_LOCAL, &partition_id));
  // Try to create a buffer from the partition
  ASSERT_SUCCESS(rtems_partition_get_buffer(partition_id, &buffer_ptr));

  // Status code returned should indicate action is not allowed
  sc = rtems_partition_delete(partition_id);
  ctx->valid_call_RES_IN_USE_1 = (RTEMS_RESOURCE_IN_USE == sc);

  // Now return the buffer and clean up the partition
  ASSERT_SUCCESS(rtems_partition_return_buffer(partition_id, buffer_ptr));
  ASSERT_SUCCESS(rtems_partition_delete(partition_id));

  /*
   * INVALID CALL - attribute_set = RTEMS_LOCAL | RTEMS_GLOBAL
   */

  ASSERT_SUCCESS(rtems_partition_create(rtems_build_name('P', 'R', 'T', 'I'), &starting_address, 128, 32, RTEMS_LOCAL | RTEMS_GLOBAL, &partition_id));
  ASSERT_SUCCESS(rtems_partition_get_buffer(partition_id, &buffer_ptr));

  // To assess the effectiveness of the directive in question, we will try to match with known correct program behaviour.
  // Trying to delete a partition with an unreturned buffer should throw the same error as in the valid call.
  sc = rtems_partition_delete(partition_id);
  ctx->invalid_call_RES_IN_USE = (RTEMS_RESOURCE_IN_USE == sc);

  ASSERT_SUCCESS(rtems_partition_return_buffer(partition_id, buffer_ptr));
  ASSERT_SUCCESS(rtems_partition_delete(partition_id));

  /*
   * VALID CALL - attribute_set = RTEMS_LOCAL
   */

  ASSERT_SUCCESS(rtems_partition_create(rtems_build_name('P', 'R', 'T', '2'), &starting_address, 128, 32, RTEMS_LOCAL, &partition_id));
  ASSERT_SUCCESS(rtems_partition_get_buffer(partition_id, &buffer_ptr));

  sc = rtems_partition_delete(partition_id);
  ctx->valid_call_RES_IN_USE_2 = (RTEMS_RESOURCE_IN_USE == sc);

  ASSERT_SUCCESS(rtems_partition_return_buffer(partition_id, buffer_ptr));
  ASSERT_SUCCESS(rtems_partition_delete(partition_id));

  /*
   * FINISHED EXECUTION OF PARTITION RELATED FUNCTIONS
   */

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
  uint8_t wait = 0;

  TASK_COUNTER_SEMAPHORE = CreateCounterSemaphore(rtems_build_name('T', 'C', 'S', '0'), 0);

  // Config for our mandelbrot tile function
  rtems_task_config calc_task_config = {
      .initial_priority = PRIO_NORMAL,
      .storage_size = TASK_STORAGE_SIZE,
      .maximum_thread_local_storage_size = MAX_TLS_SIZE,
      .initial_modes = RTEMS_DEFAULT_MODES,
      .attributes = TASK_ATTRIBUTES};

  // Create our barrier for mandelbrot tasks + undefined behaviour task
  start_barrier = CreateAutomaticBarrier(TASK_COUNT);

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
      .storage_area = &task_storage[TASK_COUNT-1][0]};

  // Set properties to check for functionality of directives
  ctx.valid_call_RES_IN_USE_1 = ctx.invalid_call_RES_IN_USE = ctx.valid_call_RES_IN_USE_2 = FALSE;

  ctx.undefined_task_id = DoCreateTask(undef_task_config);
  StartTask(ctx.undefined_task_id, undefined_behaviour_task, (void *)&ctx);

  // Wait for all tasks to complete
  while (wait < TASK_COUNT)
  {
    ObtainCounterSemaphore(TASK_COUNTER_SEMAPHORE);
    wait++;
  }

  // Check RESOURCE_IN_USE errors which should've been thrown
  if (ctx.valid_call_RES_IN_USE_1 && ctx.invalid_call_RES_IN_USE && ctx.valid_call_RES_IN_USE_2)
  {
    print_string("Incorrectly deleting partitions throws expected errors: true\n");
  }
  else
  {
    print_string("Incorrectly deleting partitions throws expected errors: false\n");
    (ctx.valid_call_RES_IN_USE_1) ? print_string("1st call - SUCCEEDED\n") : print_string("1st call - FAILED\n");
    (ctx.invalid_call_RES_IN_USE) ? print_string("2nd call - SUCCEEDED\n") : print_string("2nd call - FAILED\n");
    (ctx.valid_call_RES_IN_USE_2) ? print_string("3rd call - SUCCEEDED\n") : print_string("3rd call - FAILED\n");
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

  print_test_results();

  // Perform tidy up activities and exit application
  rtems_semaphore_delete(TASK_COUNTER_SEMAPHORE);
  rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, 0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS TEST_PROCESSORS

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

// Reduce the priority of the Init task so all other tasks can be schedules to all other cores
#define CONFIGURE_INIT_TASK_PRIORITY PRIO_LOW

#define CONFIGURE_MAXIMUM_PARTITIONS 1

#define CONFIGURE_MAXIMUM_BARRIERS 1

#define CONFIGURE_MAXIMUM_TASKS ((TEST_PROCESSORS) + 1)

#define CONFIGURE_SCHEDULER_EDF_SMP

#define CONFIGURE_MINIMUM_TASK_STACK_SIZE RTEMS_MINIMUM_STACK_SIZE + CPU_STACK_ALIGNMENT

#define CONFIGURE_EXTRA_TASK_STACKS RTEMS_MINIMUM_STACK_SIZE

#define CONFIGURE_IDLE_TASK_STORAGE_SIZE TASK_STORAGE_SIZE
#define CONFIGURE_INIT_TASK_CONSTRUCT_STORAGE_SIZE 2 * (TASK_STORAGE_SIZE)

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