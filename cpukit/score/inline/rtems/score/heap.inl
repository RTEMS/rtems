/** 
 * @file
 *
 * @brief Static inline implementations of the inlined routines from the heap
 * handler.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
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

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_First_free_block( Heap_Control *heap )
{
  return _Heap_Free_list_head(heap)->next;
}

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Last_free_block( Heap_Control *heap )
{
  return _Heap_Free_list_tail(heap)->prev;
}

RTEMS_INLINE_ROUTINE void _Heap_Block_remove_from_free_list( Heap_Block *block )
{
  Heap_Block *next = block->next;
  Heap_Block *prev = block->prev;

  prev->next = next;
  next->prev = prev;
}

RTEMS_INLINE_ROUTINE void _Heap_Block_replace_in_free_list(
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

RTEMS_INLINE_ROUTINE void _Heap_Block_insert_after(
  Heap_Block *prev_block,
  Heap_Block *new_block
)
{
  Heap_Block *next = prev_block->next;

  new_block->next = next;
  new_block->prev = prev_block;
  prev_block->next = new_block;
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

/**
 * @brief Returns the begin of the allocatable area of @a block.
 */
RTEMS_INLINE_ROUTINE uintptr_t _Heap_Alloc_area_of_block(
  Heap_Block *block
)
{
  return (uintptr_t) block + HEAP_BLOCK_ALLOC_AREA_OFFSET;
}

/**
 * @brief Returns the block associated with the allocatable area starting at
 * @a alloc_area_begin inside a heap with a page size of @a page_size.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Block_of_alloc_area(
  uintptr_t alloc_area_begin,
  uintptr_t page_size
)
{
  return (Heap_Block *) (_Heap_Align_down( alloc_area_begin, page_size )
    - HEAP_BLOCK_ALLOC_AREA_OFFSET);
}

RTEMS_INLINE_ROUTINE bool _Heap_Is_prev_used( Heap_Block *block )
{
  return block->size_and_flag & HEAP_PREV_BLOCK_USED;
}

RTEMS_INLINE_ROUTINE uintptr_t _Heap_Block_size( Heap_Block *block )
{
  return block->size_and_flag & ~HEAP_PREV_BLOCK_USED;
}

RTEMS_INLINE_ROUTINE bool _Heap_Is_block_in_heap(
  Heap_Control *heap,
  Heap_Block *block
)
{
  return _Addresses_Is_in_range( block, heap->start, heap->final );
}

/**
 * @brief Returns the maximum size of the heap.
 */
RTEMS_INLINE_ROUTINE uintptr_t _Heap_Get_size( Heap_Control *heap )
{
  return (uintptr_t) heap->end - (uintptr_t) heap->begin;
}

/** @} */

#endif
/* end of include file */
