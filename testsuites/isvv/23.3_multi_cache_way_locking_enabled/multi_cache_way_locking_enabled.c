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
 * @brief Tests impact performing of L2 cache way locking
 *
 * Step 3: Multiprocessor with L2 cache way locking enable
 *
 * There are two main math set of equations used here:
 *  - one named "filter_simulation"
 *  - and other named "snr_processing"
 *
 * The equations for "filter_simulation" are used by TWO tasks that processes small
 * sections of a "data array" in parallel.
 * The equations for "snr_processing" are used by TWO tasks that processes small
 * sections of other "data array" in sequential order, but in parallel with the other
 * tasks.
 *
 * The locations in memory for both "data arrays" do overlap in terms of cache memory
 * positions, and when acessing one "data array", or part of one, other sections of
 * the other "data array" should be kicked out of the cache. Sometimes that should
 * also happen within the same "data array".
 *
 * With the "L2 cache way locking" enabled for 2 of the 4 cache ways, the data array
 * associated with the "snr_processing" is never kicked out the L2 cache. But in other
 * hand but there is less ways available for the "filter_simulation" processing, and
 * its stumbles a lot during the processing of its "data array".
 *
 * Expected Results:
 *   - The Tiles must be processed only once.
 *   - "Ouput Data Result Value" must match with the Uniprocessor version
 *      and Multiprocessor version with L2 cache way locking disable.
 *   - SNR triggers index positions must match with the ones from the Multiprocessor
 *      version with L2 cache way locking disable version and with the Uniprocessor
 *      version.
 *   - Time spent for "snr_processing" should be slightly lower than the Multiprocessor
 *      version with L2 cache way locking disable (due to the L2 cache way locking 
 *      enabled for the arrays associated to way#3 and way#4).
 *
 */

/**
 *
 * For standalone tests in the actual hardware boards the following options can be used:
 *
 *  1) use make XFLAGS="-Dgr740 -DGR740_ESA_BOARD"
 *  2) use and declare #define GR740_ESA_BOARD at the beginning of this file
 *
 */


#define MAX_TLS_SIZE RTEMS_ALIGN_UP(64, RTEMS_TASK_STORAGE_ALIGNMENT)

#define TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define TASK_STORAGE_SIZE                      \
  RTEMS_TASK_STORAGE_SIZE(                     \
      MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE, \
      TASK_ATTRIBUTES)


// test specific global vars
#define TASK_COUNT            (2+2)  //2 parallel tasks for "filter simulation" and
                                     // another 2 sequential tasks for "snr processing"
#define TOTAL_TILES           192

#define MAX_MESSAGE_QUEUES    6
#define MAX_MESSAGE_SIZE      sizeof(uint8_t)
#define MAX_PENDING_MESSAGES  10

const rtems_event_set event_send[6] = {RTEMS_EVENT_1,
                                       RTEMS_EVENT_2,
                                       RTEMS_EVENT_3,
                                       RTEMS_EVENT_4,
                                       RTEMS_EVENT_5,
                                       RTEMS_EVENT_6
                                      };

#define EVENT_GO_READ_SENSOR      RTEMS_EVENT_10
#define EVENT_DATA_READY          RTEMS_EVENT_11
#define EVENT_DATA_WAS_PROCESSED  RTEMS_EVENT_12

uint8_t count_process[TOTAL_TILES];

typedef struct{
  rtems_id main_task;
  uint8_t ntiles;
  uint8_t next_tile;
  rtems_id task_filter_simulation_id[TEST_PROCESSORS];
  rtems_id tile_queue;
  rtems_id message_queue[TEST_PROCESSORS];
  rtems_id filter_mutex_id;
  float filter_sim_process_time;
  uint64_t accxL2;
  rtems_id task_snr_process_data_id;
  rtems_id task_snr_read_sensor_data_id;
  rtems_id snr_mutex_id;
  float snr_process_time;
  float scaling_fft_factor;
} test_context;

RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static char calc_task_storage[TASK_COUNT][TASK_STORAGE_SIZE];

RTEMS_MESSAGE_QUEUE_BUFFER(MAX_MESSAGE_SIZE)
msg_tile_queue_storage[MAX_PENDING_MESSAGES];

RTEMS_MESSAGE_QUEUE_BUFFER(MAX_MESSAGE_SIZE)
msg_task_queue_storage[TASK_COUNT][MAX_PENDING_MESSAGES];

#define ITOA_STR_SIZE     (8*sizeof(int)+1)

//-------------------------------------------------------------------------------
#define L2_CACHE_SIZE     (4U*512U*1024U)                      // 2M bytes
#define L2_CACHE_WAY_SIZE (512U*1024U)                         // 512k bytes
#define xL2_ELEM          (2*L2_CACHE_SIZE/sizeof(uint32_t))   // 1M elements

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
#define MAX_ITER                    (TOTAL_TILES*1/3)
#define FFT_SIZE                    (65536U)
#define FFT_SIZE_LOG2               (16U)
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
#ifdef GR740_ESA_BOARD
RTEMS_ALIGNED(L2_CACHE_WAY_SIZE)
#endif
static float inSensorDataRe[FFT_SIZE];

#ifdef GR740_ESA_BOARD
RTEMS_ALIGNED(L2_CACHE_WAY_SIZE)
#endif
static float inSensorDataIm[FFT_SIZE];
static uint8_t  dsp_result[MAX_ITER];


//=======================================================================================
// Auxiliary /Functions
//=======================================================================================
static void fill_main_memory_with_data(void){
  // Store to memory
  for ( uint32_t j = 0 ; j < xL2_ELEM; j ++)
    xL2[j] = j;
}

static uint64_t warmup_caches(void) {
  uint32_t j;
  uint64_t volatile acc = 0;

  // This data should be cached in way #1
  for ( j = 0 ; j < FFT_SIZE; j++)
    acc += xL2[j];

  // This data should be cached in way #2
  for ( j = 0 ; j < FFT_SIZE; j++)
    acc += xL2[j+(L2_CACHE_WAY_SIZE/sizeof(uint32_t))];

  // This data should be cached in way #3
  for ( j = 0 ; j < FFT_SIZE; j++)
    acc += inSensorDataRe[j];

  // This data should be cached in way #4
  for ( j = 0 ; j < FFT_SIZE; j++)
    acc += inSensorDataIm[j];

  return acc;
}


//=======================================================================================
// "filter_simulation" Tasks/Functions
//=======================================================================================
static uint64_t calc_filter_simulation_equation(uint8_t tile, uint32_t total_elems) {
  uint64_t acc = 0;
  const uint32_t begin_idx = tile*xL2_ELEM/total_elems;
  const uint32_t end_idx   = begin_idx + (xL2_ELEM/total_elems) - 1;

  for ( uint32_t j = 0 ; j <= FILTER_REPETITIONS; j++ ) {
    uint32_t i;

    // Simulating filtering/convolution
    uint32_t t1 = 0, t2 = 0, t3 = 0, t4 = 0;
    for (i = 0 ; i < FFT_SIZE; i++) {
      t1 += xL2[(j+i+0*(L2_CACHE_WAY_SIZE/sizeof(uint32_t))) % xL2_ELEM] * coefs[i % COEFS_SIZE];
      t1 += xL2[(j+i+1*(L2_CACHE_WAY_SIZE/sizeof(uint32_t))) % xL2_ELEM] * coefs[i % COEFS_SIZE];
      t1 += xL2[(j+i+2*(L2_CACHE_WAY_SIZE/sizeof(uint32_t))) % xL2_ELEM] * coefs[i % COEFS_SIZE];
      t1 += xL2[(j+i+3*(L2_CACHE_WAY_SIZE/sizeof(uint32_t))) % xL2_ELEM] * coefs[i % COEFS_SIZE];
      t1 += xL2[(j+i+4*(L2_CACHE_WAY_SIZE/sizeof(uint32_t))) % xL2_ELEM] * coefs[i % COEFS_SIZE];
      t1 += xL2[(j+i+5*(L2_CACHE_WAY_SIZE/sizeof(uint32_t))) % xL2_ELEM] * coefs[i % COEFS_SIZE];
      t1 += xL2[(j+i+6*(L2_CACHE_WAY_SIZE/sizeof(uint32_t))) % xL2_ELEM] * coefs[i % COEFS_SIZE];
      t1 += xL2[(j+i+7*(L2_CACHE_WAY_SIZE/sizeof(uint32_t))) % xL2_ELEM] * coefs[i % COEFS_SIZE];
    }

    // Simulating normalization
    t3 = ((t1*100)/(42*137)) + xL2[(begin_idx) % xL2_ELEM] ;
    t4 = ((t2*10)/(96*137)) + xL2[(end_idx) % xL2_ELEM] ;
    acc += (t3+t4);
  }
  return acc;
}


static void calc_task_function(rtems_task_argument arg) {
  test_context *ctx;
  uint64_t acc;
  struct timespec begin_time;
  struct timespec end_time;
  uint8_t tile = 0;
  uint8_t task_idx = 255;

  ctx = (test_context *)arg;
  rtems_id local_id = TaskSelfId();

  for (int i = 0; i < TASK_COUNT; i++){
    if (ctx->task_filter_simulation_id[i] == local_id){
      task_idx = i;
      break;
    }
  }

  char ch = '0' + task_idx;

  rtems_message_queue_config msg_config = {
      .name = rtems_build_name('M', 'S', 'G', ch),
      .maximum_pending_messages = MAX_PENDING_MESSAGES,
      .maximum_message_size = MAX_MESSAGE_SIZE,
      .storage_size = sizeof(msg_task_queue_storage[task_idx]),
      .storage_area = &msg_task_queue_storage[task_idx],
      .attributes = RTEMS_FIFO | RTEMS_GLOBAL};

  ctx->message_queue[task_idx] = CreateMessageQueue(msg_config);

  SendMessage(ctx->tile_queue, &task_idx, sizeof(task_idx));
  ReceiveMessage(ctx->message_queue[task_idx], &tile);

  while (tile <= ctx->ntiles) {
    count_process[tile - 1]++;
    rtems_clock_get_uptime(&begin_time);
    acc = calc_filter_simulation_equation(tile-1, ctx->ntiles);
    rtems_clock_get_uptime(&end_time);

    ObtainMutex(ctx->filter_mutex_id);
    ctx->filter_sim_process_time += (float)(end_time.tv_sec - begin_time.tv_sec)
                    +  ( (float)(end_time.tv_nsec/1000 - begin_time.tv_nsec/1000)/1000000.0);
    ctx->accxL2 += acc;
    ReleaseMutex(ctx->filter_mutex_id);

    SendMessage(ctx->tile_queue, &task_idx, sizeof(task_idx));
    ReceiveMessage(ctx->message_queue[task_idx], &tile);
  }

  SendMessage(ctx->tile_queue, &task_idx, sizeof(task_idx));
  SendEvents(ctx->main_task, event_send[task_idx]);
  SuspendSelf();
}


//=======================================================================================
// "snr_processing" Tasks/Functions
//=======================================================================================
static void task_snr_read_sensor_data(rtems_task_argument arg){
  test_context *ctx;
  uint32_t iter = 0;

  ctx = (test_context *)arg;

  ReceiveAllEvents(EVENT_GO_READ_SENSOR);

  while ( iter < MAX_ITER) {
    ObtainMutex(ctx->snr_mutex_id);

    for (uint32_t i = 0; i < FFT_SIZE; i++) {
      inSensorDataRe[i] = 0.4995f * cos_aprox(i*2.0f*PI*FREQ[iter%TC_MAX]/FS)
                                   + (noise_generator(0) * NOISE_FACTOR[iter%TC_MAX]);
      inSensorDataIm[i] = 0.4995f * sin_aprox(i*2.0f*PI*FREQ[iter%TC_MAX]/FS)
                                   + (noise_generator(0) * NOISE_FACTOR[iter % TC_MAX]);
      }

    ReleaseMutex(ctx->snr_mutex_id);
    SendEvents(ctx->task_snr_process_data_id, EVENT_DATA_READY);

    iter++;
    ReceiveAllEvents(EVENT_DATA_WAS_PROCESSED);
  }

  rtems_event_send(ctx->main_task, event_send[(TASK_COUNT-2)]);
  SuspendSelf();
}

static void task_snr_process_data(rtems_task_argument arg) {
  test_context *ctx;
  ctx = (test_context *)arg;
  uint32_t iter = 0;
  struct timespec begin_time;
  struct timespec end_time;

  while ( iter < MAX_ITER ) {
    ReceiveAllEvents(EVENT_DATA_READY);
    ObtainMutex(ctx->snr_mutex_id);
    rtems_clock_get_uptime(&begin_time);

    //Apply Blackman-Harris Window
    for (uint32_t i = 0; i < FFT_SIZE; i++) {
      inSensorDataRe[i] = inSensorDataRe[i]*blackman_harris(i, FFT_SIZE)/ctx->scaling_fft_factor;
      inSensorDataIm[i] = inSensorDataIm[i]*blackman_harris(i, FFT_SIZE)/ctx->scaling_fft_factor;
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
      if ((i > maxValueIndex - 8) && (i < maxValueIndex + 8)) {
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

    ReleaseMutex(ctx->snr_mutex_id);
    iter++;
    SendEvents(ctx->task_snr_read_sensor_data_id, EVENT_DATA_WAS_PROCESSED);
  }

  rtems_event_send(ctx->main_task, event_send[(TASK_COUNT-1)]);
  SuspendSelf();
}


//=======================================================================================
// INIT/MAIN Task
//=======================================================================================
static void Init(rtems_task_argument arg){
  (void)arg;
  test_context ctx;
  uint32_t start_time, end_time, elapsed_time;
  char ch, str[ITOA_STR_SIZE];
  uint32_t total_events = 0;
  uint8_t task = 255;
  bool correctly_processed = true;
  (void) memset(&ctx, 0, sizeof(test_context));
  (void) memset(&count_process[0], 0, TOTAL_TILES);
  (void) memset(&xL2[0], 0, xL2_ELEM);
  (void) memset(&inSensorDataRe[0], 0, FFT_SIZE);
  (void) memset(&inSensorDataIm[0], 0, FFT_SIZE);

#ifdef GR740_ESA_BOARD 
  soc_stats_regs soc_stats;
#endif


//-----------------------------------------------------------------------------
// Create/Initialize Objects
//-----------------------------------------------------------------------------
  rtems_message_queue_config msg_config = {
      .name = rtems_build_name('M', 'S', 'G', 'T'),
      .maximum_pending_messages = RTEMS_ARRAY_SIZE(msg_tile_queue_storage),
      .maximum_message_size = MAX_MESSAGE_SIZE,
      .storage_size = sizeof(msg_tile_queue_storage),
      .storage_area = &msg_tile_queue_storage,
      .attributes = RTEMS_FIFO | RTEMS_GLOBAL};

  ctx.main_task = rtems_task_self();
  ctx.tile_queue = CreateMessageQueue(msg_config);
  ctx.ntiles = TOTAL_TILES;
  ctx.next_tile = 1;

  rtems_task_config calc_task_config = {
      .initial_priority = PRIO_NORMAL,
      .storage_size = TASK_STORAGE_SIZE,
      .maximum_thread_local_storage_size = MAX_TLS_SIZE,
      .initial_modes = RTEMS_DEFAULT_MODES,
      .attributes = TASK_ATTRIBUTES};

  ctx.filter_mutex_id   = CreateMutex(rtems_build_name('M', 'U', 'T', '0'));
  ctx.snr_mutex_id = CreateMutex(rtems_build_name('M', 'U', 'T', '1'));
  SetSelfPriority( PRIO_NORMAL );

  for (uint32_t i = 0; i < TASK_COUNT-2; i++){
    ch = '0' + i;
    calc_task_config.name = rtems_build_name('R', 'U', 'N', ch);
    calc_task_config.storage_area = &calc_task_storage[i][0];

    ctx.task_filter_simulation_id[i] = DoCreateTask(calc_task_config);
    StartTask(ctx.task_filter_simulation_id[i], calc_task_function, &ctx);
    total_events += event_send[i];
  }

  ch = '0' + (TASK_COUNT-2);
  calc_task_config.name = rtems_build_name('R', 'U', 'N', ch);
  calc_task_config.storage_area = &calc_task_storage[(TASK_COUNT-2)][0];
  ctx.task_snr_read_sensor_data_id = DoCreateTask(calc_task_config);
  StartTask(ctx.task_snr_read_sensor_data_id, task_snr_read_sensor_data, &ctx);

  ch = '0' + (TASK_COUNT-1);
  calc_task_config.name = rtems_build_name('R', 'U', 'N', ch);
  calc_task_config.storage_area = &calc_task_storage[(TASK_COUNT-1)][0];
  ctx.task_snr_process_data_id = DoCreateTask(calc_task_config);
  StartTask(ctx.task_snr_process_data_id, task_snr_process_data, &ctx);

//-----------------------------------------------------------------------------
// Setup the testcase
//-----------------------------------------------------------------------------
  fill_main_memory_with_data();
  l1_dcache_flush();
  l1_dcache_disable();

#ifdef GR740_ESA_BOARD
  l2_cache_disable();
  l2_cache_flush();
  l2_cache_enable();
#endif

  l1_dcache_enable();
  uint32_t control_data_word = warmup_caches();

#ifdef GR740_ESA_BOARD
  // Locking cache way #3 and #4
  l2_cache_set_way_lock(2);
  l2_cache_enable_split_responses();
  clockgating_enable_l4stat();
  soc_stats_configure_regs();
  soc_stats_init(&soc_stats);
#endif

	//Calculates mean of Blackman-Harris terms --> to scale a bit the FFT
  ctx.scaling_fft_factor = 0.0;
  for (uint32_t i = 0; i<FFT_SIZE;  i++){
    ctx.scaling_fft_factor += blackman_harris( i, FFT_SIZE);
  }
  ctx.scaling_fft_factor = ctx.scaling_fft_factor/ ((float) FFT_SIZE);

//-----------------------------------------------------------------------------
// Do the work: distribute the work through the tasks
//-----------------------------------------------------------------------------
  SendEvents(ctx.task_snr_read_sensor_data_id, EVENT_GO_READ_SENSOR);
  start_time = rtems_clock_get_ticks_since_boot();

  rtems_event_set revents =  ReceiveAvailableEvents();
  while ( (revents & total_events) != total_events ) {
    ReceiveMessage(ctx.tile_queue, &task);
    SendMessage(ctx.message_queue[task], &ctx.next_tile, sizeof(ctx.next_tile));
    ctx.next_tile++;
    revents =  ReceiveAvailableEvents();
  }
  // Wait  for the SNR tasks to finish
  ReceiveAllEvents(event_send[(TASK_COUNT-2)]);
  ReceiveAllEvents(event_send[(TASK_COUNT-1)]);

  end_time = rtems_clock_get_ticks_since_boot();
  elapsed_time = end_time - start_time;

#ifdef GR740_ESA_BOARD
  soc_stats_update(&soc_stats);
#endif

//-----------------------------------------------------------------------------
// Show Results
//-----------------------------------------------------------------------------
  print_string("\n");
  print_string("Multicore Elapsed Time -");
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
  for (uint32_t i = 0; i < (TASK_COUNT-2); i++){
    DeleteTask(ctx.task_filter_simulation_id[i]);
    DeleteMessageQueue(ctx.message_queue[i]);
  }

  DeleteTask(ctx.task_snr_process_data_id);
  DeleteTask(ctx.task_snr_read_sensor_data_id);

  DeleteMutex(ctx.filter_mutex_id);
  DeleteMutex(ctx.snr_mutex_id);

  DeleteMessageQueue(ctx.tile_queue);
  rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, 0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS TEST_PROCESSORS

#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES MAX_MESSAGE_QUEUES

#define CONFIGURE_MAXIMUM_SEMAPHORES 2

#define CONFIGURE_MAXIMUM_TASKS ( TASK_COUNT + 1 )

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
