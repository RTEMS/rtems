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
 * @brief Tests impact performing of L2 cache split transactions
 *
 * Step 1: Uniprocessor case
 *
 * In this step the same set of math equations used by several tasks in the
 * Multiprocessor version are run in an Uniprocessor and ONE task only environment
 * in order to achieve a reference output result for comparison.
 *
 * There is only one set of math equations processed here:
 *  - named "filter_simulation"
 *
 * Result values are obtained for later comparison with multiprocessor
 * versions. Also some internal cache statistics are shown. Most relevant ones are the
 * number of L2 cache misses, which when increased significantly, may delay the data
 * processing and the number of split transactions which when enable should lower
 * the number of time wasted in waiting for other bus transactions to finish.
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
#define TOTAL_TILES 64

#define MAX_MESSAGE_SIZE sizeof(uint8_t)
#define MAX_PENDING_MESSAGES 10

rtems_event_set event_send[4] = {RTEMS_EVENT_1,
                                 RTEMS_EVENT_2,
                                 RTEMS_EVENT_3,
                                 RTEMS_EVENT_4};

uint8_t count_process[TOTAL_TILES];

typedef struct{
  uint8_t ntiles;
  uint8_t next_tile;
  rtems_id task_id[TASK_COUNT];
  rtems_id tile_queue;
  uint64_t accxL2;
} test_context;

#define ITOA_STR_SIZE     (8*sizeof(int)+1)

//-----------------------------------------------------------------------------------------
#define L2_CACHE_SIZE     (4U*512U*1024U)                     // 2Mbytes
#define L2_CACHE_WAY_SIZE (512U*1024U)                        // 512kbytes
#define xL2_ELEM          (8*L2_CACHE_SIZE/sizeof(uint32_t))  // 4M elements

#ifdef GR740_ESA_BOARD
RTEMS_ALIGNED(L2_CACHE_SIZE)
#endif
static uint32_t xL2[xL2_ELEM];

//-----------------------------------------------------------------------------------------
#define FILTER_TAPS       (16U)
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
    for (i = 0 ; i < FILTER_TAPS; i++)
      t1 += xL2[(j+i) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < FILTER_TAPS; i++)
      t1 += xL2[(j+i+xL2_ELEM*1/8/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < FILTER_TAPS; i++)
      t1 += xL2[(j+i+xL2_ELEM*2/8/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < FILTER_TAPS; i++)
      t1 += xL2[(j+i+xL2_ELEM*3/8/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < FILTER_TAPS; i++)
      t2 += xL2[(j+i+xL2_ELEM*4/8/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < FILTER_TAPS; i++)
      t2 += xL2[(j+i+xL2_ELEM*5/8/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < FILTER_TAPS; i++)
      t2 += xL2[(j+i+xL2_ELEM*6/8/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    for (i = 0 ; i < FILTER_TAPS; i++)
      t2 += xL2[(j+i+xL2_ELEM*7/8/sizeof(uint32_t)) % xL2_ELEM]*coefs[i];

    // Simulating normalization
    t3 = ((t1*1000)/(42*137)) + 1;
    t4 = ((t2*10)/(96*137)) + 1;
    uint64_t t5 = (uint64_t)t3 * (uint64_t)t3;
    uint64_t t6 = (uint64_t)t4 * (uint64_t)t4;
    acc += (t5+t6)/((uint64_t)isqrt(t4+t3));
  }
  return acc;
}

static void Init(rtems_task_argument arg){
  (void)arg;
  test_context ctx;
  uint32_t start_time, end_time, elapsed_time;
  char str[ITOA_STR_SIZE];
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
  l2_cache_enable();
#endif

  l1_dcache_enable();
  uint32_t control_data_word = warmup_caches();

#ifdef GR740_ESA_BOARD
  l2_cache_disable_split_responses();
  clockgating_enable_l4stat();
  soc_stats_configure_regs();
  soc_stats_init(&soc_stats);
#endif

//-----------------------------------------------------------------------------
// Do the work
//-----------------------------------------------------------------------------
  start_time = rtems_clock_get_ticks_since_boot();
  ctx.accxL2 = calc_filter_simulation_equation(0, 1);

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
