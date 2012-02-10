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

#include <rtems/score/heap.h>

Heap_Block *_Heap_Greedy_allocate(
  Heap_Control *heap,
  uintptr_t remaining_free_space
)
{
  void *free_space = remaining_free_space > 0 ?
    _Heap_Allocate( heap, remaining_free_space )
      : NULL;
  Heap_Block *const free_list_tail = _Heap_Free_list_tail( heap );
  Heap_Block *current = _Heap_Free_list_first( heap );
  Heap_Block *blocks = NULL;

  while ( current != free_list_tail ) {
    _Heap_Block_allocate(
      heap,
      current,
      _Heap_Alloc_area_of_block( current ),
      _Heap_Block_size( current ) - HEAP_BLOCK_HEADER_SIZE
    );

    current->next = blocks;
    blocks = current;
    current = _Heap_Free_list_first( heap );
  }

  _Heap_Free( heap, free_space );

  return blocks;
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
