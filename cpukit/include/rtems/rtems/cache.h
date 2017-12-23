/**
 * @file
 *
 * @ingroup ClassicCache
 */

/* COPYRIGHT (c) 1989-2013.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_CACHE_H
#define _RTEMS_RTEMS_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/system.h>
#include <sys/types.h>

/**
 * @defgroup ClassicCache Cache
 *
 * @ingroup ClassicRTEMS
 *
 * @brief The Cache Manager provides functions to perform maintenance
 * operations for data and instruction caches.
 *
 * The actual actions of the Cache Manager operations depend on the hardware
 * and the implementation provided by the CPU architecture port or a board
 * support package.  Cache implementations tend to be highly hardware
 * dependent.
 *
 * @{
 */

/**
 * @brief Returns the data cache line size in bytes.
 *
 * For multi-level caches this is the maximum of the cache line sizes of all
 * levels.
 *
 * @retval 0 No data cache is present.
 * @retval positive The data cache line size in bytes.
 */
size_t rtems_cache_get_data_line_size( void );

/**
 * @brief Returns the instruction cache line size in bytes.
 *
 * For multi-level caches this is the maximum of the cache line sizes of all
 * levels.
 *
 * @retval 0 No instruction cache is present.
 * @retval positive The instruction cache line size in bytes.
 */
size_t rtems_cache_get_instruction_line_size( void );

/**
 * @brief Returns the maximal cache line size of all cache kinds in bytes.
 *
 * Returns computed or obtained maximal cache line size of all
 * all caches in the system.
 *
 * @retval 0 No cache is present
 * @retval positive The maximal cache line size in bytes.
 */
size_t rtems_cache_get_maximal_line_size( void );

/**
 * @brief Returns the data cache size in bytes.
 *
 * @param[in] level The cache level of interest.  The cache level zero
 * specifies the entire data cache.
 *
 * @returns The data cache size in bytes of the specified level.
 */
size_t rtems_cache_get_data_cache_size( uint32_t level );

/**
 * @brief Returns the instruction cache size in bytes.
 *
 * @param[in] level The cache level of interest.  The cache level zero
 * specifies the entire instruction cache.
 *
 * @returns The instruction cache size in bytes of the specified level.
 */
size_t rtems_cache_get_instruction_cache_size( uint32_t level );

/**
 * @brief Flushes multiple data cache lines.
 *
 * Dirty cache lines covering the area are transfered to memory.  Depending on
 * the cache implementation this may mark the lines as invalid.
 *
 * @param[in] addr The start address of the area to flush.
 * @param[in] size The size in bytes of the area to flush.
 */
void rtems_cache_flush_multiple_data_lines( const void *addr, size_t size );

/**
 * @brief Invalidates multiple data cache lines.
 *
 * The cache lines covering the area are marked as invalid.  A later read
 * access in the area will load the data from memory.
 *
 * In case the area is not aligned on cache line boundaries, then this
 * operation may destroy unrelated data.
 *
 * @param[in] addr The start address of the area to invalidate.
 * @param[in] size The size in bytes of the area to invalidate.
 */
void rtems_cache_invalidate_multiple_data_lines(
  const void *addr,
  size_t size
);

/**
 * @brief Invalidates multiple instruction cache lines.
 *
 * The cache lines covering the area are marked as invalid.  A later
 * instruction fetch from the area will result in a load from memory.
 * In SMP mode, on processors without instruction cache snooping, this
 * operation will invalidate the instruction cache lines on all processors.
 * It should not be called from interrupt context in such case.
 *
 * @param[in] addr The start address of the area to invalidate.
 * @param[in] size The size in bytes of the area to invalidate.
 */
void rtems_cache_invalidate_multiple_instruction_lines(
  const void *addr,
  size_t size
);


/**
 * @brief Ensure necessary synchronization required after code changes
 *
 * When code is loaded or modified then many Harvard cache equipped
 * systems require synchronization of main memory and or updated
 * code in data cache to ensure visibility of change in all
 * connected CPUs instruction memory view. This operation
 * should be used by run time loader for example.
 *
 * @param[in] addr The start address of the area to invalidate.
 * @param[in] size The size in bytes of the area to invalidate.
 */
void rtems_cache_instruction_sync_after_code_change(
  const void * code_addr,
  size_t n_bytes
);

/**
 * @brief Flushes the entire data cache.
 *
 * @see rtems_cache_flush_multiple_data_lines().
 */
void rtems_cache_flush_entire_data( void );

