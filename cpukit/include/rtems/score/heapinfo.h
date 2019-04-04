/**
 * @file
 *
 * @ingroup RTEMSScoreHeap
 *
 * @brief Heap Handler Information API
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_HEAPINFO_H
#define _RTEMS_SCORE_HEAPINFO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreHeap
 *
 * @{
 */

/**
 * @brief Run-time heap statistics.
 *
 * The value @a searches / @a allocs gives the mean number of searches per
 * allocation, while @a max_search gives maximum number of searches ever
 * performed on a single allocation call.
 */
typedef struct {
  /**
   * @brief Lifetime number of bytes allocated from this heap.
   *
   * This value is an integral multiple of the page size.
   */
  uint64_t lifetime_allocated;

  /**
   * @brief Lifetime number of bytes freed to this heap.
   *
   * This value is an integral multiple of the page size.
   */
  uint64_t lifetime_freed;

  /**
   * @brief Size of the allocatable area in bytes.
   *
   * This value is an integral multiple of the page size.
   */
  uintptr_t size;

  /**
   * @brief Current free size in bytes.
   *
   * This value is an integral multiple of the page size.
   */
  uintptr_t free_size;

  /**
   * @brief Minimum free size ever in bytes.
   *
   * This value is an integral multiple of the page size.
   */
  uintptr_t min_free_size;

  /**
   * @brief Current number of free blocks.
   */
  uint32_t free_blocks;

  /**
   * @brief Maximum number of free blocks ever.
   */
  uint32_t max_free_blocks;

  /**
   * @brief Current number of used blocks.
   */
  uint32_t used_blocks;

  /**
   * @brief Maximum number of blocks searched ever.
   */
  uint32_t max_search;

  /**
   * @brief Total number of searches.
   */
  uint32_t searches;

  /**
   * @brief Total number of successful allocations.
   */
  uint32_t allocs;

  /**
   * @brief Total number of failed allocations.
   */
  uint32_t failed_allocs;

  /**
   * @brief Total number of successful frees.
   */
  uint32_t frees;

  /**
   * @brief Total number of successful resizes.
   */
  uint32_t resizes;
} Heap_Statistics;

/**
 * @brief Information about blocks.
 */
typedef struct {
  /**
   * @brief Number of blocks of this type.
   */
  uintptr_t number;

  /**
   * @brief Largest block of this type.
   */
  uintptr_t largest;

  /**
   * @brief Total size of the blocks of this type.
   */
  uintptr_t total;
} Heap_Information;

/**
 * @brief Information block returned by _Heap_Get_information().
 */
typedef struct {
  Heap_Information Free;
  Heap_Information Used;
  Heap_Statistics Stats;
} Heap_Information_block;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
