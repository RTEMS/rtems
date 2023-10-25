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
 * @brief Tests impact performing of L2 cache enabled/disable
 *
 * Step 2: Multiprocessor with L2 cache enabled
 *
 * There is one set of math equations processed:
 *  - named "filter_simulation"
 *
 * The equations for "filter_simulation" are used by TEST_PROCESSORS tasks that
 * processes small sections of a "data array" in parallel.
 *
 * The locations in memory for acessing the small sections of the "data arrays" do
 * overlap in terms of cache memory positions, and when acessing one section of the
 * "data array", or even a smaller part of one, other sections of that "data array"
 * should be kicked out of the cache. Sometimes that may happen within the same task.
 *
 * With the "L2 cache" disabled, every data word missed in l1 cache must be retreived
 * from the main memory which can be significantly slower that L2 Cache. With the
 * "L2 cache" enabled, in the case of a l2 cache hit a small number of data words
 * are transactioned with the L1 cache right away, avoiding acessing the slower main
 * memory.
 *
 * Expected Results:
 *   - The Tiles must be processed only once.
 *   - "Ouput Data Result Value" must match with the Uniprocessor version and with the
 *      Multiprocessor version with L2 cache disabled.
 *   - Elapsed Time should be lower than the Uniprocessor version, and also lower
 *      than Multiprocessor version with L2 cache disable.
 *   - Significant number of L2 cache misses
 *
 */

