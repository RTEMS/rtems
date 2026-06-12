/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreHeap
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreBarrier which are only used by the implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

#ifndef _RTEMS_SCORE_HEAPIMPL_H
#define _RTEMS_SCORE_HEAPIMPL_H

#include <rtems/score/heap.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreHeap
 *
 * @{
 */

/**
 * @brief See _Heap_Resize_block().
 */
typedef enum {
  HEAP_RESIZE_SUCCESSFUL,
  HEAP_RESIZE_UNSATISFIED,
  HEAP_RESIZE_FATAL_ERROR
} Heap_Resize_status;

/**
 * @brief Initializes the heap control block.
 *
 * Blocks of memory are allocated from the heap in multiples of @a page_size
 * byte units.  If the @a page_size is equal to zero or is not multiple of
 * @c CPU_ALIGNMENT, it is aligned up to the nearest @c CPU_ALIGNMENT boundary.
 *
 * @param[out] heap The heap control block to manage the area.
 * @param area_begin The starting address of the area.
 * @param area_size The size of the area in bytes.
 * @param page_size The page size for the calculation
 *
 * @retval some_value The maximum memory available.
 * @retval 0 The initialization failed.
 *
 * @see Heap_Initialization_or_extend_handler.
 */
uintptr_t _Heap_Initialize(
  Heap_Control *heap,
  void         *area_begin,
  uintptr_t     area_size,
  uintptr_t     page_size
);

/**
 * @brief Allocates an aligned memory area with boundary constraint.
 *
 * A size value of zero will return a unique address which may be freed with
 * _Heap_Free().
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
void *_Heap_Allocate_aligned_with_boundary(
  Heap_Control *heap,
  uintptr_t     size,
  uintptr_t     alignment,
  uintptr_t     boundary
);

/**
 * @brief Allocates an aligned memory area.
 *
 * A size value of zero will return a unique address which may be freed with
 * _Heap_Free().
 *
 * @param[in, out] heap The heap to allocate a memory are from.
 * @param size The size of the desired memory are in bytes.
 * @param alignment The allocated memory area will begin at an address aligned by this value.
 *
 * @retval pointer The starting address of the allocated memory area.
 * @retval NULL No memory is available of the parameters are inconsistent.
 */
static inline void *_Heap_Allocate_aligned(
  Heap_Control *heap,
  uintptr_t     size,
  uintptr_t     alignment
)
{
  return _Heap_Allocate_aligned_with_boundary( heap, size, alignment, 0 );
}

/**
 * @brief Allocates a memory area.
 *
 * A size value of zero will return a unique address which may be freed with
 * _Heap_Free().
 *
 * @param[in, out] heap The heap to allocate a memory are from.
 * @param size The size of the desired memory are in bytes.
 *
 * @retval pointer The starting address of the allocated memory area.
 * @retval NULL No memory is available of the parameters are inconsistent.
 */
static inline void *_Heap_Allocate( Heap_Control *heap, uintptr_t size )
{
  return _Heap_Allocate_aligned_with_boundary( heap, size, 0, 0 );
}

/**
 * @brief Frees the allocated memory area.
 *
 * Inappropriate values for @a addr may corrupt the heap.
 *
 * @param[in, out] heap The heap of the allocated memory area.
 * @param addr The starting address of the memory area to be freed.
 *
 * @retval true The allocated memory area was successfully freed.
 * @retval false The method failed.
 */
bool _Heap_Free( Heap_Control *heap, void *addr );

/**
 * @brief Verifies the integrity of the heap.
 *
 * Walks the heap to verify its integrity.
 *
 * @param heap The heap whose integrity is to be verified.
 * @param source If @a dump is @c true, this is used to mark the output lines.
 * @param dump Indicates whether diagnostic messages will be printed to standard output.
 *
 * @retval true No errors occurred, the heap's integrity is not violated.
 * @retval false The heap is corrupt.
 */
bool _Heap_Walk( Heap_Control *heap, int source, bool dump );

/**
 * @brief Heap block visitor.
 *
 * @see _Heap_Iterate().
 *
 * @retval true Stop the iteration.
 * @retval false Continue the iteration.
 */
typedef bool ( *Heap_Block_visitor )(
  const Heap_Block *block,
  uintptr_t         block_size,
  bool              block_is_used,
  void             *visitor_arg
);

/**
 * @brief Iterates over all blocks of the heap.
 *
 * @param[in, out] heap The heap to iterate over.
 * @param visitor This will be called for each heap block with
 *      the argument @a visitor_arg.
 * @param[in, out] visitor_arg The argument for all calls of @a visitor.
 */
void _Heap_Iterate(
  Heap_Control      *heap,
  Heap_Block_visitor visitor,
  void              *visitor_arg
);

/**
 * @brief Greedily allocates and empties the heap.
 *
 * Afterwards, the heap has at most @a block_count allocatable blocks of sizes
 * specified by @a block_sizes.  All other blocks are used.
 *
 * @param[in, out] heap The heap to operate upon
 * @param block_sizes The sizes of the allocatable blocks.  Must point to an
 *      array with @a block_count members.
 * @param block_count The maximum number of allocatable blocks of sizes
 *      specified by @block_sizes.
 *
 * @return Pointer to the first allocated block.
 *
 * @see _Heap_Greedy_free().
 */
Heap_Block *_Heap_Greedy_allocate(
  Heap_Control    *heap,
  const uintptr_t *block_sizes,
  size_t           block_count
);

/**
 * @brief Greedily allocates all blocks except the largest free block.
 *
 * Afterwards the heap has at most one allocatable block.  This block is the
 * largest free block if it exists.  All other blocks are used.
 *
 * @param[in, out] heap The heap to operate upon.
 * @param[out] allocatable_size Stores the size of the largest free block of
 *      the heap after the method call.
 *
 * @return Pointer to the first allocated block.
 *
 * @see _Heap_Greedy_free().
 */
Heap_Block *_Heap_Greedy_allocate_all_except_largest(
  Heap_Control *heap,
  uintptr_t    *allocatable_size
);

/**
 * @brief Frees blocks of a greedy allocation.
 *
 * @param[in, out] heap The heap to operate upon.
 * @param blocks Must be the return value of _Heap_Greedy_allocate().
 */
void _Heap_Greedy_free( Heap_Control *heap, Heap_Block *blocks );

/**
 * @brief Returns information about used and free blocks for the heap.
 *
 * @param heap The heap to get the information from.
 * @param[out] info Stores the information of the @a heap after the method call.
 */
void _Heap_Get_information( Heap_Control *heap, Heap_Information_block *info );

/**
 * @brief Returns information about free blocks for the heap.
 *
 * @param heap The heap to get the information from.
 * @param[out] info Stores the information about free blocks of @a heap after the
 *      method call.
 */
void _Heap_Get_free_information( Heap_Control *heap, Heap_Information *info );

/**
 * @brief Returns the size of the allocatable memory area.
 *
 * The size value may be greater than the initially requested size in
 * _Heap_Allocate_aligned_with_boundary().
 *
 * Inappropriate values for @a addr will not corrupt the heap, but may yield
 * invalid size values.
 *
 * @param heap The heap to operate upon.
 * @param addr The starting address of the allocatable memory area.
 * @param[out] size Stores the size of the allocatable memory area after the method call.
 *
 * @retval true The operation was successful.
 * @retval false The operation was not successful.
 */
bool _Heap_Size_of_alloc_area(
  Heap_Control *heap,
  void         *addr,
  uintptr_t    *size
);

/**
 * @brief Resizes the block of the allocated memory area.
 *
 * Inappropriate values for @a addr may corrupt the heap.
 *
 * @param[in, out] heap The heap to operate upon.
 * @param addr The starting address of the allocated memory area to be resized.
 * @param size The least possible size for the new memory area.  Resize may be
 *      impossible and depends on the current heap usage.
 * @param[out] old_size Stores the size available for allocation in the current
 *      block before the resize after the method call.
 * @param[out] new_size Stores the size available for allocation in the resized
 *      block after the method call.  In the case of an unsuccessful resize,
 *      zero is returned in this parameter
 *
 * @retval HEAP_RESIZE_SUCCESSFUL The resize was successful.
 * @retval HEAP_RESIZE_UNSATISFIED The least possible size @a size was too big.
 *      Resize not possible.
 * @retval HEAP_RESIZE_FATAL_ERROR The block starting at @a addr is not part of
 *      the heap.
 */
Heap_Resize_status _Heap_Resize_block(
  Heap_Control *heap,
  void         *addr,
  uintptr_t     size,
  uintptr_t    *old_size,
  uintptr_t    *new_size
);

/**
 * @brief Checks if the value is aligned to the given alignment.
 *
 * @param value The value to check the alignment of.
 * @param alignment The alignment for the operation.
 *
 * @retval true The value is aligned to the given alignment.
 * @retval false The value is not aligned to the given alignment.
 */
static inline bool _Heap_Is_aligned( uintptr_t value, uintptr_t alignment )
{
  return ( value % alignment ) == 0;
}

/**
 * @brief Returns the aligned value, truncating.
 *
 * @param value The value to be aligned
 * @param alignment The alignment for the operation.
 *
 * @return The aligned value, truncated.
 */
static inline uintptr_t _Heap_Align_down(
  uintptr_t value,
  uintptr_t alignment
)
{
  return value - ( value % alignment );
}

/**
 * @brief Returns the size of the allocatable area in bytes.
 *
 * This value is an integral multiple of the page size.
 *
 * @param heap The heap to get the allocatable area from.
 *
 * @return The size of the allocatable area in @a heap in bytes.
 */
static inline uintptr_t _Heap_Get_size( const Heap_Control *heap )
{
  return heap->stats.size;
}

/**
 * @brief Returns the bigger one of the two arguments.
 *
 * @param a The parameter on the left hand side of the comparison.
 * @param b The parameter on the right hand side of the comparison.
 *
 * @retval a If a > b.
 * @retval b If b >= a
 */
static inline uintptr_t _Heap_Max( uintptr_t a, uintptr_t b )
{
  return a > b ? a : b;
}

/**
 * @brief Returns the smaller one of the two arguments.
 *
 * @param a The parameter on the left hand side of the comparison.
 * @param b The parameter on the right hand side of the comparison.
 *
 * @retval a If a < b.
 * @retval b If b <= a
 */
static inline uintptr_t _Heap_Min( uintptr_t a, uintptr_t b )
{
  return a < b ? a : b;
}

#ifdef RTEMS_DEBUG
  #define RTEMS_HEAP_DEBUG
#endif

#ifdef RTEMS_HEAP_DEBUG
  #include <assert.h>
  #define _HAssert( cond )                   \
  do {                                       \
    if ( !( cond ) ) {                       \
      __assert( __FILE__, __LINE__, #cond ); \
    }                                        \
  } while ( 0 )
#else
  #define _HAssert( cond ) ( (void) 0 )
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
