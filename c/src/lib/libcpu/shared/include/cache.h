/*
 *  libcpu Cache Manager Support
 *
 *  $Id$
 */

#ifndef __LIBCPU_CACHE_h
#define __LIBCPU_CACHE_h

#include <sys/types.h>

void _CPU_disable_cache();
void _CPU_enable_cache();

void _CPU_flush_1_data_cache_line(const void *d_addr);
void _CPU_invalidate_1_data_cache_line(const void *d_addr);
void _CPU_freeze_data_cache(void);
void _CPU_unfreeze_data_cache(void);
void _CPU_invalidate_1_inst_cache_line(const void *d_addr);
void _CPU_freeze_inst_cache(void);
void _CPU_unfreeze_inst_cache(void);

void _CPU_flush_entire_data_cache(void);
void _CPU_invalidate_entire_data_cache(void);
void _CPU_enable_data_cache(void);
void _CPU_disable_data_cache(void);
void _CPU_invalidate_entire_inst_cache(void);
void _CPU_enable_inst_cache(void);
void _CPU_disable_inst_cache(void);

#endif
/* end of include file */
