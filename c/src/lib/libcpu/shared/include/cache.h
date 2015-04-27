/*
 *  libcpu Cache Manager Support
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  The functions declared in this file are implemented for
 *  each processor in the cache.c file under libcpu/CPU/.
 *  They provide the processor specific actions to take for
 *  implementing most of the RTEMS Cache Manager directives,
 *  and should only ever be called by these directives.
 *
 *  The API for the RTEMS Cache Manager can be found in
 *  c/src/exec/rtems/include/rtems/rtems/cache.h
 */

#ifndef __LIBCPU_CACHE_h
#define __LIBCPU_CACHE_h

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
