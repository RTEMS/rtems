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
#include "../shared/isvv_rtems_aux.h"
#include "../shared/utils.h"
#include <string.h>

/**
- * @brief Assert RTEMS SMP under the stress of multiple core workloads accessing protected data.
- *
- * This test case performs the calculation of orbital frequencies based on readings from sensors
- * with three periodic tasks, and two binary semaphores. The result is stored as a reference set
- * and the elapsed time is measured.
*/

#define ITOA_STR_SIZE                           (8 * sizeof(int) + 1)

#define MAX_TLS_SIZE RTEMS_ALIGN_UP(64, RTEMS_TASK_STORAGE_ALIGNMENT)

#define TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define TASK_STORAGE_SIZE                                                      \
  RTEMS_TASK_STORAGE_SIZE(MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE,             \
			  TASK_ATTRIBUTES)

// test specific global vars
#undef TEST_PROCESSORS
#define TEST_PROCESSORS 2
#define TASK_COUNT 3

#define END_OF_STREAM UINT8_MAX

#define INCREMENT_AVAILABLE_DATA(x)             (x++)

#define DECREMENT_AVAILABLE_DATA(x)             ({if (x>0) x--;})

#define TEST_IF_DATA_AVAILABLE_IS_EMPTY(x)      (x==0)

#define TEST_IF_DATA_AVAILABLE_IS_NOT_EMPTY(x)  (x!=0)

#define EVENT_FINISHED_SENSING RTEMS_EVENT_0
#define EVENT_CALCULATED_DATA RTEMS_EVENT_1
#define EVENT_WROTE_DATA RTEMS_EVENT_2
#define EVENT_SENSOR_READY RTEMS_EVENT_3
#define EVENT_DATA_READY RTEMS_EVENT_4
#define EVENT_DATA_OUT RTEMS_EVENT_5

// For this data set, and given lumosity, the total number of planets found should equal 11

#define MAX_ITER                    (64U)
#define FFT_SIZE                    (32768U)
#define FFT_SIZE_LOG2               (15U)
#define TC_MAX                      (16U)
#define FILTER_REPETITIONS          (2U)
#define SNR_THRESHOLD               (1000000.0f)

const uint16_t FS                   = 48000U;
const uint16_t FREQ[TC_MAX]         = {   2500U,   1500U,   15000U,     500U,
                                          1000U,    800U,     440U,    8000U,
                                           100U,   3500U,   12345U,    1200U,
                                         20000U,    715U,    5000U,    4500U };
const float    NOISE_FACTOR[TC_MAX] = { 0.0001f, 0.2250f, 0.00068f,    0.30f,
                                         0.004f, 0.0123f,  0.0054f, 0.00054f,
                                          0.01f, 0.0325f,   0.012f, 0.00032f,
                                         0.075f, 0.0423f,  0.0354f, 0.00002f };

static float inSensorDataRe[FFT_SIZE];
static float inSensorDataIm[FFT_SIZE];
typedef struct
{
  rtems_id main_task_id;
  rtems_id read_task_id;
  rtems_id calc_task_id;
  rtems_id write_task_id;
  uint8_t m1_data;
  uint32_t iter;
  float scaling_fft_factor;
  float snr_float;
  rtems_id m1_data_mutex;
  rtems_id m2_result_mutex;
  uint8_t planet_count;
  uint8_t planet_locations[MAX_ITER];
  uint16_t value_in_watts_flag_counter;
  uint16_t luminosity_flag_counter;
  float debug_maxValueIndex;
  float debug_maxValue;
  float debug_sig;
  float debig_noise;
} test_context;

RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static char calc_task_storage[TASK_COUNT][TASK_STORAGE_SIZE];

// * Task 0 -  writes sensors data onto shared memory
static void task_snr_read_sensor_data(rtems_task_argument arg)
{
  test_context *ctx;
  ctx = (test_context *)arg;

  while ( ctx->iter < MAX_ITER)
  {
    ObtainMutex(ctx->m1_data_mutex);
    if TEST_IF_DATA_AVAILABLE_IS_EMPTY(ctx->value_in_watts_flag_counter)
    {
      for (uint32_t i = 0; i < FFT_SIZE; i++)
      {
        inSensorDataRe[i] = 0.4995f * cos_aprox(i*2.0f*PI*FREQ[ctx->iter%TC_MAX]/FS)
                                    + (noise_generator(0) * NOISE_FACTOR[ctx->iter%TC_MAX]);
        inSensorDataIm[i] = 0.4995f * sin_aprox(i*2.0f*PI*FREQ[ctx->iter%TC_MAX]/FS)
                                    + (noise_generator(0) * NOISE_FACTOR[ctx->iter% TC_MAX]);
      }
      INCREMENT_AVAILABLE_DATA(ctx->value_in_watts_flag_counter);
    }
    ReleaseMutex(ctx->m1_data_mutex);
    rtems_task_wake_after(1);
  }
  SuspendSelf();
}

