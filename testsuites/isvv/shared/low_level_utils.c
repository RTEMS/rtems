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

#include <string.h>
#include "low_level_utils.h"
#include "utils.h"

// ------------------------------------------------------------------------------------------------
// Pricate Declarations
// ------------------------------------------------------------------------------------------------

// Processor numeration
#define CPU_0                             (0U)
#define CPU_1                             (1U)
#define CPU_2                             (2U)
#define CPU_3                             (3U)
    
// L1 Cache Bit registers bit definitions 
#define L1_CACHE_CTRL_FD                  (0x00400000U)
#define L1_CACHE_CTRL_DP                  (0x00004000U)
#define L1_CACHE_CTRL_DCS                 (0x0000000CU)

// L1 Cache Replacement policies  
#define L1_CACHE_REPL_MAX                 (3U)
#define L1_CACHE_REPL_MASK                (0xCFFFFFFF) 
#define L1_CACHE_REPL_SHIFT               (28U)    
    
// L2 Cache Bit registers bit definitions 
#define L2_CACHE_L2CC_EN                  (0x80000000U)
#define L2_CACHE_L2CFMA_FMODE_FLUSH_ALL   (0x00000007U)
    
// L2 Cache Replacement policies  
#define L2_CACHE_REPL_MAX                 (3U)
#define L2_CACHE_ENABLE_SPLIT_RESPONSES   (0x00000002U)

#define L2_CACHE_REPL_MASK                (0xCFFFFFFF) 
#define L2_CACHE_REPL_SHIFT               (28U)
#define L2_CACHE_WAYS_MAX                 (4U)    
#define L2_CACHE_WAYS_MASK                (0xFFFFF0FF) 
#define L2_CACHE_WAYS_LOCK_SHIFT          (8U)
#define L2_CACHE_INDEX_WAYS_MASK          (0xFFFF0FFF) 
#define L2_CACHE_INDEX_WAYS_SHIFT         (12U)

// L4 Stats definitions
#define L4_STAT_CLEAR_COUNTER_ON_READ     (0x00002000U)
#define L4_STAT_ENABLE_COUNTER            (0x00001000U) 
#define L4_STAT_L1_INST_CACHE_MISS_EVENT  (0x00000000U)
#define L4_STAT_L1_DATA_CACHE_MISS_EVENT  (0x00000008U)
#define L4_STAT_L2_CACHE_HIT_EVENT        (0x00000060U)
#define L4_STAT_L2_CACHE_MISS_EVENT       (0x00000061U)
#define L4_STAT_AHB_SPLIT_DELAY_EVENT     (0x0000004EU)
#define L4_STAT_EVENT_ID_SHIFT            (0x00000004U)
#define L4_STAT_EVENT_ID_MASK             (0x00000FF0U)
#define L4_STAT_CPU_SHIFT                 (0x00000000U)
#define L4_STAT_CPU_MASK                  (0x0000000FU)


typedef struct l4stat_regs {
  uint32_t cval[32];				  /* 0x000 */
  uint32_t cctrl[32];				  /* 0x080 */
  uint32_t cmax[32];				  /* 0x100 */
  uint32_t timestamp;				  /* 0x180 */
} l4stat_regs;


// ------------------------------------------------------------------------------------------------
// Leon3/4 internal registers access - Public functions implementation
// ------------------------------------------------------------------------------------------------

uint32_t leon3_get_configuration_register(void) {
  uint32_t asr17;
  __asm__ volatile ("mov %%asr17, %0" : "=&r" (asr17) );
  return asr17;
}

uint32_t leon3_get_psr_register(void) {
  uint32_t psr;
  __asm__ volatile ("mov %%psr, %0" : "=&r" (psr) );
  return psr;
}

uint32_t leon3_get_wim_register(void) {
  uint32_t wim;
  __asm__ volatile ("mov %%wim, %0" : "=&r" (wim) );
  return wim;
}

uint32_t leon3_get_tbr_register(void) {
  uint32_t tbr;
  __asm__ volatile ("mov %%tbr, %0" : "=&r" (tbr) );
  return tbr;
}


// ------------------------------------------------------------------------------------------------
// L1 Cache peripheral/module - Public functions implementation 
// ------------------------------------------------------------------------------------------------

void l1_dcache_disable(void) {
  // Set the Data Cache state bits (DCS bits 3:2) to '00' in the Cache control register
  uint32_t cache_reg = leon3_get_cache_control_register();
  cache_reg &= ~L1_CACHE_CTRL_DCS;
  leon3_set_cache_control_register(cache_reg);   
}

