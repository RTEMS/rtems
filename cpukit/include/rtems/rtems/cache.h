/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicCache
 *
 * @brief This header file defines the Cache Manager API.
 */

/*
 * Copyright (C) 2016 Pavel Pisa
 * Copyright (C) 2014, 2021 embedded brains GmbH & Co. KG
 * Copyright (C) 2000, 2008 On-Line Applications Research Corporation (OAR)
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/rtems/cache/if/header */

#ifndef _RTEMS_RTEMS_CACHE_H
#define _RTEMS_RTEMS_CACHE_H

#include <stddef.h>
#include <stdint.h>
#include <rtems/rtems/status.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/cache/if/group */

/**
 * @defgroup RTEMSAPIClassicCache Cache Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Cache Manager provides functions to perform maintenance
 *   operations for data and instruction caches.
 *
 * The actual actions of the Cache Manager operations depend on the hardware
 * and the implementation provided by the CPU architecture port or a board
 * support package.  Cache implementations tend to be highly hardware
 * dependent.
 */

/* Generated from spec:/rtems/cache/if/coherent-add-area */

/**
 * @brief Adds a cache coherent memory area to the cache coherent allocator.
 *
 * @param begin is the begin address of the cache coherent memory area to add.
 *
 * @param size is the size in bytes of the cache coherent memory area to add.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_UNSATISFIED The requested operation was not successful.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_cache_coherent_add_area( void *begin, uintptr_t size );

/* Generated from spec:/rtems/cache/if/coherent-allocate */

/**
 * @brief Allocates a memory area from cache coherent memory.
 *
 * @param size is the requested size in bytes of the memory area to allocate.
 *
 * @param alignment is the requested alignment in bytes of the memory area to
 *   allocate.  If the alignment parameter is not equal to zero, the allocated
 *   memory area will begin at an address aligned by this value.
 *
 * @param boundary is the requested boundary in bytes of the memory area to
 *   allocate.  If the boundary parameter is not equal to zero, the allocated
 *   memory area will comply with a boundary constraint.  The boundary value
 *   specifies the set of addresses which are aligned by the boundary value.
 *   The interior of the allocated memory area will not contain an element of
 *   this set.  The begin or end address of the area may be a member of the
 *   set.
 *
 * @retval NULL There is not enough memory available to satisfy the allocation
 *   request.
 *
 * @return Returns the begin address of the allocated memory.
 *
 * @par Notes
 * @parblock
 * A size value of zero will return a unique address which may be freed with
 * rtems_cache_coherent_free().
 *
 * The memory allocated by the directive may be released with a call to
 * rtems_cache_coherent_free().
 *
 * By default the C Program Heap allocator is used.  In case special memory
 * areas must be used, then the BSP or the application should add cache
 * coherent memory areas for the allocator via rtems_cache_coherent_add_area().
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
void *rtems_cache_coherent_allocate(
  size_t    size,
  uintptr_t alignment,
  uintptr_t boundary
);

/* Generated from spec:/rtems/cache/if/coherent-free */

/**
 * @brief Frees memory allocated by rtems_cache_coherent_allocate().
 *
 * @param ptr is a pointer returned by rtems_cache_coherent_allocate().
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cache_coherent_free( void *ptr );

/* Generated from spec:/rtems/cache/if/freeze-data */

/**
 * @brief Freezes the data caches.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cache_freeze_data( void );

/* Generated from spec:/rtems/cache/if/freeze-instruction */

/**
 * @brief Freezes the instruction caches.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cache_freeze_instruction( void );

/* Generated from spec:/rtems/cache/if/unfreeze-data */

/**
 * @brief Unfreezes the data cache.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cache_unfreeze_data( void );

/* Generated from spec:/rtems/cache/if/unfreeze-instruction */

/**
 * @brief Unfreezes the instruction cache.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cache_unfreeze_instruction( void );

/* Generated from spec:/rtems/cache/if/flush-multiple-data-lines */

/**
 * @ingroup RTEMSAPIClassicCache
 *
 * @brief Flushes the data cache lines covering the memory area.
 *
 * @param begin is the begin address of the memory area to flush.
 *
 * @param size is the size in bytes of the memory area to flush.
 *
 * Dirty data cache lines covering the area are transfered to memory.
 * Depending on the cache implementation this may mark the lines as invalid.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cache_flush_multiple_data_lines( const void *begin, size_t size );

/* Generated from spec:/rtems/cache/if/invalidate-multiple-data-lines */

/**
 * @ingroup RTEMSAPIClassicCache
 *
 * @brief Invalidates the data cache lines covering the memory area.
 *
 * @param begin is the begin address of the memory area to invalidate.
 *
 * @param size is the size in bytes of the memory area to invalidate.
 *
 * The cache lines covering the area are marked as invalid.  A later read
 * access in the area will load the data from memory.
 *
 * @par Notes
 * @parblock
 * In case the area is not aligned on cache line boundaries, then this
 * operation may destroy unrelated data.
 *
 * On some systems, the cache lines may be flushed before they are invalidated.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cache_invalidate_multiple_data_lines(
  const void *begin,
  size_t      size
);

/* Generated from spec:/rtems/cache/if/invalidate-multiple-instruction-lines */