// * Task 1 - collects data when ready, calculates, and stores the result on the shared memory
static void task_snr_process_data(rtems_task_argument arg)
{
  test_context *ctx;
  ctx = (test_context *)arg;
  float sig = 0.0f;
  float noise = 0.0f;
  bool update_luminosity_calc_result = false;

  while ( ctx->iter < MAX_ITER )
  {
    ObtainMutex(ctx->m1_data_mutex);
    if ( TEST_IF_DATA_AVAILABLE_IS_NOT_EMPTY(ctx->value_in_watts_flag_counter) && 
      (update_luminosity_calc_result == false) )
    {
      update_luminosity_calc_result = true;

      //Apply Blackman-Harris Window
      for (uint32_t i = 0; i < FFT_SIZE; i++) 
      {
        inSensorDataRe[i] = inSensorDataRe[i]*blackman_harris(i, FFT_SIZE)/ctx->scaling_fft_factor;
        inSensorDataIm[i] = inSensorDataIm[i]*blackman_harris(i, FFT_SIZE)/ctx->scaling_fft_factor;
      }
      //Calculates FFT
      fft(&inSensorDataRe[0], &inSensorDataIm[0], FFT_SIZE_LOG2);

      //Calculates the magnitude^2 values
      for (uint32_t i = 0; i < FFT_SIZE; i++)
      {
        inSensorDataRe[i] = (inSensorDataRe[i] * inSensorDataRe[i]
                              + inSensorDataIm[i] * inSensorDataIm[i]) / ((float)FFT_SIZE);
      }

      //Look for the peak Value and its index (no DC)
      float maxValue = 0.0;
      int32_t maxValueIndex = -1;
      for (uint32_t i = 3; i < (FFT_SIZE / 2); i++)
      {
        if (inSensorDataRe[i] > maxValue)
        {
          maxValue = inSensorDataRe[i];
          maxValueIndex = i;
        }
      }

      //Calculates the signal and noise power (no DC)
      sig = 0.0f;
      noise = 0.0f;
      for (uint32_t i = 3; i < (FFT_SIZE / 2); i++)
      {
        if ((i > maxValueIndex - 8) && (i < maxValueIndex + 8))
        {
          sig += (2 * inSensorDataRe[i]);
        }
        else
        {
          noise += (2 * inSensorDataRe[i]);
        }
      }
      ctx->debug_maxValueIndex = maxValueIndex;
      ctx->debug_maxValue = maxValue;
      ctx->debig_noise = noise;
      ctx->debug_sig = sig;
    }

    ReleaseMutex(ctx->m1_data_mutex);

    if (!(sig > 0.0f)) { sig = 0.0000000001f; }
    if (!(noise > 0.0f)) { noise = 0.0000000001f; }

    if (update_luminosity_calc_result)
    {
      ObtainMutex(ctx->m2_result_mutex);
      if ( TEST_IF_DATA_AVAILABLE_IS_EMPTY(ctx->luminosity_flag_counter) )
      {
        update_luminosity_calc_result = false;

        //Calculates SNR
        ctx->snr_float = sig/noise;
        DECREMENT_AVAILABLE_DATA(ctx->value_in_watts_flag_counter);
        INCREMENT_AVAILABLE_DATA(ctx->luminosity_flag_counter);
      }
      ReleaseMutex(ctx->m2_result_mutex);
    }
    rtems_task_wake_after(1);
  }
  SuspendSelf();
}

// * Task 2 - reads the results
static void read_and_output(rtems_task_argument arg)
{
  test_context *ctx = (test_context *)arg;
  float result;

  // Luminosity threshold to indicate planet, not e.g. star
  float luminosity_threshold = 0.005;

  while (ctx->iter < MAX_ITER)
  {
    ObtainMutex(ctx->m2_result_mutex);
    if (TEST_IF_DATA_AVAILABLE_IS_NOT_EMPTY(ctx->luminosity_flag_counter))
    {
      if (ctx->snr_float > SNR_THRESHOLD)
      {
        ctx->planet_locations[ctx->iter] = 1;
        ctx->planet_count++;
      }
      else 
      {
        ctx->planet_locations[ctx->iter] = 0;
      }
      DECREMENT_AVAILABLE_DATA(ctx->luminosity_flag_counter);
      ctx->iter++;
    }
    ReleaseMutex(ctx->m2_result_mutex);
    rtems_task_wake_after(1);
  }
  rtems_event_send(ctx->main_task_id, EVENT_WROTE_DATA);
  SuspendSelf();
}

