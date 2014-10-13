/**
 *  @file
 *
 *  Cache Management Support Routines for the MCF5225x
 */

#include <rtems.h>
#include "cache_.h"

/*
 * Cannot be frozen
 */
void _CPU_cache_freeze_data(void) {}
void _CPU_cache_unfreeze_data(void) {}
void _CPU_cache_freeze_instruction(void) {}
void _CPU_cache_unfreeze_instruction(void) {}

/*
 * Write-through data cache -- flushes are unnecessary
 */
void _CPU_cache_flush_1_data_line(const void *d_addr) {}
void _CPU_cache_flush_entire_data(void) {}

void _CPU_cache_enable_instruction(void) {}
void _CPU_cache_disable_instruction(void) {}
void _CPU_cache_invalidate_entire_instruction(void) {}
void _CPU_cache_invalidate_1_instruction_line(const void *addr) {}

void _CPU_cache_enable_data(void) {}
void _CPU_cache_disable_data(void) {}
void _CPU_cache_invalidate_entire_data(void) {}
void _CPU_cache_invalidate_1_data_line(const void *addr) {}
