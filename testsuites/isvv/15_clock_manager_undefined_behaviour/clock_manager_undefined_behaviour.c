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
#include <string.h>

/**
 *
 * @brief Tests impact of undefined behaviours
 *
 * This test case performs the calculation of the Mandelbrot set in parallel with
 * execution of various clock-related functions
 */

#define MAX_TLS_SIZE RTEMS_ALIGN_UP(64, RTEMS_TASK_STORAGE_ALIGNMENT)

#define TASK_STORAGE_SIZE                      \
  RTEMS_TASK_STORAGE_SIZE(                     \
      MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE, \
      TASK_ATTRIBUTES)

#define TASK_ATTRIBUTES RTEMS_FLOATING_POINT

// test specific global vars
#define TASK_COUNT (TEST_PROCESSORS - 1)

#define ITOA_STR_SIZE (8 * sizeof(int) + 1)

// 1 (microsecond) is equal to (2^64 - 1)*10^-6 (64 bit fraction seconds).
// Using 7 significant figures (enough to preserve microsecond precision) this is 18446740000000.
#define FRAC_BINTIME_TO_USEC_CONV_FACTOR 18446740000000LLU

#define NUM_TEST_FUNCTIONS 15
#define RELATIVE_SLEEP 0
#define FUNCTION_WAIT_MICROSECONDS 100
#define FUNCTION_WAIT_NANOSECONDS (FUNCTION_WAIT_MICROSECONDS * 1000)
// Acceptable difference returned by subsequent clock-related function call
#define MARGIN_USEC 8

static rtems_id start_barrier;
static rtems_id TASK_COUNTER_SEMAPHORE;

typedef struct timespec timespec_t;
typedef struct bintime bintime_t;
typedef struct timeval timeval_t;

typedef enum
{
  TIMESPEC,
  BINTIME,
  TIMEVAL
} struct_type;

// Create storage areas for each worker, using task construct forces
RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static char calc_task_storage[TASK_COUNT][TASK_STORAGE_SIZE];

/* debug
RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static uint32_t task_processor[TASK_COUNT];
*/

RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static uint32_t task_before_mandelbrot[TASK_COUNT];

RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static uint32_t task_after_mandelbrot[TASK_COUNT];

/*
 * Find whether later time struct is within margin_usec of same type time struct earlier.
 */
static bool close_in_time(void *earlier, void *later, uint32_t margin_usec, struct_type type)
{
  bool ret = false;
  uint64_t earlier_total, later_total;

  /*
  char str[ITOA_STR_SIZE];
  */

  switch(type)
  {
    case TIMESPEC:
      earlier_total = (uint64_t)(((timespec_t *)earlier)->tv_sec * 1000000 + ((timespec_t *)earlier)->tv_nsec / 1000);
      later_total = (uint64_t)(((timespec_t *)later)->tv_sec * 1000000 + ((timespec_t *)later)->tv_nsec / 1000);
	  /* debug
	  print_string("timespec earlier: ");
	  print_string(itoa((int)earlier_total, &str[0], 10));
	  print_string("\n");
	  print_string("timespec later: ");
	  print_string(itoa((int)later_total, &str[0], 10));
	  print_string("\n");
	  */
      break;
    case BINTIME:
      earlier_total = (uint64_t)(((bintime_t *)earlier)->sec * 1000000 + ((bintime_t *)earlier)->frac / FRAC_BINTIME_TO_USEC_CONV_FACTOR);
      later_total = (uint64_t)(((bintime_t *)later)->sec * 1000000 + ((bintime_t *)later)->frac / FRAC_BINTIME_TO_USEC_CONV_FACTOR);
	  /* debug
	  print_string("bintime earlier: ");
	  print_string(itoa((int)earlier_total, &str[0], 10));
	  print_string("\n");
	  print_string("bintime later: ");
	  print_string(itoa((int)later_total, &str[0], 10));
	  print_string("\n");
	  */
      break;
    case TIMEVAL:
      earlier_total = (uint64_t)(((timeval_t *)earlier)->tv_sec * 1000000 + ((timeval_t *)earlier)->tv_usec);
      later_total = (uint64_t)(((timeval_t *)later)->tv_sec * 1000000 + ((timeval_t *)later)->tv_usec);
	  /* debug
	  print_string("timeval earlier: ");
	  print_string(itoa((int)earlier_total, &str[0], 10));
	  print_string("\n");
	  print_string("timeval later: ");
	  print_string(itoa((int)later_total, &str[0], 10));
	  print_string("\n");
	  */
      break;
    default:
      return ret;
  }

  if(earlier_total + margin_usec >= later_total)
    ret = true;

  return ret;
}

