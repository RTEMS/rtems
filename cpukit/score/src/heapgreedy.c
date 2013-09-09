/**
 * @file
 *
 * @ingroup ScoreHeap
 *
 * @brief Heap Handler API.
 */

/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/heapimpl.h>

Heap_Block *_Heap_Greedy_allocate(
  Heap_Control *heap,
  const uintptr_t *block_sizes,
  size_t block_count
)
{
  Heap_Block *const free_list_tail = _Heap_Free_list_tail( heap );
  Heap_Block *allocated_blocks = NULL;
  Heap_Block *blocks = NULL;
  Heap_Block *current;
  size_t i;

  _Heap_Protection_free_all_delayed_blocks( heap );

  for (i = 0; i < block_count; ++i) {
    void *next = _Heap_Allocate( heap, block_sizes [i] );

    if ( next != NULL ) {
      Heap_Block *next_block = _Heap_Block_of_alloc_area(
        (uintptr_t) next,
        heap->page_size
      );

      next_block->next = allocated_blocks;
      allocated_blocks = next_block;
    }
  }

  while ( (current = _Heap_Free_list_first( heap )) != free_list_tail ) {
    _Heap_Block_allocate(
      heap,
      current,
      _Heap_Alloc_area_of_block( current ),
      _Heap_Block_size( current ) - HEAP_BLOCK_HEADER_SIZE
    );

    current->next = blocks;
    blocks = current;
  }

  while ( allocated_blocks != NULL ) {
    current = allocated_blocks;
    allocated_blocks = allocated_blocks->next;
    _Heap_Free( heap, (void *) _Heap_Alloc_area_of_block( current ) );
  }

  return blocks;
}

Heap_Block *_Heap_Greedy_allocate_all_except_largest(
  Heap_Control *heap,
  uintptr_t *allocatable_size
)
{
  Heap_Information info;

  _Heap_Get_free_information( heap, &info );
  *allocatable_size = info.largest - HEAP_BLOCK_HEADER_SIZE + HEAP_ALLOC_BONUS;

  return _Heap_Greedy_allocate( heap, allocatable_size, 1 );
}

void _Heap_Greedy_free(
  Heap_Control *heap,
  Heap_Block *blocks
)
{
  while ( blocks != NULL ) {
    Heap_Block *current = blocks;

    blocks = blocks->next;
    _Heap_Free( heap, (void *) _Heap_Alloc_area_of_block( current ) );
  }
}