static void Init(rtems_task_argument arg)
{
  (void)arg;
  test_context ctx;

  char str[ITOA_STR_SIZE];
  uint32_t start_time, end_time, multi_core_elapsed_time;

  rtems_id scsw_core_id;
  rtems_id aocs_core_id;

  (void) memset(&ctx, 0, sizeof(test_context));
  (void) memset(&inSensorDataRe[0], 0, FFT_SIZE);
  (void) memset(&inSensorDataIm[0], 0, FFT_SIZE);

  // Identify main task for communication between tasks
  ctx.main_task_id = rtems_task_self();
  SetSelfPriority( PRIO_NORMAL );

	//Calculates mean of Blackman-Harris terms
  ctx.scaling_fft_factor = 0.0;
  for (uint32_t i = 0; i<FFT_SIZE;  i++)
  {
    ctx.scaling_fft_factor += blackman_harris( i, FFT_SIZE);
  }
  ctx.scaling_fft_factor = ctx.scaling_fft_factor/ ((float) FFT_SIZE); 

  // Create the binary semaphores and intialize variables
  ctx.m1_data_mutex = CreateMutexMrsP(rtems_build_name('M', '1', 'M', 'X'));
  ctx.m2_result_mutex = CreateMutexMrsP(rtems_build_name('M', '2', 'M', 'X'));

  ctx.planet_count = 0;
  memset(&ctx.planet_locations, 0, MAX_ITER);

  rtems_task_config calc_task_config = {
      .initial_priority = PRIO_NORMAL,
      .storage_size = TASK_STORAGE_SIZE,
      .maximum_thread_local_storage_size =
          MAX_TLS_SIZE,
      .initial_modes = RTEMS_DEFAULT_MODES,
      .attributes = TASK_ATTRIBUTES};

  scsw_core_id = IdentifyScheduler(SCHEDULER_A_NAME);
  aocs_core_id = IdentifyScheduler(SCHEDULER_B_NAME);

  // Setup task for reading data from the sensor
  calc_task_config.name = rtems_build_name('R', 'E', 'A', 'D');
  calc_task_config.storage_area = &calc_task_storage[0][0];

  ctx.read_task_id = DoCreateTask(calc_task_config);
  SetScheduler(ctx.read_task_id, scsw_core_id, PRIO_LOW);

  // Setup task for reading from shared memory location
  // And calculating the new result in lumens
  calc_task_config.name = rtems_build_name('C', 'A', 'L', 'C');
  calc_task_config.storage_area = &calc_task_storage[1][0];

  ctx.calc_task_id = DoCreateTask(calc_task_config);
  SetScheduler(ctx.calc_task_id, aocs_core_id, PRIO_NORMAL);

  // Setup task for outputting from shared memory location
  calc_task_config.name = rtems_build_name('O', 'U', 'T', 'P');
  calc_task_config.storage_area = &calc_task_storage[2][0];

  ctx.write_task_id = DoCreateTask(calc_task_config);
  SetScheduler(ctx.write_task_id, scsw_core_id, PRIO_HIGH);

  // Start all tasks
  StartTask(ctx.read_task_id, task_snr_read_sensor_data, &ctx);
  StartTask(ctx.calc_task_id, task_snr_process_data, &ctx);
  StartTask(ctx.write_task_id, read_and_output, &ctx);
  
  start_time = rtems_clock_get_ticks_since_boot();
  
  // Wait for all tasks to be completed
  ReceiveAllEvents(EVENT_WROTE_DATA);
  
  end_time = rtems_clock_get_ticks_since_boot();

  // Output the results
  print_string("Planet count: ");
  print_string(itoa(ctx.planet_count, &str[0], 10));
  print_string("\n");
  print_string("Planets sensed at sensor positions: ");
  for (uint8_t i=0; i<MAX_ITER; i++)
  {
    if (ctx.planet_locations[i] > 0)
    {
      print_string(itoa(i, &str[0], 10));
      print_string(" ");
    }
  }

  multi_core_elapsed_time = end_time - start_time;
  print_string("\n");
  print_string("Multicore Elapsed Time - ");
  print_string(itoa(multi_core_elapsed_time, &str[0], 10));
  print_string("\n");

  rtems_task_delete(ctx.read_task_id);
  rtems_task_delete(ctx.calc_task_id);
  rtems_task_delete(ctx.write_task_id);

  rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, 0);
}


#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS TEST_PROCESSORS

#define CONFIGURE_MAXIMUM_SEMAPHORES 2

#define CONFIGURE_MAXIMUM_TASKS ( TASK_COUNT + 1 )

#define CONFIGURE_SCHEDULER_EDF_SMP

#define CONFIGURE_MINIMUM_TASK_STACK_SIZE                                      \
  RTEMS_MINIMUM_STACK_SIZE + CPU_STACK_ALIGNMENT

#define CONFIGURE_EXTRA_TASK_STACKS RTEMS_MINIMUM_STACK_SIZE

#define CONFIGURE_INIT_TASK_CONSTRUCT_STORAGE_SIZE 2 * TASK_STORAGE_SIZE

#define CONFIGURE_MINIMUM_TASKS_WITH_USER_PROVIDED_STORAGE                     \
  CONFIGURE_MAXIMUM_TASKS

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 0

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_EDF_SMP( a );
RTEMS_SCHEDULER_EDF_SMP( b );

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
    RTEMS_SCHEDULER_TABLE_EDF_SMP(a, SCHEDULER_A_NAME),	\
    RTEMS_SCHEDULER_TABLE_EDF_SMP(b, SCHEDULER_B_NAME)

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY)


#define CONFIGURE_SCHEDULER_NAME SCHEDULER_A_NAME

#define CONFIGURE_SCHEDULER_PRIORITY
	
	
#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE MAX_TLS_SIZE

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES (RTEMS_SYSTEM_TASK | TASK_ATTRIBUTES)

#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
