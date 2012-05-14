/**
 * @file
 *
 * @ingroup ScoreHeap
 *
 * @brief Heap Handler API.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_HEAP_H
# error "Never use <rtems/score/heap.inl> directly; include <rtems/score/heap.h> instead."
#endif

#ifndef _RTEMS_SCORE_HEAP_INL
#define _RTEMS_SCORE_HEAP_INL

#include <rtems/score/address.h>

/**
 * @addtogroup ScoreHeap
 *
 * @{
 */

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Free_list_head( Heap_Control *heap )
{
  return &heap->free_list;
}

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Free_list_tail( Heap_Control *heap )
{
  return &heap->free_list;
}

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Free_list_first( Heap_Control *heap )
{
  return _Heap_Free_list_head(heap)->next;
}

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Free_list_last( Heap_Control *heap )
{
  return _Heap_Free_list_tail(heap)->prev;
}

RTEMS_INLINE_ROUTINE void _Heap_Free_list_remove( Heap_Block *block )
{
  Heap_Block *next = block->next;
  Heap_Block *prev = block->prev;

  prev->next = next;
  next->prev = prev;
}

RTEMS_INLINE_ROUTINE void _Heap_Free_list_replace(
  Heap_Block *old_block,
  Heap_Block *new_block
)
{
  Heap_Block *next = old_block->next;
  Heap_Block *prev = old_block->prev;

  new_block->next = next;
  new_block->prev = prev;

  next->prev = new_block;
  prev->next = new_block;
}

RTEMS_INLINE_ROUTINE void _Heap_Free_list_insert_after(
  Heap_Block *block_before,
  Heap_Block *new_block
)
{
  Heap_Block *next = block_before->next;

  new_block->next = next;
  new_block->prev = block_before;
  block_before->next = new_block;
  next->prev = new_block;
}

RTEMS_INLINE_ROUTINE bool _Heap_Is_aligned(
  uintptr_t value,
  uintptr_t alignment
)
{
  return (value % alignment) == 0;
}

RTEMS_INLINE_ROUTINE uintptr_t _Heap_Align_up(
  uintptr_t value,
  uintptr_t alignment
)
{
  uintptr_t remainder = value % alignment;

  if ( remainder != 0 ) {
    return value - remainder + alignment;
  } else {
    return value;
  }
}

RTEMS_INLINE_ROUTINE uintptr_t _Heap_Align_down(
  uintptr_t value,
  uintptr_t alignment
)
{
  return value - (value % alignment);
}

/**
 * @brief Returns the block which is @a offset away from @a block.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Block_at(
  const Heap_Block *block,
  uintptr_t offset
)
{
  return (Heap_Block *) ((uintptr_t) block + offset);
}

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Prev_block(
  const Heap_Block *block
)
{
  return (Heap_Block *) ((uintptr_t) block - block->prev_size);
}

RTEMS_INLINE_ROUTINE uintptr_t _Heap_Alloc_area_of_block(
  const Heap_Block *block
)
{
  return (uintptr_t) block + HEAP_BLOCK_HEADER_SIZE;
}

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Block_of_alloc_area(
  uintptr_t alloc_begin,
  uintptr_t page_size
)
{
  return (Heap_Block *) (_Heap_Align_down( alloc_begin, page_size )
    - HEAP_BLOCK_HEADER_SIZE);
}

RTEMS_INLINE_ROUTINE uintptr_t _Heap_Block_size( const Heap_Block *block )
{
  return block->size_and_flag & ~HEAP_PREV_BLOCK_USED;
}

RTEMS_INLINE_ROUTINE void _Heap_Block_set_size(
  Heap_Block *block,
  uintptr_t size
)
{
  uintptr_t flag = block->size_and_flag & HEAP_PREV_BLOCK_USED;

  block->size_and_flag = size | flag;
}

RTEMS_INLINE_ROUTINE bool _Heap_Is_prev_used( const Heap_Block *block )
{
  return block->size_and_flag & HEAP_PREV_BLOCK_USED;
}

RTEMS_INLINE_ROUTINE bool _Heap_Is_used(
  const Heap_Block *block
)
{
  const Heap_Block *const next_block =
    _Heap_Block_at( block, _Heap_Block_size( block ) );

  return _Heap_Is_prev_used( next_block );
}

RTEMS_INLINE_ROUTINE bool _Heap_Is_free(
  const Heap_Block *block
)
{
  return !_Heap_Is_used( block );
}

RTEMS_INLINE_ROUTINE bool _Heap_Is_block_in_heap(
  const Heap_Control *heap,
  const Heap_Block *block
)
{
  return (uintptr_t) block >= (uintptr_t) heap->first_block
    && (uintptr_t) block <= (uintptr_t) heap->last_block;
}

/**
 * @brief Sets the size of the last block for heap @a heap.
 *
 * The next block of the last block will be the first block.  Since the first
 * block indicates that the previous block is used, this ensures that the last
 * block appears as used for the _Heap_Is_used() and _Heap_Is_free() functions.
 *
 * This feature will be used to terminate the scattered heap area list.  See
 * also _Heap_Extend().
 */
RTEMS_INLINE_ROUTINE void _Heap_Set_last_block_size( Heap_Control *heap )
{
  _Heap_Block_set_size(
    heap->last_block,
    (uintptr_t) heap->first_block - (uintptr_t) heap->last_block
  );
}

/**
 * @brief Returns the size of the allocatable area in bytes.
 *
 * This value is an integral multiple of the page size.
 */
RTEMS_INLINE_ROUTINE uintptr_t _Heap_Get_size( const Heap_Control *heap )
{
  return heap->stats.size;
}

RTEMS_INLINE_ROUTINE uintptr_t _Heap_Max( uintptr_t a, uintptr_t b )
{
  return a > b ? a : b;
}

RTEMS_INLINE_ROUTINE uintptr_t _Heap_Min( uintptr_t a, uintptr_t b )
{
  return a < b ? a : b;
}

/** @} */

#endif
/* end of include file */