void l1_dcache_enable(void) {
  // Set the Data Cache state bits (DCS bits 3:2) to '11' in the Cache control register
  uint32_t cache_reg = leon3_get_cache_control_register();
  cache_reg |= L1_CACHE_CTRL_DCS;
  leon3_set_cache_control_register(cache_reg);   
}

void l1_dcache_flush(void) {
  // Set the Flush data cache bit (FD - bit 22) to '1' in the Cache control register
  uint32_t cache_reg = leon3_get_cache_control_register();
  cache_reg |= L1_CACHE_CTRL_FD;
  leon3_set_cache_control_register(cache_reg);  
  
  // Loop while the Data cache flush pending bit (DP - 14) in the Cache control register is set to '1'
  do {
    cache_reg = leon3_get_cache_control_register();
  }
  while ( (cache_reg & L1_CACHE_CTRL_DP) == L1_CACHE_CTRL_DP);  
}

void l1_dcache_set_replacement_policy(uint32_t policy) {
  if (policy <= L1_CACHE_REPL_MAX){
    uint32_t cache_reg = leon3_get_data_cache_config_register();
    cache_reg = (cache_reg & L1_CACHE_REPL_MASK) | (policy << L1_CACHE_REPL_SHIFT);    
    // There is no leon3_set_data_cache_config_register() in leon3.h, so a lower level function is used
    leon3_set_system_register( 0xC, cache_reg );
  }
}


// ------------------------------------------------------------------------------------------------
// L2 Cache peripheral/module - Public functions implementation 
// ------------------------------------------------------------------------------------------------

void l2_cache_disable(void) {
  // Set the L2 Controller Cache enable bit register  (EN bit 31) to '0' in the L2C Control register (L2CS)
  volatile l2cache *regs;
  regs = (l2cache *) LEON3_L2CACHE_BASE;
  uint32_t status = regs->l2cc;
  status &= ~L2_CACHE_L2CC_EN;
  regs->l2cc = status;  
}

void l2_cache_enable(void) {
  // Set the L2 Controller Cache enable bit register (EN bit 31) to '1' in the L2C Control register (L2CS)
  volatile l2cache *regs;
  regs = (l2cache *) LEON3_L2CACHE_BASE;
  uint32_t status = regs->l2cc;
  status |= L2_CACHE_L2CC_EN;
  regs->l2cc = status;  
}

void l2_cache_flush(void) {
  // Set the L2 Flush mode bit registers ( FMODE bits 2:0) to '111' in the "L2C Flush
  //   (Memory address) register" (L2CFMA)  
  volatile l2cache *regs;
  regs = (l2cache *) LEON3_L2CACHE_BASE;  
  uint32_t status = L2_CACHE_L2CFMA_FMODE_FLUSH_ALL;
  regs->l2cfma = status;  

  // There is no active way to wait for the L2 flush to finish, so a loop is added 
  Loop(1000);
}

void l2_cache_set_replacement_policy(uint32_t policy, uint32_t index_way) {
  // The index_way is only used if the Policy is L2_CACHE_REPL_MASTERINDEX_REPLACE
  //   (10: Master-index using index-replace field)
  if ((policy <= L2_CACHE_REPL_MAX) && (index_way <= L2_CACHE_REPL_MAX)) {
    volatile l2cache *regs;
    regs = (l2cache *) LEON3_L2CACHE_BASE;
    uint32_t status = regs->l2cc;
    status = (status & L2_CACHE_REPL_MASK) | (policy << L2_CACHE_REPL_SHIFT) ;
    if ( policy == L2_CACHE_REPL_MASTERINDEX_REPLACE)
      status = (status & L2_CACHE_INDEX_WAYS_MASK)| (index_way << L2_CACHE_INDEX_WAYS_SHIFT);
    regs->l2cc = status;  
  }
}

void l2_cache_set_way_lock(uint32_t ways) {
  // We can only control the number of cache ways to be locked:
  //  if 1 way is to be locked then way#4 is locked,
  //  if 2 ways are to be locked then way#3 and way#4 are locked,
  //  if 3 ways are to be locked then way#2, way#3 and way#4 are locked,
  //  otherwise all cache ways are locked.

  if (ways <= L2_CACHE_WAYS_MAX) {
    volatile l2cache *regs;
    regs = (l2cache *) LEON3_L2CACHE_BASE;
    uint32_t status = regs->l2cc;
    status = (status & L2_CACHE_WAYS_MASK) | (ways << L2_CACHE_WAYS_LOCK_SHIFT);
    regs->l2cc = status;  
  }
}

