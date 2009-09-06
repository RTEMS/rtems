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
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/heap.h>

Heap_Resize_status _Heap_Resize_block(
  Heap_Control *heap,
  void *alloc_begin_ptr,
  uintptr_t new_alloc_size,
  uintptr_t *old_size,
  uintptr_t *new_size
)
{
  Heap_Statistics *const stats = &heap->stats;
  uintptr_t const min_block_size = heap->min_block_size;
  uintptr_t const page_size = heap->page_size;
  uintptr_t const alloc_begin = (uintptr_t) alloc_begin_ptr;
  Heap_Block *const block = _Heap_Block_of_alloc_area( alloc_begin, page_size );
  Heap_Block *next_block = NULL;
  Heap_Block *next_next_block = NULL;
  uintptr_t block_size = 0;
  uintptr_t block_end = 0;
  uintptr_t next_block_size = 0;
  bool next_block_is_used = false;;
  uintptr_t alloc_size = 0;
  uintptr_t prev_block_used_flag = 0;

  *old_size = 0;
  *new_size = 0;

  if ( !_Heap_Is_block_in_heap( heap, block ) ) {
    return HEAP_RESIZE_FATAL_ERROR;
  }

  block_size = _Heap_Block_size( block );
  block_end = (uintptr_t) block + block_size;
  prev_block_used_flag = block->size_and_flag & HEAP_PREV_BLOCK_USED;
  next_block = _Heap_Block_at( block, block_size );

  _HAssert( _Heap_Is_block_in_heap( heap, next_block ) );
  _HAssert( _Heap_Is_prev_used( next_block ) );

  next_block_size = _Heap_Block_size( next_block );
  next_next_block = _Heap_Block_at( next_block, next_block_size );

  _HAssert(
    next_block == heap->last_block
      || _Heap_Is_block_in_heap( heap, next_next_block )
  );

  next_block_is_used = next_block == heap->last_block
    || _Heap_Is_prev_used( next_next_block );

  alloc_size = block_end - alloc_begin + HEAP_BLOCK_SIZE_OFFSET;

  *old_size = alloc_size;

  if ( new_alloc_size > alloc_size ) {
    /*
     * Need to extend the block: allocate part of the next block and then
     * merge the blocks.
     */
    if ( next_block_is_used ) {
      return HEAP_RESIZE_UNSATISFIED;
    } else {
      uintptr_t add_block_size =
        _Heap_Align_up( new_alloc_size - alloc_size, page_size );

      if ( add_block_size < min_block_size ) {
        add_block_size = min_block_size;
      }

      if ( add_block_size > next_block_size ) {
        return HEAP_RESIZE_UNSATISFIED;
      }

      next_block = _Heap_Block_allocate(
        heap,
        next_block,
        _Heap_Alloc_area_of_block( next_block ),
        add_block_size - HEAP_BLOCK_HEADER_SIZE
      );

      /* Merge the blocks */
      block->size_and_flag = ( block_size + _Heap_Block_size( next_block ) )
        | prev_block_used_flag;

      /* Statistics */
      --stats->used_blocks;
    }
  } else {
    /* Calculate how much memory we could free */
    uintptr_t free_block_size =
      _Heap_Align_down( alloc_size - new_alloc_size, page_size );

    if ( free_block_size > 0 ) {
      /*
       * To free some memory the block should be shortened so that it can can
       * hold 'new_alloc_size' user bytes and still remain not shorter than
       * 'min_block_size'. 
       */
      uintptr_t new_block_size = block_size - free_block_size;

      if ( new_block_size < min_block_size ) {
        uintptr_t const delta = min_block_size - new_block_size;

        _HAssert( free_block_size >= delta );

        free_block_size -= delta;

        if ( free_block_size == 0 ) {
          /* Statistics */
          ++stats->resizes;

          return HEAP_RESIZE_SUCCESSFUL;
        }

        new_block_size += delta;
      }

      _HAssert( new_block_size >= min_block_size );
      _HAssert( new_block_size + free_block_size == block_size );
      _HAssert( _Heap_Is_aligned( new_block_size, page_size ) );
      _HAssert( _Heap_Is_aligned( free_block_size, page_size ) );

      if ( !next_block_is_used ) {
        /* Extend the next block */
        Heap_Block *const new_next_block =
          _Heap_Block_at( block, new_block_size );
        uintptr_t const new_next_block_size =
          next_block_size + free_block_size;

        _HAssert( _Heap_Is_block_in_heap( heap, next_next_block ) );

        block->size_and_flag = new_block_size | prev_block_used_flag;
        new_next_block->size_and_flag =
          new_next_block_size | HEAP_PREV_BLOCK_USED;
        next_next_block->prev_size = new_next_block_size;

        _Heap_Free_list_replace( next_block, new_next_block );

        *new_size = new_next_block_size - HEAP_BLOCK_SIZE_OFFSET;

        /* Statistics */
        stats->free_size += free_block_size;
      } else if ( free_block_size >= min_block_size ) {
        /* Split the block into two used parts, then free the second one */
        block->size_and_flag = new_block_size | prev_block_used_flag;
        next_block = _Heap_Block_at( block, new_block_size );
        next_block->size_and_flag = free_block_size | HEAP_PREV_BLOCK_USED;

        _Heap_Free( heap, (void *) _Heap_Alloc_area_of_block( next_block ) );

        *new_size = free_block_size - HEAP_BLOCK_SIZE_OFFSET;

        /* Statistics */
        ++stats->used_blocks; /* We have created used block */
        --stats->frees; /* Do not count next call in stats */
      }
    }
  }

  /* Statistics */
  ++stats->resizes;

  return HEAP_RESIZE_SUCCESSFUL;
}
