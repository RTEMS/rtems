/*
 * or1k Cache Manager Support
 */

#ifndef __OR1K_CACHE_H
#define __OR1K_CACHE_H

#include <bsp/cache_.h>
#include <libcpu/cache.h>

#define CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS 1

void _CPU_cache_flush_data_range(const void *d_addr, size_t n_bytes);
void _CPU_cache_invalidate_data_range(const void *d_addr, size_t n_bytes);
void _CPU_cache_invalidate_instruction_range(const void *i_addr, size_t n_bytes);

#endif
/* end of include file */
