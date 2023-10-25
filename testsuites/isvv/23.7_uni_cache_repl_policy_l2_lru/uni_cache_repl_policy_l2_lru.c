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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/bspIo.h>
#include <string.h>
#include "../shared/utils.h"
#include "../shared/isvv_rtems_aux.h"
#include "../shared/low_level_utils.h"


/**
 *
 * @brief Tests impact performing of L2 cache replacement policy
 *
 * Step 1: Uniprocessor case
 *
 * In this step the same set of math equations used by several tasks in the
 * Multiprocessor version are run in an Uniprocessor and ONE task only environment
 * in order to achieve a reference output result for comparison.
 *
 * There are two main sets of math equations processed here:
 *  - one named "filter_simulation"
 *  - and other named "snr_processing"
 *
 * For each one, result values are obtained for later comparison with multiprocessor
 * versions. Also some internal cache statistics are shown. Most relevant one is the number of
 * L2 cache misses, which when increased significantly, may delay the data processing.
 *
 */

/**
 *
 * For standalone tests in the actual hardware boards the following options can be used:
 *
 *  1) use make XFLAGS="-Dgr740 -DGR740_ESA_BOARD"
 *  2) declare #define GR740_ESA_BOARD at the beginning of this file
 *
 */

#define MAX_TLS_SIZE RTEMS_ALIGN_UP(64, RTEMS_TASK_STORAGE_ALIGNMENT)

#define TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define TASK_STORAGE_SIZE                      \
  RTEMS_TASK_STORAGE_SIZE(                     \
      MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE, \
      TASK_ATTRIBUTES)


// test specific global vars
#undef TEST_PROCESSORS
#define TEST_PROCESSORS 1

#define TASK_COUNT TEST_PROCESSORS
#define TOTAL_TILES           128

#define MAX_MESSAGE_SIZE sizeof(uint8_t)
#define MAX_PENDING_MESSAGES 10

const rtems_event_set event_send[6] = {RTEMS_EVENT_1,
                                       RTEMS_EVENT_2,
                                       RTEMS_EVENT_3,
                                       RTEMS_EVENT_4,
                                       RTEMS_EVENT_5,
                                       RTEMS_EVENT_6
                                      };

uint8_t count_process[TOTAL_TILES];

typedef struct{
  uint8_t ntiles;
  uint8_t next_tile;
  float filter_sim_process_time;
  uint64_t accxL2;
  float snr_process_time;
  float scaling_fft_factor;
} test_context;

#define ITOA_STR_SIZE     (8*sizeof(int)+1)

//-------------------------------------------------------------------------------
#define L2_CACHE_SIZE     (4U*512U*1024U)                      // 2M bytes
#define L2_CACHE_WAY_SIZE (512U*1024U)                         // 512k bytes
#define xL2_ELEM          (4*L2_CACHE_SIZE/sizeof(uint32_t))   // 2M elements

#ifdef GR740_ESA_BOARD
RTEMS_ALIGNED(L2_CACHE_SIZE)
#endif
static uint32_t xL2[xL2_ELEM];

#define COEFS_SIZE        (128U)
const uint32_t coefs[COEFS_SIZE] =
                            { 29,  31,  37,  41,  43,  47,  53,  59,
                              61,  67,  71,  73,  79,  83,  89,  97,
                             101, 103, 107, 109, 113, 127, 131, 137,
                             139, 149, 151, 157, 163, 167, 173, 179,
                             181, 191, 193, 197, 199, 211, 223, 227,
                             229, 233, 239, 241, 251, 257, 263, 269,
                             271, 277, 281, 283, 293, 307, 311, 313,
                             317, 331, 337, 347, 349, 353, 359, 367,
                             373, 379, 383, 389, 397, 401, 409, 419,
                             421, 431, 433, 439, 443, 449, 457, 461,
                             463, 467, 479, 487, 491, 499, 503, 509,
                             521, 523, 541, 547, 557, 563, 569, 571,
                             577, 587, 593, 599, 601, 607, 613, 617,
                             619, 631, 641, 643, 647, 653, 659, 661,
                             673, 677, 683, 691, 701, 709, 719, 727,
                             733, 739, 743, 751, 757, 761, 769, 773};

//-------------------------------------------------------------------------------
#define SENSOR_DATA_ELEM            (6*L2_CACHE_WAY_SIZE/sizeof(uint32_t))
#define MAX_ITER                    (SENSOR_DATA_ELEM / FFT_SIZE)
#define FFT_SIZE                    (8192U)
#define FFT_SIZE_LOG2               (13U)
#define TC_MAX                      (16U)
#define FILTER_TAPS                 (64U)
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
#ifdef GR740_ESA_BOARD
RTEMS_ALIGNED(L2_CACHE_WAY_SIZE)
#endif
static float inSensorDataRe[SENSOR_DATA_ELEM];

