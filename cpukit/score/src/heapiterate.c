/**
 * @file
 *
 * @ingroup ScoreHeap
 *
 * @brief _Heap_Iterate() implementation.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
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

void _Heap_Iterate(
  Heap_Control *heap,
  Heap_Block_visitor visitor,
  void *visitor_arg
)
{
  Heap_Block *current = heap->first_block;
  Heap_Block *end = heap->last_block;
  bool stop = false;

  while ( !stop && current != end ) {
    uintptr_t size = _Heap_Block_size( current );
    Heap_Block *next = _Heap_Block_at( current, size );
    bool used = _Heap_Is_prev_used( next );

    stop = (*visitor)( current, size, used, visitor_arg );

    current = next;
  }
}
