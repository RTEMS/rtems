/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreHeap
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreBarrier which are used by the implementation and the API.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
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
