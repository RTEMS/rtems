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
 *  Copyright (c) 2010 embedded brains GmbH.
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

static void _Heap_Free_block( Heap_Control *heap, Heap_Block *block )
{
  Heap_Statistics *const stats = &heap->stats;

  /* Statistics */
  ++stats->used_blocks;
  --stats->frees;

  _Heap_Free( heap, (void *) _Heap_Alloc_area_of_block( block ));
}

static void _Heap_Merge_below(
  Heap_Control *heap,
  uintptr_t extend_area_begin,
  Heap_Block *first_block
)
{
  uintptr_t const page_size = heap->page_size;
  uintptr_t const new_first_block_alloc_begin =
    _Heap_Align_up( extend_area_begin + HEAP_BLOCK_HEADER_SIZE, page_size );
  uintptr_t const new_first_block_begin =
    new_first_block_alloc_begin - HEAP_BLOCK_HEADER_SIZE;
  uintptr_t const first_block_begin = (uintptr_t) first_block;
  uintptr_t const new_first_block_size =
    first_block_begin - new_first_block_begin;
  Heap_Block *const new_first_block = (Heap_Block *) new_first_block_begin;

  new_first_block->prev_size = first_block->prev_size;
  new_first_block->size_and_flag = new_first_block_size | HEAP_PREV_BLOCK_USED;

  _Heap_Free_block( heap, new_first_block );
}

static void _Heap_Merge_above(
  Heap_Control *heap,
  Heap_Block *last_block,
  uintptr_t extend_area_end
)
{
  uintptr_t const page_size = heap->page_size;
  uintptr_t const last_block_begin = (uintptr_t) last_block;
  uintptr_t const last_block_new_size = _Heap_Align_down(
    extend_area_end - last_block_begin - HEAP_BLOCK_HEADER_SIZE,
    page_size
  );
  Heap_Block *const new_last_block =
    _Heap_Block_at( last_block, last_block_new_size );

  new_last_block->size_and_flag =
    (last_block->size_and_flag - last_block_new_size)
      | HEAP_PREV_BLOCK_USED;

  _Heap_Block_set_size( last_block, last_block_new_size );

  _Heap_Free_block( heap, last_block );
}

static void _Heap_Link_below(
  Heap_Block *link,
  Heap_Block *last_block
)
{
  uintptr_t const last_block_begin = (uintptr_t) last_block;
  uintptr_t const link_begin = (uintptr_t) link;

  last_block->size_and_flag =
    (link_begin - last_block_begin) | HEAP_PREV_BLOCK_USED;
}

static void _Heap_Link_above(
  Heap_Block *link,
  Heap_Block *first_block,
  Heap_Block *last_block
)
{
  uintptr_t const link_begin = (uintptr_t) link;
  uintptr_t const first_block_begin = (uintptr_t) first_block;

  _Heap_Block_set_size( link, first_block_begin - link_begin );

  last_block->size_and_flag |= HEAP_PREV_BLOCK_USED;
}

bool _Heap_Extend(
  Heap_Control *heap,
  void *extend_area_begin_ptr,
  uintptr_t extend_area_size,
  uintptr_t *extended_size_ptr
)
{
  Heap_Statistics *const stats = &heap->stats;
  Heap_Block *const first_block = heap->first_block;
  Heap_Block *start_block = first_block;
  Heap_Block *merge_below_block = NULL;
  Heap_Block *merge_above_block = NULL;
  Heap_Block *link_below_block = NULL;
  Heap_Block *link_above_block = NULL;
  Heap_Block *extend_first_block = NULL;
  Heap_Block *extend_last_block = NULL;
  uintptr_t const page_size = heap->page_size;
  uintptr_t const min_block_size = heap->min_block_size;
  uintptr_t const extend_area_begin = (uintptr_t) extend_area_begin_ptr;
  uintptr_t const extend_area_end = extend_area_begin + extend_area_size;
  uintptr_t const free_size = stats->free_size;
  uintptr_t extend_first_block_size = 0;
  uintptr_t extended_size = 0;
  bool extend_area_ok = false;

  if ( extend_area_end < extend_area_begin ) {
    return false;
  }

  extend_area_ok = _Heap_Get_first_and_last_block(
    extend_area_begin,
    extend_area_size,
    page_size,
    min_block_size,
    &extend_first_block,
    &extend_last_block
  );
  if (!extend_area_ok ) {
    /* For simplicity we reject extend areas that are too small */
    return false;
  }

  do {
    uintptr_t const sub_area_begin = (start_block != first_block) ?
      (uintptr_t) start_block : heap->area_begin;
    uintptr_t const sub_area_end = start_block->prev_size;
    Heap_Block *const end_block =
      _Heap_Block_of_alloc_area( sub_area_end, page_size );

    if (
      sub_area_end > extend_area_begin && extend_area_end > sub_area_begin
    ) {
      return false;
    }

    if ( extend_area_end == sub_area_begin ) {
      merge_below_block = start_block;
    } else if ( extend_area_end < sub_area_end ) {
      link_below_block = start_block;
    }

    if ( sub_area_end == extend_area_begin ) {
      start_block->prev_size = extend_area_end;

      merge_above_block = end_block;
    } else if ( sub_area_end < extend_area_begin ) {
      link_above_block = end_block;
    }

    start_block = _Heap_Block_at( end_block, _Heap_Block_size( end_block ) );
  } while ( start_block != first_block );

  if ( extend_area_begin < heap->area_begin ) {
    heap->area_begin = extend_area_begin;
  } else if ( heap->area_end < extend_area_end ) {
    heap->area_end = extend_area_end;
  }

  extend_first_block_size =
    (uintptr_t) extend_last_block - (uintptr_t) extend_first_block;

  extend_first_block->prev_size = extend_area_end;
  extend_first_block->size_and_flag =
    extend_first_block_size | HEAP_PREV_BLOCK_USED;
  _Heap_Protection_block_initialize( heap, extend_first_block );

  extend_last_block->prev_size = extend_first_block_size;
  extend_last_block->size_and_flag = 0;
  _Heap_Protection_block_initialize( heap, extend_last_block );

  if ( (uintptr_t) extend_first_block < (uintptr_t) heap->first_block ) {
    heap->first_block = extend_first_block;
  } else if ( (uintptr_t) extend_last_block > (uintptr_t) heap->last_block ) {
    heap->last_block = extend_last_block;
  }

  if ( merge_below_block != NULL ) {
    _Heap_Merge_below( heap, extend_area_begin, merge_below_block );
  } else if ( link_below_block != NULL ) {
    _Heap_Link_below(
      link_below_block,
      extend_last_block
    );
  }

  if ( merge_above_block != NULL ) {
    _Heap_Merge_above( heap, merge_above_block, extend_area_end );
  } else if ( link_above_block != NULL ) {
    _Heap_Link_above(
      link_above_block,
      extend_first_block,
      extend_last_block
    );
  }

  if ( merge_below_block == NULL && merge_above_block == NULL ) {
    _Heap_Free_block( heap, extend_first_block );
  }

  _Heap_Set_last_block_size( heap );

  extended_size = stats->free_size - free_size;

  /* Statistics */
  stats->size += extended_size;

  if ( extended_size_ptr != NULL )
    *extended_size_ptr = extended_size;

  return true;
}
