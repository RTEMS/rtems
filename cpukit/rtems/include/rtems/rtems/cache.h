/*  cache.h
 *
 *  Cache Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *
 *  The functions in this file define the API to the RTEMS Cache Manager and
 *  are divided into data cache and instruction cache functions. Data cache
 *  functions are only declared if a data cache is supported. Instruction
 *  cache functions are only declared if an instruction cache is supported.
 *  Support for a particular cache exists only if _CPU_x_CACHE_ALIGNMENT is
 *  defined, where x E {DATA, INST}. These definitions are found in the CPU
 *  dependent source files in the supercore, often
 *  
 *  rtems/c/src/exec/score/cpu/CPU/rtems/score/CPU.h
 *  
 *  The functions below are implemented with CPU dependent inline routines
 *  also found in the above file. In the event that a CPU does not support a
 *  specific function, the CPU dependent routine does nothing (but does exist).
 *  
 *  At this point, the Cache Manager makes no considerations, and provides no
 *  support for BSP specific issues such as a secondary cache. In such a system,
 *  the CPU dependent routines would have to be modified, or a BSP layer added
 *  to this Manager.
 */

#ifndef __CACHE_h
#define __CACHE_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/system.h>
#include <sys/types.h>


/* THESE FUNCTIONS ONLY EXIST IF WE HAVE A DATA CACHE */
#if defined(_CPU_DATA_CACHE_ALIGNMENT)

/*
 * This function is called to flush the data cache by performing cache
 * copybacks. It must determine how many cache lines need to be copied
 * back and then perform the copybacks.
 */
void rtems_flush_multiple_data_cache_lines( const void *, size_t );

/*
 * This function is responsible for performing a data cache invalidate.
 * It must determine how many cache lines need to be invalidated and then
 * perform the invalidations.
 */
void rtems_invalidate_multiple_data_cache_lines( const void *, size_t );

/*
 * This function is responsible for performing a data cache flush.
 * It flushes the entire cache.
 */
void rtems_flush_entire_data_cache( void );

/*
 * This function is responsible for performing a data cache
 * invalidate. It invalidates the entire cache.
 */
void rtems_invalidate_entire_data_cache( void );

/*
 * This function returns the data cache granularity.
 */
int rtems_get_data_cache_line_size( void );

/*
 * This function freezes the data cache.
 */
void rtems_freeze_data_cache( void );

/*
 * This function unfreezes the data cache.
 */
void rtems_unfreeze_data_cache( void );

/*
 * These functions enable/disable the data cache.
 */
void rtems_enable_data_cache( void );
void rtems_disable_data_cache( void );
#endif


/* THESE FUNCTIONS ONLY EXIST IF WE HAVE AN INSTRUCTION CACHE */
#if defined(_CPU_INST_CACHE_ALIGNMENT)

/*
 * This function is responsible for performing an instruction cache
 * invalidate. It must determine how many cache lines need to be invalidated
 * and then perform the invalidations.
 */
void rtems_invalidate_multiple_inst_cache_lines( const void *, size_t );

/*
 * This function is responsible for performing an instruction cache
 * invalidate. It invalidates the entire cache.
 */
void rtems_invalidate_entire_inst_cache( void );

/*
 * This function returns the instruction cache granularity.
 */
int rtems_get_inst_cache_line_size( void );

/*
 * This function freezes the instruction cache.
 */
void rtems_freeze_inst_cache( void );

/*
 * This function unfreezes the instruction cache.
 */
void rtems_unfreeze_inst_cache( void );

/*
 * These functions enable/disable the instruction cache.
 */
void rtems_enable_inst_cache( void );
void rtems_disable_inst_cache( void );
#endif


#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
