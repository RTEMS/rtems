/*
 *  Heap Handler
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/heap.h>

void *_Heap_Allocate( Heap_Control *heap, uintptr_t size )
{
  Heap_Statistics *const stats = &heap->stats;
  Heap_Block * const tail = _Heap_Free_list_tail( heap );
  Heap_Block *block = _Heap_First_free_block( heap );
  uint32_t search_count = 0;
  void *alloc_area_begin_ptr = NULL;

  size = _Heap_Calc_block_size( size, heap->page_size, heap->min_block_size );
  if( size == 0 ) {
    return NULL;
  }

  /*
   * Find large enough free block.
   *
   * Do not bother to mask out the HEAP_PREV_BLOCK_USED bit as it will not
   * change the result of the size comparison.
   */
  while (block != tail && block->size_and_flag < size) {
    _HAssert( _Heap_Is_prev_used( block ));

    block = block->next;
    ++search_count;
  }

  if (block != tail) {
    _Heap_Block_allocate( heap, block, size );

    alloc_area_begin_ptr = (void *) _Heap_Alloc_area_of_block( block );

    _HAssert( _Heap_Is_aligned( (uintptr_t) alloc_area_begin_ptr, heap->page_size ));

    /* Statistics */
    ++stats->allocs;
    stats->searches += search_count;
  }

  /* Statistics */
  if (stats->max_search < search_count) {
    stats->max_search = search_count;
  }

  return alloc_area_begin_ptr;
}