/**
 *
 * For standalone tests in the actual hardware boards the following options can be used:
 *
 *  1) make XFLAGS="-Dgr740 -DGR740_ESA_BOARD"
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
#define TASK_COUNT TEST_PROCESSORS
#define TOTAL_TILES 64

#define MAX_MESSAGE_QUEUES 5
#define MAX_MESSAGE_SIZE sizeof(uint8_t)
#define MAX_PENDING_MESSAGES 10

rtems_event_set event_send[4] = {RTEMS_EVENT_1,
                                 RTEMS_EVENT_2,
                                 RTEMS_EVENT_3,
                                 RTEMS_EVENT_4};

uint8_t count_process[TOTAL_TILES];

typedef struct{
  rtems_id main_task;
  uint8_t ntiles;
  uint8_t next_tile;
  rtems_id task_id[TASK_COUNT];
  rtems_id tile_queue;
  rtems_id message_queue[TASK_COUNT];
  rtems_id mutex_id;
  uint64_t accxL2;
} test_context;

RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static char calc_task_storage[TASK_COUNT][TASK_STORAGE_SIZE];

RTEMS_MESSAGE_QUEUE_BUFFER(MAX_MESSAGE_SIZE)
msg_tile_queue_storage[MAX_PENDING_MESSAGES];

RTEMS_MESSAGE_QUEUE_BUFFER(MAX_MESSAGE_SIZE)
msg_task_queue_storage[TASK_COUNT][MAX_PENDING_MESSAGES];


#define ITOA_STR_SIZE     (8*sizeof(int)+1)

//-----------------------------------------------------------------------------------------
#define L2_CACHE_SIZE     (4U*512U*1024U)                     // 2Mbytes
#define L2_CACHE_WAY_SIZE (512U*1024U)                        // 512kbytes
#define xL2_ELEM          (L2_CACHE_SIZE/sizeof(uint32_t))    // 512k elements

#ifdef GR740_ESA_BOARD
RTEMS_ALIGNED(L2_CACHE_SIZE)
#endif
static uint32_t xL2[xL2_ELEM];

//-----------------------------------------------------------------------------------------
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

static uint64_t calc_filter_simulation_equation(uint8_t tile, uint32_t total_elems){
  uint64_t acc = 0;
  const uint32_t begin_idx = tile*xL2_ELEM/total_elems;
  const uint32_t end_idx   = begin_idx + (xL2_ELEM/total_elems) - 1;

  for ( uint32_t j = begin_idx ; j <= end_idx; j ++) {
    uint32_t i;
    // Simulating filtering/convolution
    uint32_t t1 = 0, t2 = 0, t3 = 0, t4 = 0;
    for (i = 0 ; i < 128; i++)
      t1 += xL2[(j+i) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < 128; i++)
      t1 += xL2[(j+i+L2_CACHE_WAY_SIZE*1/8/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < 128; i++)
      t1 += xL2[(j+i+L2_CACHE_WAY_SIZE*2/8/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < 128; i++)
      t1 += xL2[(j+i+L2_CACHE_WAY_SIZE*3/8/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < 128; i++)
      t2 += xL2[(j+i+L2_CACHE_WAY_SIZE*4/8/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < 128; i++)
      t2 += xL2[(j+i+L2_CACHE_WAY_SIZE*5/8/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < 128; i++)
      t2 += xL2[(j+i+L2_CACHE_WAY_SIZE*6/8/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < 128; i++)
      t2 += xL2[(j+i+L2_CACHE_WAY_SIZE*7/8/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    // Simulating normalization
    t3 = ((t1*1000)/(42*137)) + 1;
    t4 = ((t2*10)/(96*137)) + 1;
    uint64_t t5 = (uint64_t)t3 * (uint64_t)t3;
    uint64_t t6 = (uint64_t)t4 * (uint64_t)t4;
    acc += (t5+t6)/((uint64_t)isqrt(t4+t3));
  }
  return acc;
}


static void calc_task_function(rtems_task_argument arg)
{
  test_context *ctx;
  uint64_t acc;

  ctx = (test_context *)arg;

  uint8_t tile;
  rtems_id local_id = TaskSelfId();
  uint8_t task_idx = 255;

  for (int i = 0; i < TASK_COUNT; i++){
    if (ctx->task_id[i] == local_id){
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
    acc = 0;
    count_process[tile - 1]++;
    acc = calc_filter_simulation_equation(tile-1, ctx->ntiles);

    ObtainMutex(ctx->mutex_id);
    ctx->accxL2 += acc;
    ReleaseMutex(ctx->mutex_id);

    SendMessage(ctx->tile_queue, &task_idx, sizeof(task_idx));
    ReceiveMessage(ctx->message_queue[task_idx], &tile);
  }

  SendMessage(ctx->tile_queue, &task_idx, sizeof(task_idx));
  SendEvents(ctx->main_task, event_send[task_idx]);
  SuspendSelf();
}

static void Init(rtems_task_argument arg)
{
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

  ctx.mutex_id = CreateMutex(rtems_build_name('M', 'U', 'T', 'X'));

  for (uint32_t i = 0; i < TASK_COUNT; i++){
    ch = '0' + i;
    calc_task_config.name = rtems_build_name('R', 'U', 'N', ch);
    calc_task_config.storage_area = &calc_task_storage[i][0];

    ctx.task_id[i] = DoCreateTask(calc_task_config);
    StartTask(ctx.task_id[i], calc_task_function, &ctx);
    total_events += event_send[i];
  }
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
  l2_cache_enable();
#endif

  l1_dcache_enable();
  uint32_t control_data_word = warmup_caches();

#ifdef GR740_ESA_BOARD
  clockgating_enable_l4stat();
  soc_stats_configure_regs();
  soc_stats_init(&soc_stats);
#endif

//-----------------------------------------------------------------------------
// Do the work: distribute the work through the tasks
//-----------------------------------------------------------------------------
  start_time = rtems_clock_get_ticks_since_boot();
  while (ReceiveAvailableEvents() != total_events)
  {
    ReceiveMessage(ctx.tile_queue, &task);
    SendMessage(ctx.message_queue[task], &ctx.next_tile, sizeof(ctx.next_tile));
    ctx.next_tile++;
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
  for (uint32_t i = 0; i < TASK_COUNT; i++){
    DeleteTask(ctx.task_id[i]);
    DeleteMessageQueue(ctx.message_queue[i]);
  }
  DeleteMessageQueue(ctx.tile_queue);

  DeleteMutex(ctx.mutex_id);

  rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, 0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS TEST_PROCESSORS

#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES MAX_MESSAGE_QUEUES

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_MAXIMUM_TASKS ( TEST_PROCESSORS + 1 )

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
