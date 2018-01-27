/**
 *  @file
 *
 *  Cache Management Support Routines for the MCF5225x
 */

#include <rtems.h>
#include "cache.h"

/*
 * Cannot be frozen
 */
static void _CPU_cache_freeze_data(void) {}
static void _CPU_cache_unfreeze_data(void) {}
static void _CPU_cache_freeze_instruction(void) {}
static void _CPU_cache_unfreeze_instruction(void) {}

/*
 * Write-through data cache -- flushes are unnecessary
 */
static void _CPU_cache_flush_1_data_line(const void *d_addr) {}
static void _CPU_cache_flush_entire_data(void) {}

static void _CPU_cache_enable_instruction(void) {}
static void _CPU_cache_disable_instruction(void) {}
static void _CPU_cache_invalidate_entire_instruction(void) {}
static void _CPU_cache_invalidate_1_instruction_line(const void *addr) {}

static void _CPU_cache_enable_data(void) {}
static void _CPU_cache_disable_data(void) {}
static void _CPU_cache_invalidate_entire_data(void) {}
static void _CPU_cache_invalidate_1_data_line(const void *addr) {}

#include "../../../shared/cache/cacheimpl.h"
