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

bool _Heap_Size_of_alloc_area(
  Heap_Control *heap,
  void *alloc_area_begin_ptr,
  uintptr_t *size
)
{
  uintptr_t alloc_area_begin = (uintptr_t) alloc_area_begin_ptr;
  Heap_Block *block =
    _Heap_Block_of_alloc_area( alloc_area_begin, heap->page_size );
  Heap_Block *next_block = NULL;
  uintptr_t block_size = 0;

  if (
    !_Addresses_Is_in_range( alloc_area_begin_ptr, heap->start, heap->final )
  ) {
    return false;
  }


  _HAssert(_Heap_Is_block_in_heap( heap, block ));
  if ( !_Heap_Is_block_in_heap( heap, block ) ) {
    return false;
  }

  block_size = _Heap_Block_size( block );
  next_block = _Heap_Block_at( block, block_size );

  _HAssert( _Heap_Is_block_in_heap( heap, next_block ));
  _HAssert( _Heap_Is_prev_used( next_block ));
  if (
    !_Heap_Is_block_in_heap( heap, next_block ) ||
    !_Heap_Is_prev_used( next_block )
  ) {
    return false;
  }

  /*
   * 'alloc_area_begin' could be greater than 'block' address plus
   * HEAP_BLOCK_ALLOC_AREA_OFFSET as _Heap_Allocate_aligned() may produce such
   * user pointers. To get rid of this offset we calculate user size as
   * difference between the end of 'block' (='next_block') and
   * 'alloc_area_begin' and then add correction equal to the offset of the
   * 'size' field of the 'Heap_Block' structure. The correction is due to the
   * fact that 'prev_size' field of the next block is actually used as user
   * accessible area of 'block'.
   */
  *size = (uintptr_t) next_block - alloc_area_begin + HEAP_BLOCK_SIZE_OFFSET;

  return true;
}