/**
 * @brief Invalidates the entire instruction cache.
 *
 * @see rtems_cache_invalidate_multiple_instruction_lines().
 */
void rtems_cache_invalidate_entire_instruction( void );

/**
 * This function is responsible for performing a data cache
 * invalidate. It invalidates the entire cache.
 */
void rtems_cache_invalidate_entire_data( void );

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

/**
 * @brief Allocates a memory area of size @a size bytes from cache coherent
 * memory.
 *
 * A size value of zero will return a unique address which may be freed with
 * rtems_cache_coherent_free().
 *
 * The memory allocated by this function can be released with a call to
 * rtems_cache_coherent_free().
 *
 * By default the C program heap allocator is used.  In case special memory
 * areas must be used, then the BSP or the application must add cache coherent
 * memory areas for the allocator via rtems_cache_coherent_add_area().
 *
 * This function must be called from driver initialization or task context
 * only.
 *
 * @param[in] alignment If the alignment parameter is not equal to zero, the
 *   allocated memory area will begin at an address aligned by this value.
 * @param[in] boundary If the boundary parameter is not equal to zero, the
 *   allocated memory area will comply with a boundary constraint.  The
 *   boundary value specifies the set of addresses which are aligned by the
 *   boundary value.  The interior of the allocated memory area will not
 *   contain an element of this set.  The begin or end address of the area may
 *   be a member of the set.
 *
 * @retval NULL If no memory is available or the parameters are inconsistent.
 * @retval other A pointer to the begin of the allocated memory area.
 */
void *rtems_cache_coherent_allocate(
  size_t size,
  uintptr_t alignment,
  uintptr_t boundary
);

/**
 * @brief Frees memory allocated by rtems_cache_coherent_allocate().
 *
 * This function must be called from driver initialization or task context
 * only.
 *
 * @param[in] ptr A pointer returned by rtems_cache_coherent_allocate().
 */
void rtems_cache_coherent_free( void *ptr );

/**
 * @brief Adds a cache coherent memory area to the cache coherent allocator.
 *
 * This function must be called from BSP initialization, driver initialization
 * or task context only.
 *
 * @param[in] area_begin The area begin address.
 * @param[in] area_size The area size in bytes.
 *
 * @see rtems_cache_coherent_allocate().
 */
void rtems_cache_coherent_add_area(
  void *area_begin,
  uintptr_t area_size
);

#if defined( RTEMS_SMP )

/**
 * @brief Flushes multiple data cache lines for a set of processors
 *
 * Dirty cache lines covering the area are transferred to memory.
 * Depending on the cache implementation this may mark the lines as invalid.
 *
 * This operation should not be called from interrupt context.
 *
 * @param[in] addr The start address of the area to flush.
 * @param[in] size The size in bytes of the area to flush.
 * @param[in] setsize The size of the processor set.
 * @param[in] set The target processor set.
 */
void rtems_cache_flush_multiple_data_lines_processor_set(
  const void *addr,
  size_t size,
  const size_t setsize,
  const cpu_set_t *set
);

/**
 * @brief Invalidates multiple data cache lines for a set of processors
 *
 * The cache lines covering the area are marked as invalid.  A later read
 * access in the area will load the data from memory.
 *
 * In case the area is not aligned on cache line boundaries, then this
 * operation may destroy unrelated data.
 *
 * This operation should not be called from interrupt context.
 *
 * @param[in] addr The start address of the area to invalidate.
 * @param[in] size The size in bytes of the area to invalidate.
 * @param[in] setsize The size of the processor set.
 * @param[in] set The target processor set.
 */
void rtems_cache_invalidate_multiple_data_lines_processor_set(
  const void *addr,
  size_t size,
  const size_t setsize,
  const cpu_set_t *set
);

/**
 * @brief Flushes the entire data cache for a set of processors
 *
 * This operation should not be called from interrupt context.
 *
 * @see rtems_cache_flush_multiple_data_lines().
 *
 * @param[in] setsize The size of the processor set.
 * @param[in] set The target processor set.
 */
void rtems_cache_flush_entire_data_processor_set(
  const size_t setsize,
  const cpu_set_t *set
);

/**
 * @brief Invalidates the entire cache for a set of processors
 *
 * This function is responsible for performing a data cache
 * invalidate. It invalidates the entire cache for a set of
 * processors.
 *
 * This operation should not be called from interrupt context.
 *
 * @param[in] setsize The size of the processor set.
 * @param[in] set The target processor set.
 */
void rtems_cache_invalidate_entire_data_processor_set(
  const size_t setsize,
  const cpu_set_t *set
);

#endif

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