static void calc_task_function(rtems_task_argument arg)
{
  uint8_t tile = (uint8_t) arg;
  struct timespec uptime;

  WaitAtBarrier(start_barrier);

  rtems_clock_get_uptime(&uptime);
  task_before_mandelbrot[tile - 1] = (uint32_t) (uptime.tv_sec * 1000000 + uptime.tv_nsec/1000);
  
  mandelbrot_tile(tile, TASK_COUNT);

  rtems_clock_get_uptime(&uptime);
  task_after_mandelbrot[tile - 1] = (uint32_t) (uptime.tv_sec * 1000000 + uptime.tv_nsec/1000);
  
  // Task has finished so increment counter
  ReleaseCounterSemaphore(TASK_COUNTER_SEMAPHORE);
  SuspendSelf();
}

static void Init(rtems_task_argument arg)
{
  (void)arg;
  /* debug
  uint32_t main_task_processor;
  */
  uint32_t functions_start, functions_end;
  rtems_id task_id[TASK_COUNT];
  char ch;
  /* debug
  str[ITOA_STR_SIZE];
  */
  struct timespec uptime, function_wait;
  uint8_t wait = 0;
  uint8_t unaffected[NUM_TEST_FUNCTIONS];

  function_wait.tv_sec = 0;
  function_wait.tv_nsec = FUNCTION_WAIT_NANOSECONDS;
  memset(&unaffected, 0, NUM_TEST_FUNCTIONS);

  TASK_COUNTER_SEMAPHORE = CreateCounterSemaphore(rtems_build_name('T', 'C', 'S', '0'), 0);

  rtems_task_config calc_task_config = {
      .initial_priority = PRIO_VERY_HIGH,
      .storage_size = TASK_STORAGE_SIZE,
      .maximum_thread_local_storage_size = MAX_TLS_SIZE,
      .initial_modes = RTEMS_DEFAULT_MODES,
      .attributes = TASK_ATTRIBUTES};

  start_barrier = CreateAutomaticBarrier(TASK_COUNT + 1);

  for (uint32_t i = 0; i < TASK_COUNT; i++)
  {
    ch = '0' + i;

    calc_task_config.name = rtems_build_name('R', 'U', 'N', ch);
    calc_task_config.storage_area = &calc_task_storage[i][0];

    task_id[i] = DoCreateTask(calc_task_config);
    StartTask(task_id[i], calc_task_function, (void *)(i + 1));
  }

  /* 
   * Start execution of clock functions following formula of:
   * Defined behaviour
   * Undefined behaviour
   * Defined behaviour
   */

  timespec_t ts_first, ts_third;
  timespec_t *ts_null = NULL;

  bintime_t bt_first, bt_third;
  bintime_t *bt_null = NULL;

  timeval_t tv_first, tv_third;
  timeval_t *tv_null = NULL;

  WaitAtBarrier(start_barrier);

  /* debug
  main_task_processor = rtems_scheduler_get_processor();  
  */

  // Wait FUNCTION_WAIT_MICROSECONDS microseconds to ensure function calls are concurrent with mandelbrot calculation
  clock_nanosleep(CLOCK_MONOTONIC, RELATIVE_SLEEP, &function_wait, NULL);

  rtems_clock_get_uptime(&uptime);
  functions_start = (uint32_t) (uptime.tv_sec* 1000000 + uptime.tv_nsec/1000);

  /*
   * Check that BOOT_TIME subsequent call is not affected by undefined call.
   * Boot time should not change so if the third call does not match the first
   * an error has been found.
   */
  rtems_clock_get_boot_time(&ts_first);
  rtems_clock_get_boot_time_bintime(&bt_first);
  rtems_clock_get_boot_time_timeval(&tv_first);

  rtems_clock_get_boot_time(ts_null);
  rtems_clock_get_boot_time(&ts_third);
  if ((ts_third.tv_sec == ts_first.tv_sec) && (ts_third.tv_nsec == ts_first.tv_nsec))
	  unaffected[0] = true;

  rtems_clock_get_boot_time_bintime(bt_null);
  rtems_clock_get_boot_time_bintime(&bt_third);
  if ((bt_third.sec == bt_first.sec) && (bt_third.frac == bt_first.frac))
	  unaffected[1] = true;

  rtems_clock_get_boot_time_timeval(tv_null);
  rtems_clock_get_boot_time_timeval(&tv_third);
  if ((tv_third.tv_sec == tv_first.tv_sec) && (tv_third.tv_usec == tv_first.tv_usec))
	  unaffected[2] = true;

  /*
   * Check that CLOCK_MONOTONIC subsequent call is not affected by undefined call.
   * If third call is not close in time to the first call an error has been found.
   */
  rtems_clock_get_monotonic(&ts_first);
  rtems_clock_get_monotonic(ts_null);
  rtems_clock_get_monotonic(&ts_third);
  if(close_in_time(&ts_first, &ts_third, MARGIN_USEC, TIMESPEC))
	  unaffected[3] = true;

  /* debug
  print_string("clock_get_monotonic ts first tv_sec: ");
  print_string(itoa(ts_first.tv_sec, &str[0], 10));
  print_string("\n");
  print_string("clock_get_monotonic ts first tv_nsec: ");
  print_string(itoa(ts_first.tv_nsec, &str[0], 10));
  print_string("\n");
  print_string("clock_get_monotonic ts third tv_sec: ");
  print_string(itoa(ts_third.tv_sec, &str[0], 10));
  print_string("\n");
  print_string("clock_get_monotonic ts third tv_nsec: ");
  print_string(itoa(ts_third.tv_nsec, &str[0], 10));
  print_string("\n");
  */

  rtems_clock_get_monotonic_bintime(&bt_first);
  rtems_clock_get_monotonic_bintime(bt_null);
  rtems_clock_get_monotonic_bintime(&bt_third);
  if(close_in_time(&bt_first, &bt_third, MARGIN_USEC, BINTIME))
	  unaffected[4] = true;

  /* debug
  print_string("clock_get_monotonic bt first sec: ");
  print_string(itoa(bt_first.sec, &str[0], 10));
  print_string("\n");
  print_string("clock_get_monotonic bt first frac: ");
  print_string(itoa(bt_first.frac, &str[0], 10));
  print_string("\n");
  print_string("clock_get_monotonic bt third sec: ");
  print_string(itoa(bt_third.sec, &str[0], 10));
  print_string("\n");
  print_string("clock_get_monotonic bt third frac: ");
  print_string(itoa(bt_third.frac, &str[0], 10));
  print_string("\n");
  */

  rtems_clock_get_monotonic_timeval(&tv_first);
  rtems_clock_get_monotonic_timeval(tv_null);
  rtems_clock_get_monotonic_timeval(&tv_third);
  if(close_in_time(&tv_first, &tv_third, MARGIN_USEC, TIMEVAL))
	  unaffected[5] = true;

  /* debug
  print_string("clock_get_monotonic tv first tv_sec: ");
  print_string(itoa(tv_first.tv_sec, &str[0], 10));
  print_string("\n");
  print_string("clock_get_monotonic tv first tv_nsec: ");
  print_string(itoa(tv_first.tv_usec, &str[0], 10));
  print_string("\n");
  print_string("clock_get_monotonic tv third tv_sec: ");
  print_string(itoa(tv_third.tv_sec, &str[0], 10));
  print_string("\n");
  print_string("clock_get_monotonic tv third tv_nsec: ");
  print_string(itoa(tv_third.tv_usec, &str[0], 10));
  print_string("\n");
  */

  /*
   * Check that coarse CLOCK_MONOTONIC subsequent call is not affected by undefined call.
   * If third call is not close in time to the first call an error has been found.
   */
  rtems_clock_get_monotonic_coarse(&ts_first);
  rtems_clock_get_monotonic_coarse(ts_null);
  rtems_clock_get_monotonic_coarse(&ts_third);
  if(close_in_time(&ts_first, &ts_third, MARGIN_USEC, TIMESPEC))
	  unaffected[6] = true;

  rtems_clock_get_monotonic_coarse_bintime(&bt_first);
  rtems_clock_get_monotonic_coarse_bintime(bt_null);
  rtems_clock_get_monotonic_coarse_bintime(&bt_third);
  if(close_in_time(&bt_first, &bt_third, MARGIN_USEC, BINTIME))
	  unaffected[7] = true;

  rtems_clock_get_monotonic_coarse_timeval(&tv_first);
  rtems_clock_get_monotonic_coarse_timeval(tv_null);
  rtems_clock_get_monotonic_coarse_timeval(&tv_third);
  if(close_in_time(&tv_first, &tv_third, MARGIN_USEC, TIMEVAL))
	  unaffected[8] = true;

  /*
   * Check that CLOCK_REALTIME subsequent call is not affected by undefined call.
   * If third call is not close in time to the first call an error has been found.
   */
  rtems_clock_get_realtime(&ts_first);
  rtems_clock_get_realtime(ts_null);
  rtems_clock_get_realtime(&ts_third);
  if(close_in_time(&ts_first, &ts_third, MARGIN_USEC, TIMESPEC))
	  unaffected[9] = true;

  rtems_clock_get_realtime_bintime(&bt_first);
  rtems_clock_get_realtime_bintime(bt_null);
  rtems_clock_get_realtime_bintime(&bt_third);
  if(close_in_time(&bt_first, &bt_third, MARGIN_USEC, BINTIME))
	  unaffected[10] = true;

  rtems_clock_get_realtime_timeval(&tv_first);
  rtems_clock_get_realtime_timeval(tv_null);
  rtems_clock_get_realtime_timeval(&tv_third);
  if(close_in_time(&tv_first, &tv_third, MARGIN_USEC, TIMEVAL))
	  unaffected[11] = true;

  /*
   * Check that coarse CLOCK_REALTIME subsequent call is not affected by undefined call.
   * If third call is not close in time to the first call an error has been found.
   */
  rtems_clock_get_realtime_coarse(&ts_first);
  rtems_clock_get_realtime_coarse(ts_null);
  rtems_clock_get_realtime_coarse(&ts_third);
  if(close_in_time(&ts_first, &ts_third, MARGIN_USEC, TIMESPEC))
	  unaffected[12] = true;

  rtems_clock_get_realtime_coarse_bintime(&bt_first);
  rtems_clock_get_realtime_coarse_bintime(bt_null);
  rtems_clock_get_realtime_coarse_bintime(&bt_third);
  if(close_in_time(&bt_first, &bt_third, MARGIN_USEC, BINTIME))
	  unaffected[13] = true;

  rtems_clock_get_realtime_coarse_timeval(&tv_first);
  rtems_clock_get_realtime_coarse_timeval(tv_null);
  rtems_clock_get_realtime_coarse_timeval(&tv_third);
  if(close_in_time(&tv_first, &tv_third, MARGIN_USEC, TIMEVAL))
	  unaffected[14] = true;

  rtems_clock_get_uptime(&uptime);
  functions_end = (uint32_t) (uptime.tv_sec* 1000000 + uptime.tv_nsec/1000) ;

  // Wait for all tasks to complete
  while (wait < TASK_COUNT)
  {
    ObtainCounterSemaphore(TASK_COUNTER_SEMAPHORE);
    wait++;
  }

  // Ensure clock-based functions were executed at the same time as the mandlebrot set
  uint32_t latest_task_start = 0;
  uint32_t earliest_task_finish = -1;

  for(uint32_t task = 0; task < TASK_COUNT; task++)
  {
	  if(task_before_mandelbrot[task] > latest_task_start)
		  latest_task_start = task_before_mandelbrot[task];

	  if(task_after_mandelbrot[task] < earliest_task_finish)
		  earliest_task_finish = task_after_mandelbrot[task];
  }
  
  if(functions_start > latest_task_start && functions_end < earliest_task_finish)
  {
	  print_string("Functions and mandelbrot executed concurrently: true\n\n");
  }
  else
  {
	  print_string("Functions and mandelbrot executed concurrently: false\n\n");
  }

  if(unaffected[0] && unaffected[1] && unaffected[2])
  {
	  print_string("Boot time subsequent call unaffected: true\n");
  }
  else
  {
	  print_string("Boot time subsequent call unaffected: false\n");
  }

  if(unaffected[3] && unaffected[4] && unaffected[5])
  {
	  print_string("Clock monotonic subsequent call unaffected: true\n");
  }
  else
  {
	  print_string("Clock monotonic subsequent call unaffected: false\n");
  }

  if(unaffected[6] && unaffected[7] && unaffected[8])
  {
	  print_string("Coarse clock monotonic subsequent call unaffected: true\n");
  }
  else
  {
	  print_string("Coarse clock monotonic subsequent call unaffected: false\n");
  }

  if(unaffected[9] && unaffected[10] && unaffected[11])
  {
	  print_string("Clock realtime subsequent call unaffected: true\n");
  }
  else
  {
	  print_string("Clock realtime subsequent call unaffected: false\n");
  }

  if(unaffected[12] && unaffected[13] && unaffected[14])
  {
	  print_string("Coarse clock realtime subsequent call unaffected: true\n");
  }
  else
  {
	  print_string("Coarse clock realtime subsequent call unaffected: false\n");
  }

  print_test_results();
  /* debug
  print_string("Functions processor: ");
  print_string(itoa(main_task_processor, &str[0], 10));
  print_string("\n");
  print_string("Functions start time: ");
  print_string(itoa(functions_start, &str[0], 10));
  print_string("\n");
// --------------------------------------------------------------------------------------------
  print_string("\n\n\n");
  print_string("CheckPoints:");     
  print_string("\n|TILE |PROC |  START  |  END  |TIME SPENT| (us)");
  print_string("\n");
  for (uint32_t task = 0; task < TASK_COUNT; task++)
  {
	print_string("|  ");
	print_string(itoa(task+1, &str[0], 10));        
	print_string("  |  ");       
	print_string(itoa(task_processor[task], &str[0], 10));        
	print_string("  |  ");        
	print_string(itoa(task_before_mandelbrot[task], &str[0], 10));
	print_string("  |  ");
	print_string(itoa(task_after_mandelbrot[task], &str[0], 10));
	print_string("  | ");
	print_string(itoa(task_after_mandelbrot[task]-task_before_mandelbrot[task], &str[0], 10));
	print_string(" |\n");
  }
  print_string("\n\nTICKS Per second: ");                
  print_string(itoa(rtems_clock_get_ticks_per_second(), &str[0], 10));
  print_string("\n\n");
// --------------------------------------------------------------------------------------------
  print_string("Functions end time: ");
  print_string(itoa(functions_end, &str[0], 10));
  print_string("\n");
      
  for(uint8_t i = 0; i < NUM_TEST_FUNCTIONS; i++)
  {
	  print_string("unaffected ");
	  print_string(itoa(i, &str[0], 10));
	  print_string(": ");
	  print_string(itoa(unaffected[i], &str[0], 10));
	  print_string("\n");
  }
  */

  for (uint32_t i = 0; i < TASK_COUNT; i++)
  {
    DeleteTask(task_id[i]);
  }

  DeleteMutex(TASK_COUNTER_SEMAPHORE);
  DeleteBarrier(start_barrier);

  rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, 0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS TEST_PROCESSORS

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_MAXIMUM_BARRIERS 1

#define CONFIGURE_MAXIMUM_TASKS (TEST_PROCESSORS + 1)

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