void l2_cache_enable_split_responses(void) {
  volatile l2cache *regs;  
  regs = (l2cache *) LEON3_L2CACHE_BASE;
  uint32_t status = regs->l2caccc;
  status |= L2_CACHE_ENABLE_SPLIT_RESPONSES;
  regs->l2caccc = status;  
}

void l2_cache_disable_split_responses(void) {
  volatile l2cache *regs;
  regs = (l2cache *) LEON3_L2CACHE_BASE;
  uint32_t status = regs->l2caccc;
  status &= ~L2_CACHE_ENABLE_SPLIT_RESPONSES;
  regs->l2caccc = status;  
}

size_t l2_cache_get_size(void) {
  volatile l2cache *regs;
  unsigned status;
  unsigned ways;
  unsigned set_size;

  regs = (l2cache *) LEON3_L2CACHE_BASE;
  status = regs->l2cs;
  ways = L2CACHE_L2CS_WAY_GET(status) + 1;
  set_size = L2CACHE_L2CS_WAY_SIZE_GET(status) * 1024;

  return ways * set_size;
}


void l2_cache_print_tags(uint32_t n) {
  const uint32_t LEON3_L2CACHE_TAGS_OFFSET_ADDR = 0x80000U;
  char str[8*sizeof(int)+1];
  
  for ( uint32_t i=0; i<32*n; i+=4) {
    volatile uint32_t addr = LEON3_L2CACHE_BASE + LEON3_L2CACHE_TAGS_OFFSET_ADDR + i;
    if (addr%32 < 16) {
      print_string("L2 cache Tag @addr[0x");
      print_string(itoa( addr, &str[0], 16));
      print_string("] : 0x");
      print_string(itoa(grlib_load_32((void*)(addr)), &str[0], 16));
      print_string("\n");
    }
  }
}


// ------------------------------------------------------------------------------------------------
// Leon4 Statistics peripheral/module - Public functions implementation
// ------------------------------------------------------------------------------------------------

