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

#ifndef _LOW_LEVEL_UTILS_H
#define _LOW_LEVEL_UTILS_H

#include <bsp/leon3.h>
#include <grlib/l2cache-regs.h>
#include <grlib/io.h>


// ------------------------------------------------------------------------------------------------
// Leon3/4 Configuration Registers
// ------------------------------------------------------------------------------------------------

uint32_t leon3_get_configuration_register( void );

uint32_t leon3_get_psr_register( void );

uint32_t leon3_get_wim_register( void );

uint32_t leon3_get_tbr_register( void );


// ------------------------------------------------------------------------------------------------
// L1 Cache definitions 
// ------------------------------------------------------------------------------------------------
// L1 Cache Replacement policies  
#define L1_CACHE_REPL_DIRECT_MAPPED       (0x00000000U)   // 00: no replacement policy (direct-mapped cache)
#define L1_CACHE_REPL_LRU                 (0x00000001U)   // 01: least recently used (LRU)
#define L1_CACHE_REPL_LRR                 (0x00000002U)   // 10: least recently replaced (LRR)
#define L1_CACHE_REPL_RANDOM              (0x00000003U)   // 11: random

void l1_dcache_flush(void);

void l1_dcache_disable(void);

void l1_dcache_enable(void);

void l1_dcache_set_replacement_policy(uint32_t policy);


// ------------------------------------------------------------------------------------------------
// L2 Cache definitions 
// ------------------------------------------------------------------------------------------------  
// L2 Cache Replacement policies  
#define L2_CACHE_REPL_LRU                 (0x00000000U)   // 00: LRU
#define L2_CACHE_REPL_PSEUDORANDOM        (0x00000001U)   // 01: (pseudo-) random
#define L2_CACHE_REPL_MASTERINDEX_REPLACE (0x00000002U)   // 10: Master-index using index-replace field
#define L2_CACHE_REPL_MASTERINDEX_MODULUS (0x00000003U)   // 11: Master-index using the modulus function

void l2_cache_disable(void);

void l2_cache_enable(void);

void l2_cache_flush(void);

void l2_cache_set_replacement_policy(uint32_t policy, uint32_t index_way);

void l2_cache_set_way_lock(uint32_t ways);

void l2_cache_enable_split_responses(void);

void l2_cache_disable_split_responses(void);

size_t l2_cache_get_size(void);

void l2_cache_print_tags(uint32_t n);


// ------------------------------------------------------------------------------------------------
// SOC Statistics definitions 
// ------------------------------------------------------------------------------------------------
#define L4_STAT_BASE                      (0xFFA0D000U)

typedef struct soc_stats_regs {
	uint32_t l1_inst_cache_miss[4];
	uint32_t l1_data_cache_miss[4];
	uint32_t l2_cache_hits;
	uint32_t l2_cache_miss;					
	uint32_t ahb_split_delay;				

} soc_stats_regs;

void soc_stats_init(soc_stats_regs *stats);

void soc_stats_configure_regs(void);

void soc_stats_update(soc_stats_regs *stats);

void l4_stats_print(void);


// ------------------------------------------------------------------------------------------------
// Clock Gating Definitions
// ------------------------------------------------------------------------------------------------
#define GRCLKGATE_BASE                     (0xFFA04000U)

void clockgating_enable_l4stat(void);

#endif /* !_LOW_LEVEL_UTILS_H */
/* end of include file */
