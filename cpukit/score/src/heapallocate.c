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
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/heap.h>

#ifdef RTEMS_HEAP_DEBUG
  static void _Heap_Check_allocation(
    const Heap_Control *heap,
    const Heap_Block *block,
    uintptr_t alloc_begin,
    uintptr_t alloc_size,
    uintptr_t alignment,
    uintptr_t boundary
  )
  {
    uintptr_t const min_block_size = heap->min_block_size;
    uintptr_t const page_size = heap->page_size;

    uintptr_t const block_begin = (uintptr_t) block;
    uintptr_t const block_size = _Heap_Block_size( block );
    uintptr_t const block_end = block_begin + block_size;

    uintptr_t const alloc_end = alloc_begin + alloc_size;

    uintptr_t const alloc_area_begin = _Heap_Alloc_area_of_block( block );
    uintptr_t const alloc_area_offset = alloc_begin - alloc_area_begin;

    _HAssert( block_size >= min_block_size );
    _HAssert( block_begin < block_end );
    _HAssert(
      _Heap_Is_aligned( block_begin + HEAP_BLOCK_HEADER_SIZE, page_size )
    );
    _HAssert(
      _Heap_Is_aligned( block_size, page_size )
    );

    _HAssert( alloc_end <= block_end + HEAP_BLOCK_SIZE_OFFSET );
    _HAssert( alloc_area_begin == block_begin + HEAP_BLOCK_HEADER_SIZE);
    _HAssert( alloc_area_offset < page_size );

    _HAssert( _Heap_Is_aligned( alloc_area_begin, page_size ) );
    if ( alignment == 0 ) {
      _HAssert( alloc_begin == alloc_area_begin );
    } else {
      _HAssert( _Heap_Is_aligned( alloc_begin, alignment ) );
    }

    if ( boundary != 0 ) {
      uintptr_t boundary_line = _Heap_Align_down( alloc_end, boundary );

      _HAssert( alloc_size <= boundary );
      _HAssert( boundary_line <= alloc_begin || alloc_end <= boundary_line );
    }
  }
#else
  #define _Heap_Check_allocation( h, b, ab, as, ag, bd ) ((void) 0)
#endif

static uintptr_t _Heap_Check_block(
  const Heap_Control *heap,
  const Heap_Block *block,
  uintptr_t alloc_size,
  uintptr_t alignment,
  uintptr_t boundary
)
{
  uintptr_t const page_size = heap->page_size;
  uintptr_t const min_block_size = heap->min_block_size;

  uintptr_t const block_begin = (uintptr_t) block;
  uintptr_t const block_size = _Heap_Block_size( block );
  uintptr_t const block_end = block_begin + block_size;

  uintptr_t const alloc_begin_floor = _Heap_Alloc_area_of_block( block );
  uintptr_t const alloc_begin_ceiling = block_end - min_block_size
    + HEAP_BLOCK_HEADER_SIZE + page_size - 1;

  uintptr_t alloc_end = block_end + HEAP_BLOCK_SIZE_OFFSET;
  uintptr_t alloc_begin = alloc_end - alloc_size;

  alloc_begin = _Heap_Align_down( alloc_begin, alignment );

  /* Ensure that the we have a valid new block at the end */
  if ( alloc_begin > alloc_begin_ceiling ) {
    alloc_begin = _Heap_Align_down( alloc_begin_ceiling, alignment );
  }

  alloc_end = alloc_begin + alloc_size;

  /* Ensure boundary constaint */
  if ( boundary != 0 ) {
    uintptr_t const boundary_floor = alloc_begin_floor + alloc_size;
    uintptr_t boundary_line = _Heap_Align_down( alloc_end, boundary );

    while ( alloc_begin < boundary_line && boundary_line < alloc_end ) {
      if ( boundary_line < boundary_floor ) {
        return 0;
      }
      alloc_begin = boundary_line - alloc_size;
      alloc_begin = _Heap_Align_down( alloc_begin, alignment );
      alloc_end = alloc_begin + alloc_size;
      boundary_line = _Heap_Align_down( alloc_end, boundary );
    }
  }

  /* Ensure that the we have a valid new block at the beginning */
  if ( alloc_begin >= alloc_begin_floor ) {
    uintptr_t const alloc_block_begin =
      (uintptr_t) _Heap_Block_of_alloc_area( alloc_begin, page_size );
    uintptr_t const free_size = alloc_block_begin - block_begin;

    if ( free_size >= min_block_size || free_size == 0 ) {
      return alloc_begin;
    }
  }

  return 0;
}

void *_Heap_Allocate_aligned_with_boundary(
  Heap_Control *heap,
  uintptr_t alloc_size,
  uintptr_t alignment,
  uintptr_t boundary
)
{
  Heap_Statistics *const stats = &heap->stats;
  Heap_Block *const free_list_tail = _Heap_Free_list_tail( heap );
  Heap_Block *block = _Heap_Free_list_first( heap );
  uintptr_t const block_size_floor = alloc_size + HEAP_BLOCK_HEADER_SIZE
    - HEAP_BLOCK_SIZE_OFFSET;
  uintptr_t const page_size = heap->page_size;
  uintptr_t alloc_begin = 0;
  uint32_t search_count = 0;

  if ( block_size_floor < alloc_size ) {
    /* Integer overflow occured */
    return NULL;
  }

  if ( boundary != 0 ) {
    if ( boundary < alloc_size ) {
      return NULL;
    }

    if ( alignment == 0 ) {
      alignment = page_size;
    }
  }

  while ( block != free_list_tail ) {
    _HAssert( _Heap_Is_prev_used( block ) );

    /* Statistics */
    ++search_count;

    /*
     * The HEAP_PREV_BLOCK_USED flag is always set in the block size_and_flag
     * field.  Thus the value is about one unit larger than the real block
     * size.  The greater than operator takes this into account.
     */
    if ( block->size_and_flag > block_size_floor ) {
      if ( alignment == 0 ) {
        alloc_begin = _Heap_Alloc_area_of_block( block );
      } else {
        alloc_begin = _Heap_Check_block(
          heap,
          block,
          alloc_size,
          alignment,
          boundary
        );
      }
    }

    if ( alloc_begin != 0 ) {
      break;
    }

    block = block->next;
  }

  if ( alloc_begin != 0 ) {
    /* Statistics */
    stats->searches += search_count;

    block = _Heap_Block_allocate( heap, block, alloc_begin, alloc_size );

    _Heap_Check_allocation(
      heap,
      block,
      alloc_begin,
      alloc_size,
      alignment,
      boundary
    );
  }

  /* Statistics */
  if ( stats->max_search < search_count ) {
    stats->max_search = search_count;
  }

  return (void *) alloc_begin;
}