void soc_stats_configure_regs(void) {
  volatile l4stat_regs *regs;
  uint32_t status;

  regs = (l4stat_regs *) L4_STAT_BASE;

  // "Counter 0 value register": Event "0x00 Instruction Data cache (read) miss"  on processor #0
  status  = L4_STAT_CLEAR_COUNTER_ON_READ | L4_STAT_ENABLE_COUNTER;
  status |= L4_STAT_L1_INST_CACHE_MISS_EVENT << L4_STAT_EVENT_ID_SHIFT;
  status |= CPU_0 << L4_STAT_CPU_SHIFT;
  regs->cctrl[0] = status;  

  // "Counter 1 value register": Event "0x00 Instruction cache (read) miss"  on processor #1
  status  = L4_STAT_CLEAR_COUNTER_ON_READ | L4_STAT_ENABLE_COUNTER;
  status |= L4_STAT_L1_INST_CACHE_MISS_EVENT << L4_STAT_EVENT_ID_SHIFT;
  status |= CPU_1 << L4_STAT_CPU_SHIFT;
  regs->cctrl[1] = status;  

  // "Counter 2 value register": Event "0x00 Instruction cache (read) miss"  on processor #2
  status  = L4_STAT_CLEAR_COUNTER_ON_READ | L4_STAT_ENABLE_COUNTER;
  status |= L4_STAT_L1_INST_CACHE_MISS_EVENT << L4_STAT_EVENT_ID_SHIFT;
  status |= CPU_2 << L4_STAT_CPU_SHIFT;
  regs->cctrl[2] = status;  

  // "Counter 3 value register": Event 0x00 Instruction cache (read) miss"  on processor #3
  status  = L4_STAT_CLEAR_COUNTER_ON_READ | L4_STAT_ENABLE_COUNTER;
  status |= L4_STAT_L1_INST_CACHE_MISS_EVENT << L4_STAT_EVENT_ID_SHIFT;
  status |= CPU_3 << L4_STAT_CPU_SHIFT;
  regs->cctrl[3] = status;  

  // "Counter 4 value register": Event "0x08 Data cache (read) miss"  on processor #0
  status  = L4_STAT_CLEAR_COUNTER_ON_READ | L4_STAT_ENABLE_COUNTER;
  status |= L4_STAT_L1_DATA_CACHE_MISS_EVENT << L4_STAT_EVENT_ID_SHIFT;
  status |= CPU_0 << L4_STAT_CPU_SHIFT;
  regs->cctrl[4] = status;  

  // "Counter 5 value register": Event "0x08 Data cache (read) miss"  on processor #1
  status  = L4_STAT_CLEAR_COUNTER_ON_READ | L4_STAT_ENABLE_COUNTER;
  status |= L4_STAT_L1_DATA_CACHE_MISS_EVENT << L4_STAT_EVENT_ID_SHIFT;
  status |= CPU_1 << L4_STAT_CPU_SHIFT;
  regs->cctrl[5] = status;  

  // "Counter 6 value register": Event "0x08 Data cache (read) miss"  on processor #2
  status  = L4_STAT_CLEAR_COUNTER_ON_READ | L4_STAT_ENABLE_COUNTER;
  status |= L4_STAT_L1_DATA_CACHE_MISS_EVENT << L4_STAT_EVENT_ID_SHIFT;
  status |= CPU_2 << L4_STAT_CPU_SHIFT;
  regs->cctrl[6] = status;  

  // "Counter 7 value register": Event "0x08 Data cache (read) miss"  on processor #3
  status  = L4_STAT_CLEAR_COUNTER_ON_READ | L4_STAT_ENABLE_COUNTER;
  status |= L4_STAT_L1_DATA_CACHE_MISS_EVENT << L4_STAT_EVENT_ID_SHIFT;
  status |= CPU_3 << L4_STAT_CPU_SHIFT;
  regs->cctrl[7] = status;  

  // "Counter 8 value register": Event "0x60 L2 cache hits (external event 0, CPU/AHBM
  //   field can select AHB master)"
  status  = L4_STAT_CLEAR_COUNTER_ON_READ | L4_STAT_ENABLE_COUNTER;
  status |= L4_STAT_L2_CACHE_HIT_EVENT << L4_STAT_EVENT_ID_SHIFT;
  regs->cctrl[8] = status;  

  // "Counter 9 value register": Event "0x61 L2 cache miss (external event 1, CPU/AHBM
  //   field can select AHB master)"
  status  = L4_STAT_CLEAR_COUNTER_ON_READ | L4_STAT_ENABLE_COUNTER;
  status |= L4_STAT_L2_CACHE_MISS_EVENT << L4_STAT_EVENT_ID_SHIFT;
  regs->cctrl[9] = status;    

  // "Counter 10 value register": Event "0x4E AHB SPLIT delay. Filtered on CPU/AHBM 
  //   if SU(1) = ‘1‘"
  status  = L4_STAT_CLEAR_COUNTER_ON_READ | L4_STAT_ENABLE_COUNTER;
  status |= L4_STAT_AHB_SPLIT_DELAY_EVENT << L4_STAT_EVENT_ID_SHIFT;
  status |= CPU_1 << L4_STAT_CPU_SHIFT;
  regs->cctrl[10] = status;
}

void soc_stats_init(soc_stats_regs *stats) {
  volatile l4stat_regs *regs;
  regs = (l4stat_regs *) L4_STAT_BASE;

  memset(stats, 0, sizeof(soc_stats_regs));
  
  // The following dummy read operations purpose is to clean the statistcs counters
  (void) regs->cval[0];
  (void) regs->cval[1];
  (void) regs->cval[2];
  (void) regs->cval[3];
  (void) regs->cval[4];
  (void) regs->cval[5];
  (void) regs->cval[6];
  (void) regs->cval[7];
  (void) regs->cval[8];
  (void) regs->cval[9];
  (void) regs->cval[10];
}

void soc_stats_update(soc_stats_regs *stats){
  volatile l4stat_regs *regs;

  regs = (l4stat_regs *) L4_STAT_BASE;
  stats->l1_inst_cache_miss[0] += regs->cval[0];
  stats->l1_inst_cache_miss[1] += regs->cval[1];
  stats->l1_inst_cache_miss[2] += regs->cval[2];
  stats->l1_inst_cache_miss[3] += regs->cval[3];
  stats->l1_data_cache_miss[0] += regs->cval[4];
  stats->l1_data_cache_miss[1] += regs->cval[5];
  stats->l1_data_cache_miss[2] += regs->cval[6];
  stats->l1_data_cache_miss[3] += regs->cval[7];
  stats->l2_cache_hits         += regs->cval[8];
  stats->l2_cache_miss         += regs->cval[9];
  stats->ahb_split_delay       += regs->cval[10];
}

