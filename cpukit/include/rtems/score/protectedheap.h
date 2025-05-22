/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreProtHeap
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreProtHeap.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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

#ifndef _RTEMS_SCORE_PROTECTED_HEAP_H
#define _RTEMS_SCORE_PROTECTED_HEAP_H

#include <rtems/score/heapimpl.h>
#include <rtems/score/apimutex.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreProtHeap Protected Heap Handler
 *
 * @ingroup RTEMSScoreHeap
 *
 * @brief This group contains the Protected Heap Handler implementation.
 *
 * The @ref ScoreAllocatorMutex is used to protect the heap accesses.
 *
 * @{
 */

/**
 * @brief Initializes the protected heap.
 *
 * @param[out] heap The heap to initialize.
 * @param area_begin The starting address of the heap area.
 * @param area_size The size of the heap area.
 * @param page_size The page size for the heap.
 */
static inline uintptr_t _Protected_heap_Initialize(
  Heap_Control *heap,
  void *area_begin,
  uintptr_t area_size,
  uintptr_t page_size
)
{
  return _Heap_Initialize( heap, area_begin, area_size, page_size );
}

/**
 * @brief Extends the protected heap.
 *
 * @param[in, out] heap The heap to extend.
 * @param area_begin The starting addres of the area to extend @a heap with.
 * @param area_size The size of the heap area.
 *
 * @retval true The operation succeeded.
 * @retval false The operation failed.
 */
bool _Protected_heap_Extend(
  Heap_Control *heap,
  void *area_begin,
  uintptr_t area_size
);

/**
 * @brief Allocates an aligned memory area with boundary constraint for the protected heap.
 *
 * A size value of zero will return a unique address which may be freed with
 * _Heap_Free().  This method first locks the allocator and after the allocation of the memory
 * area, unlocks it again.
 *
 * @param[in, out] heap The heap to allocate a memory are from.
 * @param size The size of the desired memory are in bytes.
 * @param alignment The allocated memory area will begin at an address aligned by this value.
 * @param boundary The allocated memory area will fulfill a boundary constraint,
 *      if this value is not equal to zero.  The boundary value specifies
 *      the set of addresses which are aligned by the boundary value.  The
 *      interior of the allocated memory area will not contain an element of this
 *      set.  The begin or end address of the area may be a member of the set.
 *
 * @retval pointer The starting address of the allocated memory area.
 * @retval NULL No memory is available of the parameters are inconsistent.
 */
void *_Protected_heap_Allocate_aligned_with_boundary(
  Heap_Control *heap,
  uintptr_t size,
  uintptr_t alignment,
  uintptr_t boundary
);

/**
 * @brief Allocates an aligned memory area.
 *
 * A size value of zero will return a unique address which may be freed with
 * _Heap_Free().  This method first locks the allocator and after the allocation of the memory
 * area, unlocks it again.
 *
 * @param[in, out] heap The heap to allocate a memory are from.
 * @param size The size of the desired memory are in bytes.
 * @param alignment The allocated memory area will begin at an address aligned by this value.
 *
 * @retval pointer The starting address of the allocated memory area.
 * @retval NULL No memory is available of the parameters are inconsistent.
 */
static inline void *_Protected_heap_Allocate_aligned(
  Heap_Control *heap,
  uintptr_t size,
  uintptr_t alignment
)
{
  return
    _Protected_heap_Allocate_aligned_with_boundary( heap, size, alignment, 0 );
}

/**
 * @brief Allocates a memory area.
 *
 * A size value of zero will return a unique address which may be freed with
 * _Heap_Free().  This method first locks the allocator and after the allocation of the memory
 * area, unlocks it again.
 *
 * @param[in, out] heap The heap to allocate a memory are from.
 * @param size The size of the desired memory are in bytes.
 *
 * @retval pointer The starting address of the allocated memory area.
 * @retval NULL No memory is available of the parameters are inconsistent.
 */
static inline void *_Protected_heap_Allocate(
  Heap_Control *heap,
  uintptr_t size
)
{
  return _Protected_heap_Allocate_aligned_with_boundary( heap, size, 0, 0 );
}

/**
 * @brief Frees the allocated memory area.
 *
 * Inappropriate values for @a addr may corrupt the heap.  This method first locks
 * the allocator and after the free operation, unlocks it again.
 *
 * @param[in, out] heap The heap of the allocated memory area.
 * @param addr The starting address of the memory area to be freed.
 *
 * @retval true The allocated memory area was successfully freed.
 * @retval false The method failed.
 */
bool _Protected_heap_Free( Heap_Control *heap, void *addr );

/**
 * @brief Verifies the integrity of the heap.
 *
 * Walks the heap to verify its integrity.  This method first locks
 * the allocator and after the operation, unlocks it again, if the thread dispatch is enabled.
 *
 * @param heap The heap whose integrity is to be verified.
 * @param source If @a dump is @c true, this is used to mark the output lines.
 * @param dump Indicates whether diagnostic messages will be printed to standard output.
 *
 * @retval true No errors occurred, the heap's integrity is not violated.
 * @retval false The heap is corrupt.
 */
bool _Protected_heap_Walk( Heap_Control *heap, int source, bool dump );

/**
 * @brief Returns information about used and free blocks for the heap.
 *
 * This method first locks the allocator and after the operation, unlocks it again.
 *
 * @param heap The heap to get the information from.
 * @param[out] info Stores the information of the @a heap after the method call.
 */
bool _Protected_heap_Get_information(
  Heap_Control *heap,
  Heap_Information_block *info
);

/**
 * @brief Returns information about free blocks for the heap.
 *
 * This method first locks the allocator and after the operation, unlocks it again.
 *
 * @param heap The heap to get the information from.
 * @param[out] info Stores the information about free blocks of @a heap after the
 *      method call.
 */
bool _Protected_heap_Get_free_information(
  Heap_Control *heap,
  Heap_Information *info
);

/**
 * @brief Returns the size of the allocatable area in bytes.
 *
 * This value is an integral multiple of the page size.
 *
 * @param heap The heap to get the allocatable area from.
 *
 * @return The size of the allocatable area in @a heap in bytes.
 */
uintptr_t _Protected_heap_Get_size( Heap_Control *heap );

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