/**
 * @ingroup RTEMSAPIClassicCache
 *
 * @brief Invalidates the instruction cache lines covering the memory area.
 *
 * @param begin is the begin address of the memory area to invalidate.
 *
 * @param size is the size in bytes of the memory area to invalidate.
 *
 * The cache lines covering the area are marked as invalid.  A later
 * instruction fetch from the area will result in a load from memory.
 *
 * @par Notes
 * In SMP configurations, on processors without instruction cache snooping,
 * this operation will invalidate the instruction cache lines on all
 * processors.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cache_invalidate_multiple_instruction_lines(
  const void *begin,
  size_t      size
);

/* Generated from spec:/rtems/cache/if/instruction-sync-after-code-change */

/**
 * @ingroup RTEMSAPIClassicCache
 *
 * @brief Ensures necessary synchronization required after code changes.
 *
 * @param begin is the begin address of the code area to synchronize.
 *
 * @param size is the size in bytes of the code area to synchronize.
 *
 * @par Notes
 * When code is loaded or modified, then most systems require synchronization
 * instructions to update the instruction caches so that the loaded or modified
 * code is fetched.  For example, systems with separate data and instruction
 * caches or systems without instruction cache snooping.  The directives should
 * be used by run time loader for example.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cache_instruction_sync_after_code_change(
  const void *begin,
  size_t      size
);

/* Generated from spec:/rtems/cache/if/get-maximal-line-size */

/**
 * @ingroup RTEMSAPIClassicCache
 *
 * @brief Gets the maximal cache line size in bytes of all caches (data,
 *   instruction, or unified).
 *
 * @retval 0 There is no cache present.
 *
 * @return Returns the maximal cache line size in bytes of all caches (data,
 *   instruction, or unified).
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
size_t rtems_cache_get_maximal_line_size( void );

/* Generated from spec:/rtems/cache/if/get-data-line-size */

/**
 * @ingroup RTEMSAPIClassicCache
 *
 * @brief Gets the data cache line size in bytes.
 *
 * @retval 0 There is no data cache present.
 *
 * @return Returns the data cache line size in bytes.  For multi-level caches
 *   this is the maximum of the cache line sizes of all levels.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
size_t rtems_cache_get_data_line_size( void );

/* Generated from spec:/rtems/cache/if/get-instruction-line-size */

/**
 * @ingroup RTEMSAPIClassicCache
 *
 * @brief Gets the instruction cache line size in bytes.
 *
 * @retval 0 There is no instruction cache present.
 *
 * @return Returns the instruction cache line size in bytes.  For multi-level
 *   caches this is the maximum of the cache line sizes of all levels.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
size_t rtems_cache_get_instruction_line_size( void );

/* Generated from spec:/rtems/cache/if/get-data-size */

/**
 * @ingroup RTEMSAPIClassicCache
 *
 * @brief Gets the data cache size in bytes for the cache level.
 *
 * @param level is the requested data cache level.  The cache level zero
 *   specifies the entire data cache.
 *
 * @retval 0 There is no data cache present at the requested cache level.
 *
 * @return Returns the data cache size in bytes of the requested cache level.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
size_t rtems_cache_get_data_cache_size( uint32_t level );

/* Generated from spec:/rtems/cache/if/get-instruction-size */

/**
 * @ingroup RTEMSAPIClassicCache
 *
 * @brief Gets the instruction cache size in bytes for the cache level.
 *
 * @param level is the requested instruction cache level.  The cache level zero
 *   specifies the entire instruction cache.
 *
 * @retval 0 There is no instruction cache present at the requested cache
 *   level.
 *
 * @return Returns the instruction cache size in bytes of the requested cache
 *   level.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
size_t rtems_cache_get_instruction_cache_size( uint32_t level );

/* Generated from spec:/rtems/cache/if/flush-entire-data */

/**
 * @ingroup RTEMSAPIClassicCache
 *
 * @brief Flushes the entire data cache.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cache_flush_entire_data( void );

/* Generated from spec:/rtems/cache/if/invalidate-entire-data */

/**
 * @ingroup RTEMSAPIClassicCache
 *
 * @brief Invalidates the entire data cache.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cache_invalidate_entire_data( void );

/* Generated from spec:/rtems/cache/if/invalidate-entire-instruction */

/**
 * @ingroup RTEMSAPIClassicCache
 *
 * @brief Invalidates the entire instruction cache.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cache_invalidate_entire_instruction( void );

/* Generated from spec:/rtems/cache/if/enable-data */

/**
 * @ingroup RTEMSAPIClassicCache
 *
 * @brief Enables the data cache.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cache_enable_data( void );

/* Generated from spec:/rtems/cache/if/disable-data */

/**
 * @ingroup RTEMSAPIClassicCache
 *
 * @brief Disables the data cache.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cache_disable_data( void );

/* Generated from spec:/rtems/cache/if/enable-instruction */

/**
 * @ingroup RTEMSAPIClassicCache
 *
 * @brief Enables the instruction cache.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cache_enable_instruction( void );

/* Generated from spec:/rtems/cache/if/disable-instruction */

/**
 * @ingroup RTEMSAPIClassicCache
 *
 * @brief Disables the instruction cache.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cache_disable_instruction( void );

/* Generated from spec:/rtems/cache/if/aligned-malloc */

/**
 * @ingroup RTEMSAPIClassicCache
 *
 * @brief Allocates memory from the C Program Heap which begins at a cache line
 *   boundary.
 *
 * @param size is the size in bytes of the memory area to allocate.
 *
 * @retval NULL There is not enough memory available to satisfy the allocation
 *   request.
 *
 * @return Returns the begin address of the allocated memory.  The begin
 *   address is on a cache line boundary.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
void *rtems_cache_aligned_malloc( size_t size );

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_CACHE_H */