void l4_stats_print(void) {
  char str[8*sizeof(int)+1];
  print_string("------------------------------------------------------------\n");
  print_string("Printing L4 STAT REGS current value\n");
  print_string("L1 Instructions Cache misses CPU_0        : ");
  print_string(itoa(grlib_load_32((uint32_t*)( L4_STAT_BASE +0x00U)), &str[0], 10));    
  print_string("\n");
  print_string("L1 Instructions Cache misses CPU_1        : ");
  print_string(itoa(grlib_load_32((uint32_t*)( L4_STAT_BASE +0x04U)), &str[0], 10));    
  print_string("\n");
  print_string("L1 Instructions Cache misses CPU_2        : ");
  print_string(itoa(grlib_load_32((uint32_t*)( L4_STAT_BASE +0x08U)), &str[0], 10));    
  print_string("\n");
  print_string("L1 Instructions Cache misses CPU_3        : ");
  print_string(itoa(grlib_load_32((uint32_t*)( L4_STAT_BASE +0x0CU)), &str[0], 10));    
  print_string("L1 Data Cache misses CPU_0                : ");
  print_string(itoa(grlib_load_32((uint32_t*)( L4_STAT_BASE +0x10U)), &str[0], 10));    
  print_string("\n");
  print_string("L1 Data Cache misses CPU_1                : ");
  print_string(itoa(grlib_load_32((uint32_t*)( L4_STAT_BASE +0x14U)), &str[0], 10));    
  print_string("\n");
  print_string("L1 Data Cache misses CPU_2                : ");
  print_string(itoa(grlib_load_32((uint32_t*)( L4_STAT_BASE +0x18U)), &str[0], 10));    
  print_string("\n");
  print_string("L1 Data Cache misses CPU_3                : ");
  print_string(itoa(grlib_load_32((uint32_t*)( L4_STAT_BASE +0x1CU)), &str[0], 10));  
  print_string("\n");  
  print_string("L2 Cache hits                             : ");
  print_string(itoa(grlib_load_32((uint32_t*)( L4_STAT_BASE +0x20U)), &str[0], 10));    
  print_string("\n");
  print_string("L2 Cache misses                           : ");
  print_string(itoa(grlib_load_32((uint32_t*)( L4_STAT_BASE +0x24U)), &str[0], 10));    
  print_string("\n");
  print_string("AHB Splits                                : ");
  print_string(itoa(grlib_load_32((uint32_t*)( L4_STAT_BASE +0x30U)), &str[0], 10));    
  print_string("\n");  
}

// ------------------------------------------------------------------------------------------------
// Clock Gating peripheral/module - Public functions implementation
// ------------------------------------------------------------------------------------------------

void clockgating_enable_l4stat(void) {
  // 1. Write a 1 to the corresponding bit in the unlock register
  volatile uint32_t reg = grlib_load_32((uint32_t*)(GRCLKGATE_BASE + 0x00U));
  reg |= 0x40LU;
  grlib_store_32((uint32_t*)(GRCLKGATE_BASE + 0x00U), reg);

  // 2. Write a 1 to the corresponding bit in the core reset register
  reg = grlib_load_32((uint32_t*)(GRCLKGATE_BASE + 0x08U));
  reg |= 0x40LU;
  grlib_store_32((uint32_t*)(GRCLKGATE_BASE + 0x08U), reg);

  // 3. Write a 1 to the corresponding bit in the clock enable register
  reg = grlib_load_32((uint32_t*)(GRCLKGATE_BASE + 0x04U));
  reg |= 0x40LU;
  grlib_store_32((uint32_t*)(GRCLKGATE_BASE + 0x04U), reg);

  // 4. Write a 0 to the corresponding bit in the clock enable register
  reg = grlib_load_32((uint32_t*)(GRCLKGATE_BASE + 0x04U));
  reg &= ~0x40LU;
  grlib_store_32((uint32_t*)(GRCLKGATE_BASE + 0x04U), reg);
  
  // 5. Write a 0 to the corresponding bit in the core reset register
  reg = grlib_load_32((uint32_t*)(GRCLKGATE_BASE + 0x08U));
  reg &= ~0x40LU;
  grlib_store_32((uint32_t*)(GRCLKGATE_BASE + 0x08U), reg);

  // 6. Write a 1 to the corresponding bit in the clock enable register
  reg = grlib_load_32((uint32_t*)(GRCLKGATE_BASE + 0x04U));
  reg |= 0x40LU;
  grlib_store_32((uint32_t*)(GRCLKGATE_BASE + 0x04U), reg);

  // 7. Write a 0 to the corresponding bit in the unlock register
  reg = grlib_load_32((uint32_t*)(GRCLKGATE_BASE + 0x00U));
  reg &= ~0x40LU;
  grlib_store_32((uint32_t*)(GRCLKGATE_BASE + 0x00U), reg);
}
