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
 * @brief Demonstrate that tasks running in parallel will not execute on separate cores of the
 * processor if the processor affinity is set to the same core for all tasks.
 *
 * This test case performs the calculation of the Mandelbrot set on 4 max cores, pinning to 0th core
 */

#define MAX_TLS_SIZE RTEMS_ALIGN_UP(64, RTEMS_TASK_STORAGE_ALIGNMENT)

#define TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define TASK_STORAGE_SIZE                      \
  RTEMS_TASK_STORAGE_SIZE(                     \
      MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE, \
      TASK_ATTRIBUTES)

#define ITOA_STR_SIZE (8 * sizeof(int) + 1)

// test specific global vars
#define TASK_COUNT ISVV_TEST_PROCESSORS

static rtems_id start_barrier;
static rtems_id TASK_COUNTER_SEMAPHORE;

typedef struct
{
  rtems_id task_id[TASK_COUNT];
  uint32_t processor[TASK_COUNT];
  uint8_t tile[TASK_COUNT];
} test_context;

test_context ctx;

/* create storage areas for each worker, using task construct forces
the user to create these manually*/
RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static char calc_task_storage[TASK_COUNT][TASK_STORAGE_SIZE];

static void calc_task_function(rtems_task_argument tile)
{
  uint8_t tile_n = (uint8_t)tile;

  /* Store tile and processor task is working on */
  ctx.processor[tile-1] = rtems_scheduler_get_processor();
  ctx.tile[tile-1] = tile_n;

  // for some reason this loop is needed for the scheduler to distribute tasks to all the cores
  Loop(4);

  WaitAtBarrier(start_barrier);

  mandelbrot_tile(tile_n, TASK_COUNT);

  // Task has finished so increment counter
  ReleaseCounterSemaphore(TASK_COUNTER_SEMAPHORE);
  // Internal call necessary to free resources for next task processing
  SuspendSelf();
}

static void Init(rtems_task_argument arg)
{
  (void)arg;
  uint32_t start_time, end_time, elapsed_time;
  char ch;
  char str[ITOA_STR_SIZE];
  cpu_set_t cpuset;
  uint8_t wait = 0;

  TASK_COUNTER_SEMAPHORE = CreateCounterSemaphore(rtems_build_name('T', 'C', 'S', '0'), 0);

  rtems_task_config calc_task_config = {
      .name = rtems_build_name('R', 'U', 'N', ' '),
      .initial_priority = 3,
      .storage_size = TASK_STORAGE_SIZE,
      .maximum_thread_local_storage_size = MAX_TLS_SIZE,
      .initial_modes = RTEMS_DEFAULT_MODES,
      .attributes = TASK_ATTRIBUTES};

  start_barrier = CreateAutomaticBarrier(TASK_COUNT);

  for (uint32_t i = 0; i < TASK_COUNT; i++)
  {
    ch = '0' + i;

    calc_task_config.name = rtems_build_name('R', 'U', 'N', ch);
    calc_task_config.storage_area = &calc_task_storage[i][0];

    ctx.task_id[i] = DoCreateTask(calc_task_config);

    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    rtems_task_set_affinity(ctx.task_id[i], sizeof(cpuset), &cpuset);

    StartTask(ctx.task_id[i], calc_task_function, (void *)(i + 1));
  }

  start_time = rtems_clock_get_ticks_since_boot();

  // Wait for all tasks to complete
  while (wait < TASK_COUNT)
  {
    ObtainCounterSemaphore(TASK_COUNTER_SEMAPHORE);
    wait++;
  }

  end_time = rtems_clock_get_ticks_since_boot();

  elapsed_time = end_time - start_time;

  for (uint32_t i = 0; i < TASK_COUNT; i++)
  {
    /* Identify tile and processor task worked on */
    print_string("Processor-");
    print_string(itoa(ctx.processor[i], &str[0], 10));
    print_string(",Tile-");
    print_string(itoa(ctx.tile[i], &str[0], 10));
    print_string("\n");
  }

  print_string("\n");
  print_string("Multicore Elapsed Time -");
  print_string(itoa(elapsed_time, &str[0], 10));
  print_string("\n");
  print_test_results();

  for (uint32_t i = 0; i < TASK_COUNT; i++)
  {
    DeleteTask(ctx.task_id[i]);
  }

  DeleteMutex(TASK_COUNTER_SEMAPHORE);
  rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, 0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS ISVV_TEST_PROCESSORS

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_MAXIMUM_BARRIERS 1

#define CONFIGURE_MAXIMUM_TASKS ( ISVV_TEST_PROCESSORS + 1 )

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

#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
