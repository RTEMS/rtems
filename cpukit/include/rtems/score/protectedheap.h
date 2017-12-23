/**
 * @file
 *
 * @ingroup ScoreProtHeap
 *
 * @brief Protected Heap Handler API
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_PROTECTED_HEAP_H
#define _RTEMS_SCORE_PROTECTED_HEAP_H

#include <rtems/score/heapimpl.h>
#include <rtems/score/apimutex.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ScoreProtHeap Protected Heap Handler
 *
 * @ingroup ScoreHeap
 *
 * @brief Provides protected heap services.
 *
 * The @ref ScoreAllocatorMutex is used to protect the heap accesses.
 *
 */
/**@{**/

/**
 * @brief See _Heap_Initialize().
 */
RTEMS_INLINE_ROUTINE uintptr_t _Protected_heap_Initialize(
  Heap_Control *heap,
  void *area_begin,
  uintptr_t area_size,
  uintptr_t page_size
)
{
  return _Heap_Initialize( heap, area_begin, area_size, page_size );
}

/**
 * @brief See _Heap_Extend().
 *
 * Returns @a true in case of success, and @a false otherwise.
 */
bool _Protected_heap_Extend(
  Heap_Control *heap,
  void *area_begin,
  uintptr_t area_size
);

/**
 * @brief See _Heap_Allocate_aligned_with_boundary().
 */
void *_Protected_heap_Allocate_aligned_with_boundary(
  Heap_Control *heap,
  uintptr_t size,
  uintptr_t alignment,
  uintptr_t boundary
);

/**
 * @brief See _Heap_Allocate_aligned_with_boundary() with boundary equals zero.
 */
RTEMS_INLINE_ROUTINE void *_Protected_heap_Allocate_aligned(
  Heap_Control *heap,
  uintptr_t size,
  uintptr_t alignment
)
{
  return
    _Protected_heap_Allocate_aligned_with_boundary( heap, size, alignment, 0 );
}

/**
 * @brief See _Heap_Allocate_aligned_with_boundary() with alignment and
 * boundary equals zero.
 */
RTEMS_INLINE_ROUTINE void *_Protected_heap_Allocate(
  Heap_Control *heap,
  uintptr_t size
)
{
  return _Protected_heap_Allocate_aligned_with_boundary( heap, size, 0, 0 );
}

/**
 * @brief See _Heap_Size_of_alloc_area().
 */
bool _Protected_heap_Get_block_size(
  Heap_Control *heap,
  void *addr,
  uintptr_t *size
);

/**
 * @brief See _Heap_Resize_block().
 *
 * Returns @a true in case of success, and @a false otherwise.
 */
bool _Protected_heap_Resize_block(
  Heap_Control *heap,
  void *addr,
  uintptr_t size
);

/**
 * @brief See _Heap_Free().
 *
 * Returns @a true in case of success, and @a false otherwise.
 */
bool _Protected_heap_Free( Heap_Control *heap, void *addr );

/**
 * @brief See _Heap_Walk().
 */
bool _Protected_heap_Walk( Heap_Control *heap, int source, bool dump );

/**
 * @brief See _Heap_Iterate().
 */
void _Protected_heap_Iterate(
  Heap_Control *heap,
  Heap_Block_visitor visitor,
  void *visitor_arg
);

/**
 * @brief See _Heap_Get_information().
 *
 * Returns @a true in case of success, and @a false otherwise.
 */
bool _Protected_heap_Get_information(
  Heap_Control *heap,
  Heap_Information_block *info
);

/**
 * @brief See _Heap_Get_free_information().
 *
 * Returns @a true in case of success, and @a false otherwise.
 */
bool _Protected_heap_Get_free_information(
  Heap_Control *heap,
  Heap_Information *info
);

/**
 * @brief See _Heap_Get_size().
 */
uintptr_t _Protected_heap_Get_size( Heap_Control *heap );

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
