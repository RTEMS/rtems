/**
 * @file
 *
 * @ingroup ScoreHeap
 *
 * @brief Heap Handler implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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

bool _Heap_Free( Heap_Control *heap, void *alloc_begin_ptr )
{
  Heap_Statistics *const stats = &heap->stats;
  uintptr_t alloc_begin = (uintptr_t) alloc_begin_ptr;
  Heap_Block *block =
    _Heap_Block_of_alloc_area( alloc_begin, heap->page_size );
  Heap_Block *next_block = NULL;
  uintptr_t block_size = 0;
  uintptr_t next_block_size = 0;
  bool next_is_free = false;

  if ( !_Heap_Is_block_in_heap( heap, block ) ) {
    return false;
  }

  block_size = _Heap_Block_size( block );
  next_block = _Heap_Block_at( block, block_size );

  if ( !_Heap_Is_block_in_heap( heap, next_block ) ) {
    _HAssert( false );
    return false;
  }

  if ( !_Heap_Is_prev_used( next_block ) ) {
    _HAssert( false );
    return false;
  }

  next_block_size = _Heap_Block_size( next_block );
  next_is_free = next_block != heap->last_block
    && !_Heap_Is_prev_used( _Heap_Block_at( next_block, next_block_size ));

  if ( !_Heap_Is_prev_used( block ) ) {
    uintptr_t const prev_size = block->prev_size;
    Heap_Block * const prev_block = _Heap_Block_at( block, -prev_size );

    if ( !_Heap_Is_block_in_heap( heap, prev_block ) ) {
      _HAssert( false );
      return( false );
    }

    /* As we always coalesce free blocks, the block that preceedes prev_block
       must have been used. */
    if ( !_Heap_Is_prev_used ( prev_block) ) {
      _HAssert( false );
      return( false );
    }

    if ( next_is_free ) {       /* coalesce both */
      uintptr_t const size = block_size + prev_size + next_block_size;
      _Heap_Free_list_remove( next_block );
      stats->free_blocks -= 1;
      prev_block->size_and_flag = size | HEAP_PREV_BLOCK_USED;
      next_block = _Heap_Block_at( prev_block, size );
      _HAssert(!_Heap_Is_prev_used( next_block));
      next_block->prev_size = size;
    } else {                      /* coalesce prev */
      uintptr_t const size = block_size + prev_size;
      prev_block->size_and_flag = size | HEAP_PREV_BLOCK_USED;
      next_block->size_and_flag &= ~HEAP_PREV_BLOCK_USED;
      next_block->prev_size = size;
    }
  } else if ( next_is_free ) {    /* coalesce next */
    uintptr_t const size = block_size + next_block_size;
    _Heap_Free_list_replace( next_block, block );
    block->size_and_flag = size | HEAP_PREV_BLOCK_USED;
    next_block  = _Heap_Block_at( block, size );
    next_block->prev_size = size;
  } else {                        /* no coalesce */
    /* Add 'block' to the head of the free blocks list as it tends to
       produce less fragmentation than adding to the tail. */
    _Heap_Free_list_insert_after( _Heap_Free_list_head( heap), block );
    block->size_and_flag = block_size | HEAP_PREV_BLOCK_USED;
    next_block->size_and_flag &= ~HEAP_PREV_BLOCK_USED;
    next_block->prev_size = block_size;

    /* Statistics */
    ++stats->free_blocks;
    if ( stats->max_free_blocks < stats->free_blocks ) {
      stats->max_free_blocks = stats->free_blocks;
    }
  }

  /* Statistics */
  --stats->used_blocks;
  ++stats->frees;
  stats->free_size += block_size;

  return( true );
}
