/*
 *  libcpu Cache Manager Support
 *
 *  $Id$
 */

#ifndef __LIBCPU_CACHE_h
#define __LIBCPU_CACHE_h

#include <sys/types.h>

void _CPU_cache_flush_1_data_line(const void *d_addr);
void _CPU_cache_invalidate_1_data_line(const void *d_addr);
void _CPU_cache_freeze_data(void);
void _CPU_cache_unfreeze_data(void);
void _CPU_cache_invalidate_1_instruction_line(const void *d_addr);
void _CPU_cache_freeze_instruction(void);
void _CPU_cache_unfreeze_instruction(void);
void _CPU_cache_flush_entire_data(void);
void _CPU_cache_invalidate_entire_data(void);
void _CPU_cache_enable_data(void);
void _CPU_cache_disable_data(void);
void _CPU_cache_invalidate_entire_instruction(void);
void _CPU_cache_enable_instruction(void);
void _CPU_cache_disable_instruction(void);

#endif
/* end of include file */