#ifdef GR740_ESA_BOARD
RTEMS_ALIGNED(L2_CACHE_WAY_SIZE)
#endif
static float inSensorDataIm[SENSOR_DATA_ELEM];
static uint8_t  dsp_result[MAX_ITER];


//=======================================================================================
// Auxiliary Functions
//=======================================================================================
static void fill_main_memory_with_data(void){
  // Store to memory
  for ( uint32_t j = 0 ; j < xL2_ELEM; j ++)
    xL2[j] = j;
}

static uint64_t warmup_caches(void){
  uint64_t acc = 0;
  for ( uint32_t j = 0 ; j < xL2_ELEM; j++)
    acc += xL2[j];
  return acc;
}


//=======================================================================================
// "filter_simulation" Tasks/Functions
//=======================================================================================
static uint64_t calc_filter_simulation_equation(uint8_t tile, uint32_t total_elems) {
  uint64_t acc = 0;
  const uint32_t begin_idx = tile*xL2_ELEM/total_elems;
  const uint32_t end_idx   = begin_idx + (xL2_ELEM/total_elems) - 1;

  for ( uint32_t j = begin_idx ; j <= end_idx; j++ ) {
    uint32_t i;

    // Simulating filtering/convolution
    uint32_t t1 = 0, t2 = 0, t3 = 0, t4 = 0;
    for (i = 0 ; i < FILTER_TAPS; i++)
      t1 += xL2[(j+i) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < FILTER_TAPS; i++)
      t1 += xL2[(j+i+L2_CACHE_WAY_SIZE*1/2/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < FILTER_TAPS; i++)
      t1 += xL2[(j+i+L2_CACHE_WAY_SIZE*2/2/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < FILTER_TAPS; i++)
      t1 += xL2[(j+i+L2_CACHE_WAY_SIZE*3/2/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < FILTER_TAPS; i++)
      t1 += xL2[(j+i+L2_CACHE_WAY_SIZE*4/2/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < FILTER_TAPS; i++)
      t1 += xL2[(j+i+L2_CACHE_WAY_SIZE*5/2/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < FILTER_TAPS; i++)
      t1 += xL2[(j+i+L2_CACHE_WAY_SIZE*6/2/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < FILTER_TAPS; i++)
      t1 += xL2[(j+i+L2_CACHE_WAY_SIZE*7/2/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    // Simulating normalization
    t3 = ((t1*1000)/(42*137)) + 1;
    t4 = ((t2*10)/(96*137)) + 1;
    uint64_t t5 = (uint64_t)t3 * (uint64_t)t3;
    uint64_t t6 = (uint64_t)t4 * (uint64_t)t4;
    acc += (t5+t6)/((uint64_t)isqrt(t4+t3));
  }
  return acc;
}

static uint64_t function_calc_filter_simulation(test_context *ctx, uint8_t tile, uint32_t total_elems) {
  struct timespec begin_time;
  struct timespec end_time;
  static uint64_t acc;

  rtems_clock_get_uptime(&begin_time);

  acc = calc_filter_simulation_equation(tile, total_elems);

  rtems_clock_get_uptime(&end_time);
  ctx->filter_sim_process_time +=
        (float)(end_time.tv_sec - begin_time.tv_sec) +
        ( (float)((end_time.tv_nsec/1000) - (begin_time.tv_nsec/1000))/1000000.0);

  return acc;
}


//=======================================================================================
// "snr_processing" Tasks/Functions
//=======================================================================================
static void function_snr_read_sensor_data( uint32_t start_idx, uint32_t iter) {
    for (uint32_t i = 0; i < FFT_SIZE; i++) {
      inSensorDataRe[start_idx + i] = 0.4995f * cos_aprox(i*2.0f*PI*FREQ[iter%TC_MAX]/FS)
                                              + (noise_generator(0) * NOISE_FACTOR[iter%TC_MAX]);
      inSensorDataIm[start_idx + i] = 0.4995f * sin_aprox(i*2.0f*PI*FREQ[iter%TC_MAX]/FS)
                                              + (noise_generator(0) * NOISE_FACTOR[iter % TC_MAX]);
    }
}

static void function_snr_process_data(test_context *ctx, uint32_t start_idx, uint32_t iter) {
    struct timespec begin_time;
    struct timespec end_time;

    rtems_clock_get_uptime(&begin_time);

    //Apply Blackman-Harris Window
    for (uint32_t i = 0; i < FFT_SIZE; i++) {
      inSensorDataRe[i] = inSensorDataRe[start_idx+i]*blackman_harris(i, FFT_SIZE)/ctx->scaling_fft_factor;
      inSensorDataIm[i] = inSensorDataIm[start_idx+i]*blackman_harris(i, FFT_SIZE)/ctx->scaling_fft_factor;
    }

    //Calculates FFT
    fft(&inSensorDataRe[0], &inSensorDataIm[0], FFT_SIZE_LOG2);

    //Calculates the magnitude values
    for (uint32_t i = 0; i < FFT_SIZE; i++) {
      inSensorDataRe[i] = (inSensorDataRe[i] * inSensorDataRe[i]
                             + inSensorDataIm[i] * inSensorDataIm[i]) / ((float)FFT_SIZE);
    }

    //Look for the peak Value and its index (no DC)
    float maxValue = 0.0;
    float maxValueIndex = -1;
    for (uint32_t i = 3; i < (FFT_SIZE / 2); i++) {
      if (inSensorDataRe[i] > maxValue) {
        maxValue = inSensorDataRe[i];
        maxValueIndex = i;
      }
    }

    //Calculates the signal and noise power (no DC)
    float sig = 0.0f;
    float noise = 0.0f;
    for (uint32_t i = 3; i < (FFT_SIZE / 2); i++) {
      if ((i > maxValueIndex - 4) && (i < maxValueIndex + 4)) {
        sig += (2 * inSensorDataRe[i]);
      }
      else {
        noise += (2 * inSensorDataRe[i]);
      }
    }

    if (!(sig > 0.0f)) { sig = 0.0000000001f; }
    if (!(noise > 0.0f)) { noise = 0.0000000001f; }

    //Calculates SNR
    float snr_float = sig/noise;

    if (snr_float > SNR_THRESHOLD) {
      dsp_result[iter] = 1;
    }
    else {
      dsp_result[iter] = 0;
    }
    rtems_clock_get_uptime(&end_time);

    ctx->snr_process_time +=
        (float)(end_time.tv_sec - begin_time.tv_sec) +
        ( (float)((end_time.tv_nsec/1000) - (begin_time.tv_nsec/1000))/1000000.0);
}


//=======================================================================================
// INIT/MAIN Task
//=======================================================================================
static void Init(rtems_task_argument arg){
  (void)arg;
  test_context ctx;
  uint32_t start_time, end_time, elapsed_time;
  char str[ITOA_STR_SIZE];
  bool correctly_processed = true;
  (void) memset(&ctx, 0, sizeof(test_context));
  (void) memset(&count_process[0], 0, TOTAL_TILES);
  (void) memset(&xL2[0], 0, xL2_ELEM);
  (void) memset(&inSensorDataRe[0], 0, SENSOR_DATA_ELEM);
  (void) memset(&inSensorDataIm[0], 0, SENSOR_DATA_ELEM);

#ifdef GR740_ESA_BOARD
  soc_stats_regs soc_stats;
#endif

//-----------------------------------------------------------------------------
// Create/Initialize Objects
//-----------------------------------------------------------------------------
  ctx.ntiles = TOTAL_TILES;
  ctx.next_tile = 1;
  SetSelfPriority( PRIO_NORMAL );


//-----------------------------------------------------------------------------
// Setup the testcase
//-----------------------------------------------------------------------------
  fill_main_memory_with_data();
  l1_dcache_flush();
  l1_dcache_disable();

#ifdef GR740_ESA_BOARD
  l2_cache_disable();
  l2_cache_flush();
  l2_cache_set_replacement_policy(L2_CACHE_REPL_LRU, 0);
  l2_cache_enable();
#endif

  l1_dcache_enable();
  uint32_t control_data_word = warmup_caches();

#ifdef GR740_ESA_BOARD
  clockgating_enable_l4stat();
  soc_stats_configure_regs();
  soc_stats_init(&soc_stats);
#endif

	//Calculates mean of Blackman-Harris terms 
  ctx.scaling_fft_factor = 0.0;
  for (uint32_t i = 0; i<FFT_SIZE;  i++){
    ctx.scaling_fft_factor += blackman_harris( i, FFT_SIZE);
  }
  ctx.scaling_fft_factor = ctx.scaling_fft_factor/ ((float) FFT_SIZE);

//-----------------------------------------------------------------------------
// Do the work: distribute the work through the tasks
//-----------------------------------------------------------------------------
  start_time = rtems_clock_get_ticks_since_boot();
  ctx.accxL2 = function_calc_filter_simulation(&ctx, 0, 1);

  uint32_t start_idx = 0;
  for (uint32_t iter=0; iter<MAX_ITER; iter++, start_idx += FFT_SIZE) {
    function_snr_read_sensor_data(start_idx, iter);
    function_snr_process_data(&ctx, start_idx, iter) ;
  }

  end_time = rtems_clock_get_ticks_since_boot();
  elapsed_time = end_time - start_time;

#ifdef GR740_ESA_BOARD
  soc_stats_update(&soc_stats);
#endif

//-----------------------------------------------------------------------------
// Show Results
//-----------------------------------------------------------------------------
  print_string("\n");
  print_string("Single Core Elapsed Time -");
  print_string(itoa(elapsed_time, &str[0], 10));
  print_string("\n");

  for (uint8_t i = 0; i < ctx.ntiles; i++){
    if (count_process[i] != 1){
      correctly_processed = false;
      break;
    }
  }

  if (correctly_processed){
    print_string("Each tile only processed once             : true\n");
  }
  else{
    print_string("Each tile only processed once             : false\n");
  }

  print_string("Input Data Result Value                   : 0x");
  print_string(itoa(control_data_word , &str[0], 16));
  print_string("\n");

  print_string("Ouput Data Result Value                   : 0x");
  if (ctx.accxL2>=UINT32_MAX) {
    print_string(itoa( (int32_t) ((ctx.accxL2 >> 32U) & ((uint64_t)UINT32_MAX)) , &str[0], 16));
    print_string(itoa( (int32_t) (ctx.accxL2          & ((uint64_t)UINT32_MAX)) , &str[0], 16));
  }
  else {
    print_string(itoa((int32_t)ctx.accxL2  , &str[0], 16));
  }
  print_string("\n"); 
  print_string("Time used in filter sim processing        : "); 
  print_string(itoa( (int32_t) (ctx.filter_sim_process_time *1000) , &str[0], 10));  
  print_string(" ms\n"); 

  print_string("Time used in SNR processing               : "); 
  print_string(itoa( (int32_t) (ctx.snr_process_time *1000) , &str[0], 10));  
  print_string(" ms\n"); 

  print_string("SNR triggers found at : ");
  for (uint32_t j = 0; j < MAX_ITER; j++)
      if (dsp_result[j] > 0.0){
        print_string(itoa(j, &str[0], 10));
        print_string(" ");
      }
  print_string("\n");


#ifdef GR740_ESA_BOARD
  print_string("L1 Instr Cache misses (read) CPU_0        : ");
  print_string(itoa(soc_stats.l1_inst_cache_miss[0], &str[0], 10));
  print_string("\n");
  print_string("L1 Instr Cache misses (read) CPU_1        : ");
  print_string(itoa(soc_stats.l1_inst_cache_miss[1], &str[0], 10));
  print_string("\n");
  print_string("L1 Instr Cache misses (read) CPU_2        : ");
  print_string(itoa(soc_stats.l1_inst_cache_miss[2], &str[0], 10));
  print_string("\n");
  print_string("L1 Instr Cache misses (read) CPU_3        : ");
  print_string(itoa(soc_stats.l1_inst_cache_miss[3], &str[0], 10));
  print_string("\n");
  print_string("L1 Data Cache misses (read) CPU_0         : ");
  print_string(itoa(soc_stats.l1_data_cache_miss[0], &str[0], 10));
  print_string("\n");
  print_string("L1 Data Cache misses (read) CPU_1         : ");
  print_string(itoa(soc_stats.l1_data_cache_miss[1], &str[0], 10));
  print_string("\n");
  print_string("L1 Data Cache misses (read) CPU_2         : ");
  print_string(itoa(soc_stats.l1_data_cache_miss[2], &str[0], 10));
  print_string("\n");
  print_string("L1 Data Cache misses (read) CPU_3         : ");
  print_string(itoa(soc_stats.l1_data_cache_miss[3], &str[0], 10));
  print_string("\n");
  print_string("L2 Cache hits  (read + writes)            : ");
  print_string(itoa(soc_stats.l2_cache_hits, &str[0], 10));
  print_string("\n");
  print_string("L2 Cache misses (read + writes)           : ");
  print_string(itoa(soc_stats.l2_cache_miss, &str[0], 10));
  print_string("\n");
  print_string("AHB Splits                                : ");
  print_string(itoa(soc_stats.ahb_split_delay, &str[0], 10));
  print_string("\n");
  print_string("\n");
#endif


// --------------------------------------------------------------------------
// Delete Objects and Finalize testcase
// --------------------------------------------------------------------------

  rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, 0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS TEST_PROCESSORS

#define CONFIGURE_MAXIMUM_TASKS TEST_PROCESSORS

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
