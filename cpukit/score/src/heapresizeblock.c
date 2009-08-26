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

Heap_Resize_status _Heap_Resize_block(
  Heap_Control *heap,
  void         *alloc_area_begin_ptr,
  uintptr_t      size,
  uintptr_t     *old_mem_size,
  uintptr_t     *avail_mem_size
)
{
  uintptr_t alloc_area_begin = (uintptr_t) alloc_area_begin_ptr;
  Heap_Block *block;
  Heap_Block *next_block;
  uintptr_t   next_block_size;
  bool       next_is_used;
  Heap_Block *next_next_block;
  uintptr_t   old_block_size;
  uintptr_t   old_user_size;
  uintptr_t   prev_used_flag;
  Heap_Statistics *const stats = &heap->stats;
  uintptr_t const min_block_size = heap->min_block_size;
  uintptr_t const page_size = heap->page_size;

  *old_mem_size = 0;
  *avail_mem_size = 0;

  block = _Heap_Block_of_alloc_area(alloc_area_begin, heap->page_size);
  _HAssert(_Heap_Is_block_in_heap(heap, block));
  if (!_Heap_Is_block_in_heap(heap, block))
    return HEAP_RESIZE_FATAL_ERROR;

  prev_used_flag = block->size_and_flag & HEAP_PREV_BLOCK_USED;
  old_block_size = _Heap_Block_size(block);
  next_block = _Heap_Block_at(block, old_block_size);

  _HAssert(_Heap_Is_block_in_heap(heap, next_block));
  _HAssert(_Heap_Is_prev_used(next_block));
  if ( !_Heap_Is_block_in_heap(heap, next_block) ||
       !_Heap_Is_prev_used(next_block))
    return HEAP_RESIZE_FATAL_ERROR;

  next_block_size = _Heap_Block_size(next_block);
  next_next_block = _Heap_Block_at(next_block, next_block_size);
  next_is_used    = (next_block == heap->final) ||
                     _Heap_Is_prev_used(next_next_block);

  /* See _Heap_Size_of_alloc_area() source for explanations */
  old_user_size = (uintptr_t) next_block - alloc_area_begin
    + HEAP_BLOCK_SIZE_OFFSET;

  *old_mem_size = old_user_size;

  if (size > old_user_size) {
    /* Need to extend the block: allocate part of the next block and then
       merge 'block' and allocated block together. */
    if (next_is_used)    /* Next block is in use, -- no way to extend */
      return HEAP_RESIZE_UNSATISFIED;
    else {
      uintptr_t add_block_size =
        _Heap_Align_up(size - old_user_size, page_size);
      if (add_block_size < min_block_size)
        add_block_size = min_block_size;
      if (add_block_size > next_block_size)
        return HEAP_RESIZE_UNSATISFIED; /* Next block is too small or none. */
      add_block_size =
        _Heap_Block_allocate(heap, next_block, add_block_size);
      /* Merge two subsequent blocks */
      block->size_and_flag = (old_block_size + add_block_size) | prev_used_flag;
      --stats->used_blocks;
    }
  } else {

    /* Calculate how much memory we could free */
    uintptr_t free_block_size =
      _Heap_Align_down(old_user_size - size, page_size);

    if (free_block_size > 0) {

      /* To free some memory the block should be shortened so that it can
         can hold 'size' user bytes and still remain not shorter than
         'min_block_size'. */

      uintptr_t new_block_size = old_block_size - free_block_size;

      if (new_block_size < min_block_size) {
        uintptr_t delta = min_block_size - new_block_size;
        _HAssert(free_block_size >= delta);
        free_block_size -= delta;
        if (free_block_size == 0) {
          ++stats->resizes;
          return HEAP_RESIZE_SUCCESSFUL;
        }
        new_block_size += delta;
      }

      _HAssert(new_block_size >= min_block_size);
      _HAssert(new_block_size + free_block_size == old_block_size);
      _HAssert(_Heap_Is_aligned(new_block_size, page_size));
      _HAssert(_Heap_Is_aligned(free_block_size, page_size));

      if (!next_is_used) {
        /* Extend the next block to the low addresses by 'free_block_size' */
        Heap_Block *const new_next_block =
          _Heap_Block_at(block, new_block_size);
        uintptr_t const new_next_block_size =
          next_block_size + free_block_size;
        _HAssert(_Heap_Is_block_in_heap(heap, next_next_block));
        block->size_and_flag = new_block_size | prev_used_flag;
        new_next_block->size_and_flag = new_next_block_size | HEAP_PREV_BLOCK_USED;
        next_next_block->prev_size = new_next_block_size;
        _Heap_Block_replace_in_free_list(next_block, new_next_block);
        heap->stats.free_size += free_block_size;
        *avail_mem_size = new_next_block_size - HEAP_BLOCK_USED_OVERHEAD;

      } else if (free_block_size >= min_block_size) {
        /* Split the block into 2 used  parts, then free the second one. */
        block->size_and_flag = new_block_size | prev_used_flag;
        next_block = _Heap_Block_at(block, new_block_size);
        next_block->size_and_flag = free_block_size | HEAP_PREV_BLOCK_USED;
        ++stats->used_blocks; /* We have created used block */
        --stats->frees;       /* Don't count next call in stats */
        _Heap_Free(heap, (void *) _Heap_Alloc_area_of_block(next_block));
        *avail_mem_size = free_block_size - HEAP_BLOCK_USED_OVERHEAD;
      }
    }
  }

  ++stats->resizes;
  return HEAP_RESIZE_SUCCESSFUL;
}
