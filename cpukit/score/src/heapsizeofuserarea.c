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

bool _Heap_Size_of_alloc_area(
  Heap_Control *heap,
  void *alloc_begin_ptr,
  uintptr_t *alloc_size
)
{
  uintptr_t const page_size = heap->page_size;
  uintptr_t const alloc_begin = (uintptr_t) alloc_begin_ptr;
  Heap_Block *block = _Heap_Block_of_alloc_area( alloc_begin, page_size );
  Heap_Block *next_block = NULL;
  uintptr_t block_size = 0;

  if ( !_Heap_Is_block_in_heap( heap, block ) ) {
    return false;
  }

  block_size = _Heap_Block_size( block );
  next_block = _Heap_Block_at( block, block_size );

  if (
    !_Heap_Is_block_in_heap( heap, next_block )
      || !_Heap_Is_prev_used( next_block )
  ) {
    return false;
  }

  *alloc_size = (uintptr_t) next_block + HEAP_ALLOC_BONUS - alloc_begin;

  return true;
}

