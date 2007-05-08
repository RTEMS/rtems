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

/*
 *  _Heap_Resize_block
 *
 *  DESCRIPTION:
 *
 *  This routine tries to resize in place the block that is pointed to by the
 *  'starting_address' to the new 'size'.
 *
 *  Input parameters:
 *    the_heap         - pointer to heap header
 *    starting_address - starting address of the memory block
 *    size             - new size
 *
 *  Output parameters:
 *    'old_mem_size'   - the size of the user memory area of the block before
 *                       resizing.
 *    'avail_mem_size' - the size of the user memory area of the free block
 *                       that has been enlarged or created due to resizing,
 *                       0 if none.
 *    Returns
 *      HEAP_RESIZE_SUCCESSFUL  - if success
 *      HEAP_RESIZE_UNSATISFIED - if the block can't be resized in place
 *      HEAP_RESIZE_FATAL_ERROR - if failure
 */

Heap_Resize_status _Heap_Resize_block(
  Heap_Control *the_heap,
  void         *starting_address,
  size_t        size,
  uint32_t     *old_mem_size,
  uint32_t     *avail_mem_size
)
{
  Heap_Block *the_block;
  Heap_Block *next_block;
  uint32_t   next_block_size;
  boolean    next_is_used;
  Heap_Block *next_next_block;
  uint32_t   old_block_size;
  uint32_t   old_user_size;
  uint32_t   prev_used_flag;
  Heap_Statistics *const stats = &the_heap->stats;
  uint32_t const min_block_size = the_heap->min_block_size;
  uint32_t const page_size = the_heap->page_size;

  *old_mem_size = 0;
  *avail_mem_size = 0;

  _Heap_Start_of_block(the_heap, starting_address, &the_block);
  _HAssert(_Heap_Is_block_in(the_heap, the_block));
  if (!_Heap_Is_block_in(the_heap, the_block))
    return HEAP_RESIZE_FATAL_ERROR;

  prev_used_flag = the_block->size & HEAP_PREV_USED;
  old_block_size = _Heap_Block_size(the_block);
  next_block = _Heap_Block_at(the_block, old_block_size);

  _HAssert(_Heap_Is_block_in(the_heap, next_block));
  _HAssert(_Heap_Is_prev_used(next_block));
  if ( !_Heap_Is_block_in(the_heap, next_block) ||
       !_Heap_Is_prev_used(next_block))
    return HEAP_RESIZE_FATAL_ERROR;

  next_block_size = _Heap_Block_size(next_block);
  next_next_block = _Heap_Block_at(next_block, next_block_size);
  next_is_used    = (next_block == the_heap->final) ||
                     _Heap_Is_prev_used(next_next_block);

  /* See _Heap_Size_of_user_area() source for explanations */
  old_user_size = _Addresses_Subtract(next_block, starting_address)
    + HEAP_BLOCK_HEADER_OFFSET;

  *old_mem_size = old_user_size;

  if (size > old_user_size) {
    /* Need to extend the block: allocate part of the next block and then
       merge 'the_block' and allocated block together. */
    if (next_is_used)    /* Next block is in use, -- no way to extend */
      return HEAP_RESIZE_UNSATISFIED;
    else {
      uint32_t add_block_size = size - old_user_size;
      _Heap_Align_up(&add_block_size, page_size);
      if (add_block_size < min_block_size)
        add_block_size = min_block_size;
      if (add_block_size > next_block_size)
        return HEAP_RESIZE_UNSATISFIED; /* Next block is too small or none. */
      add_block_size =
        _Heap_Block_allocate(the_heap, next_block, add_block_size);
      /* Merge two subsequent blocks */
      the_block->size = (old_block_size + add_block_size) | prev_used_flag;
      --stats->used_blocks;
    }
  } else {

    /* Calculate how much memory we could free */
    uint32_t free_block_size = old_user_size - size;
    _Heap_Align_down(&free_block_size, page_size);

    if (free_block_size > 0) {

      /* To free some memory the block should be shortened so that it can
         can hold 'size' user bytes and still remain not shorter than
         'min_block_size'. */

      uint32_t new_block_size = old_block_size - free_block_size;

      if (new_block_size < min_block_size) {
        uint32_t delta = min_block_size - new_block_size;
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
          _Heap_Block_at(the_block, new_block_size);
        uint32_t const new_next_block_size =
          next_block_size + free_block_size;
        _HAssert(_Heap_Is_block_in(the_heap, next_next_block));
        the_block->size = new_block_size | prev_used_flag;
        new_next_block->size = new_next_block_size | HEAP_PREV_USED;
        next_next_block->prev_size = new_next_block_size;
        _Heap_Block_replace(next_block, new_next_block);
        the_heap->stats.free_size += free_block_size;
        *avail_mem_size = new_next_block_size - HEAP_BLOCK_USED_OVERHEAD;

      } else if (free_block_size >= min_block_size) {
        /* Split the block into 2 used  parts, then free the second one. */
        the_block->size = new_block_size | prev_used_flag;
        next_block = _Heap_Block_at(the_block, new_block_size);
        next_block->size = free_block_size | HEAP_PREV_USED;
        ++stats->used_blocks; /* We have created used block */
        --stats->frees;       /* Don't count next call in stats */
        _Heap_Free(the_heap, _Heap_User_area(next_block));
        *avail_mem_size = free_block_size - HEAP_BLOCK_USED_OVERHEAD;
      }
    }
  }

  ++stats->resizes;
  return HEAP_RESIZE_SUCCESSFUL;
}
