/**
 * @file rtems/rtems/cache.h
 *
 *  Cache Manager
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 *  The functions in this file define the API to the RTEMS Cache Manager and
 *  are divided into data cache and instruction cache functions. Data cache
 *  functions are only meaningful if a data cache is supported. Instruction
 *  cache functions are only meaningful if an instruction cache is supported.
 *
 *  The functions below are implemented with CPU dependent support routines
 *  implemented as part of libcpu. In the event that a CPU does not support a
 *  specific function, the CPU dependent routine does nothing (but does exist).
 *
 *  At this point, the Cache Manager makes no considerations, and provides no
 *  support for BSP specific issues such as a secondary cache. In such a system,
 *  the CPU dependent routines would have to be modified, or a BSP layer added
 *  to this Manager.
 */

#ifndef _RTEMS_RTEMS_CACHE_H
#define _RTEMS_RTEMS_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/system.h>
#include <sys/types.h>

/**
 *  @defgroup ClassicCache Cache
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality which XXX
 */
/**@{*/

/*
 *  These functions will ONLY do something if the
 *  libcpu support includes data cache routines AND
 *  the CPU model supports data caching.
 */

/**
 * This function is called to flush the data cache by performing cache
 * copybacks. It must determine how many cache lines need to be copied
 * back and then perform the copybacks.
 */
void rtems_cache_flush_multiple_data_lines( const void *, size_t );

/**
 * This function is responsible for performing a data cache invalidate.
 * It must determine how many cache lines need to be invalidated and then
 * perform the invalidations.
 */
void rtems_cache_invalidate_multiple_data_lines( const void *, size_t );

/**
 * This function is responsible for performing a data cache flush.
 * It flushes the entire cache.
 */
void rtems_cache_flush_entire_data( void );

/**
 * This function is responsible for performing a data cache
 * invalidate. It invalidates the entire cache.
 */
void rtems_cache_invalidate_entire_data( void );

/**
 * This function returns the data cache granularity.
 */
int rtems_cache_get_data_line_size( void );

/**
 * This function freezes the data cache.
 */
void rtems_cache_freeze_data( void );

/**
 * This function unfreezes the data cache.
 */
void rtems_cache_unfreeze_data( void );

/**
 * This function enables the data cache.
 */
void rtems_cache_enable_data( void );

/**
 * This function disables the data cache.
 */
void rtems_cache_disable_data( void );

/**
 *  These functions will ONLY do something if the
 *  libcpu support includes instruction cache routines AND
 *  the CPU model supports instruction caching.
 */

/**
 * This function is responsible for performing an instruction cache
 * invalidate. It must determine how many cache lines need to be invalidated
 * and then perform the invalidations.
 */
void rtems_cache_invalidate_multiple_instruction_lines( const void *, size_t );

/**
 * This function is responsible for performing an instruction cache
 * invalidate. It invalidates the entire cache.
 */
void rtems_cache_invalidate_entire_instruction( void );

/**
 * This function returns the instruction cache granularity.
 */
int rtems_cache_get_instruction_line_size( void );

/**
 * This function freezes the instruction cache.
 */
void rtems_cache_freeze_instruction( void );

/**
 * This function unfreezes the instruction cache.
 */
void rtems_cache_unfreeze_instruction( void );

/**
 * This function enables the instruction cache.
 */
void rtems_cache_enable_instruction( void );

/**
 * This function disables the instruction cache.
 */
void rtems_cache_disable_instruction( void );

/**
 *  This function is used to allocate storage that spans an
 *  integral number of cache blocks.
 */
void *rtems_cache_aligned_malloc ( size_t nbytes );
  
#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
