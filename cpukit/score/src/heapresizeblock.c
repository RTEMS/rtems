/**
 * @file
 *
 * @ingroup ScoreHeap
 *
 * @brief Heap Handler implementation.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2009 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/heap.h>

static Heap_Resize_status _Heap_Resize_block_checked(
  Heap_Control *heap,
  Heap_Block *block,
  uintptr_t alloc_begin,
  uintptr_t new_alloc_size,
  uintptr_t *old_size,
  uintptr_t *new_size
)
{
  Heap_Statistics *const stats = &heap->stats;

  uintptr_t const block_begin = (uintptr_t) block;
  uintptr_t block_size = _Heap_Block_size( block );
  uintptr_t block_end = block_begin + block_size;

  uintptr_t alloc_size = block_end - alloc_begin + HEAP_ALLOC_BONUS;

  Heap_Block *next_block = _Heap_Block_at( block, block_size );
  uintptr_t next_block_size = _Heap_Block_size( next_block );
  bool next_block_is_free = _Heap_Is_free( next_block );

  _HAssert( _Heap_Is_block_in_heap( heap, next_block ) );
  _HAssert( _Heap_Is_prev_used( next_block ) );

  *old_size = alloc_size;

  if ( next_block_is_free ) {
    block_size += next_block_size;
    alloc_size += next_block_size;
  }

  if ( new_alloc_size > alloc_size ) {
    return HEAP_RESIZE_UNSATISFIED;
  }

  if ( next_block_is_free ) {
    _Heap_Block_set_size( block, block_size );

    _Heap_Free_list_remove( next_block );

    next_block = _Heap_Block_at( block, block_size );
    next_block->size_and_flag |= HEAP_PREV_BLOCK_USED;

    /* Statistics */
    --stats->free_blocks;
    stats->free_size -= next_block_size;
  }

  block = _Heap_Block_allocate( heap, block, alloc_begin, new_alloc_size );

  block_size = _Heap_Block_size( block );
  next_block = _Heap_Block_at( block, block_size );
  *new_size = (uintptr_t) next_block - alloc_begin + HEAP_ALLOC_BONUS;

  /* Statistics */
  ++stats->resizes;

  return HEAP_RESIZE_SUCCESSFUL;
}

Heap_Resize_status _Heap_Resize_block(
  Heap_Control *heap,
  void *alloc_begin_ptr,
  uintptr_t new_alloc_size,
  uintptr_t *old_size,
  uintptr_t *new_size
)
{
  uintptr_t const page_size = heap->page_size;

  uintptr_t const alloc_begin = (uintptr_t) alloc_begin_ptr;

  Heap_Block *const block = _Heap_Block_of_alloc_area( alloc_begin, page_size );

  *old_size = 0;
  *new_size = 0;

  if ( _Heap_Is_block_in_heap( heap, block ) ) {
    _Heap_Protection_block_check( heap, block );
    return _Heap_Resize_block_checked(
      heap,
      block,
      alloc_begin,
      new_alloc_size,
      old_size,
      new_size
    );
  }
  return HEAP_RESIZE_FATAL_ERROR;
}
