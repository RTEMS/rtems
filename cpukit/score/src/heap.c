/*
 *  Heap Handler
 *
 *  COPYRIGHT (c) 1989-2009.
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
#include <rtems/score/heap.h>

static uint32_t instance = 0;

/*PAGE
 *
 *  _Heap_Initialize
 *
 *  This kernel routine initializes a heap.
 *
 *  Input parameters:
 *    heap         - pointer to heap header
 *    area_begin - starting address of heap
 *    size             - size of heap
 *    page_size        - allocatable unit of memory
 *
 *  Output parameters:
 *    returns - maximum memory available if RTEMS_SUCCESSFUL
 *    0       - otherwise
 *
 *  This is what a heap looks like in memory immediately after initialization:
 *
 *
 *            +--------------------------------+ <- begin = area_begin
 *            |  unused space due to alignment |
 *            |       size < page_size         |
 *         0  +--------------------------------+ <- first block
 *            |  prev_size = page_size         |
 *         4  +--------------------------------+
 *            |  size = size0              | 1 |
 *         8  +---------------------+----------+ <- aligned on page_size
 *            |  next = HEAP_TAIL   |          |
 *        12  +---------------------+          |
 *            |  prev = HEAP_HEAD   |  memory  |
 *            +---------------------+          |
 *            |                     available  |
 *            |                                |
 *            |                for allocation  |
 *            |                                |
 *     size0  +--------------------------------+ <- last dummy block
 *            |  prev_size = size0             |
 *        +4  +--------------------------------+
 *            |  size = page_size          | 0 | <- prev block is free
 *        +8  +--------------------------------+ <- aligned on page_size
 *            |  unused space due to alignment |
 *            |       size < page_size         |
 *            +--------------------------------+ <- end = begin + size
 *
 *  Below is what a heap looks like after first allocation of SIZE bytes using
 *  _Heap_allocate(). BSIZE stands for SIZE + 4 aligned up on 'page_size'
 *  boundary.
 *  [NOTE: If allocation were performed by _Heap_Allocate_aligned(), the
 *  block size BSIZE is defined differently, and previously free block will
 *  be split so that upper part of it will become used block (see
 *  'heapallocatealigned.c' for details).]
 *
 *            +--------------------------------+ <- begin = area_begin
 *            |  unused space due to alignment |
 *            |       size < page_size         |
 *         0  +--------------------------------+ <- used block
 *            |  prev_size = page_size         |
 *         4  +--------------------------------+
 *            |  size = BSIZE              | 1 | <- prev block is used
 *         8  +--------------------------------+ <- aligned on page_size
 *            |              .                 | Pointer returned to the user
 *            |              .                 | is 8 for _Heap_Allocate()
 *            |              .                 | and is in range
 * 8 +        |         user-accessible        | [8,8+page_size) for
 *  page_size +- - -                      - - -+ _Heap_Allocate_aligned()
 *            |             area               |
 *            |              .                 |
 *     BSIZE  +- - - - -     .        - - - - -+ <- free block
 *            |              .                 |
 * BSIZE  +4  +--------------------------------+
 *            |  size = S = size0 - BSIZE  | 1 | <- prev block is used
 * BSIZE  +8  +-------------------+------------+ <- aligned on page_size
 *            |  next = HEAP_TAIL |            |
 * BSIZE +12  +-------------------+            |
 *            |  prev = HEAP_HEAD |     memory |
 *            +-------------------+            |
 *            |                   .  available |
 *            |                   .            |
 *            |                   .        for |
 *            |                   .            |
 * BSIZE +S+0 +-------------------+ allocation + <- last dummy block
 *            |  prev_size = S    |            |
 *       +S+4 +-------------------+------------+
 *            |  size = page_size          | 0 | <- prev block is free
 *       +S+8 +--------------------------------+ <- aligned on page_size
 *            |  unused space due to alignment |
 *            |       size < page_size         |
 *            +--------------------------------+ <- end = begin + size
 *
 */

uintptr_t _Heap_Initialize(
  Heap_Control *heap,
  void *area_begin,
  uintptr_t area_size,
  uintptr_t page_size
)
{
  Heap_Statistics * const stats = &heap->stats;
  uintptr_t heap_area_begin = (uintptr_t) area_begin;
  uintptr_t heap_area_end = heap_area_begin + area_size;
  uintptr_t alloc_area_begin = heap_area_begin + HEAP_BLOCK_ALLOC_AREA_OFFSET;
  uintptr_t alloc_area_size = 0;
  uintptr_t overhead = 0;
  Heap_Block *first_block = NULL;
  Heap_Block *second_block = NULL;

  if ( page_size == 0 ) {
    page_size = CPU_ALIGNMENT;
  } else {
    page_size = _Heap_Align_up( page_size, CPU_ALIGNMENT );
  }

  heap->min_block_size = _Heap_Align_up( sizeof( Heap_Block ), page_size );

  alloc_area_begin = _Heap_Align_up( alloc_area_begin, page_size );
  overhead = HEAP_LAST_BLOCK_OVERHEAD
    + (alloc_area_begin - HEAP_BLOCK_ALLOC_AREA_OFFSET - heap_area_begin);
  alloc_area_size = _Heap_Align_down ( area_size - overhead, page_size );

  if (
    heap_area_end < heap_area_begin
      || area_size < overhead
      || alloc_area_size == 0
  ) {
    /* Invalid area or area too small */
    return 0;
  }

  heap->page_size = page_size;
  heap->begin = heap_area_begin;
  heap->end = heap_area_end;

  /* First block */
  first_block = _Heap_Block_of_alloc_area( alloc_area_begin, page_size );
  first_block->prev_size = page_size;
  first_block->size_and_flag = alloc_area_size | HEAP_PREV_BLOCK_USED;
  first_block->next = _Heap_Free_list_tail( heap );
  first_block->prev = _Heap_Free_list_head( heap );
  _Heap_Free_list_head( heap )->next = first_block;
  _Heap_Free_list_tail( heap )->prev = first_block;
  heap->start = first_block;

  /* Second and last block */
  second_block = _Heap_Block_at( first_block, alloc_area_size );
  second_block->prev_size = alloc_area_size;
  second_block->size_and_flag = page_size | HEAP_PREV_BLOCK_FREE;
  heap->final = second_block;

  /* Statistics */
  stats->size = area_size;
  stats->free_size = alloc_area_size;
  stats->min_free_size = alloc_area_size;
  stats->free_blocks = 1;
  stats->max_free_blocks = 1;
  stats->used_blocks = 0;
  stats->max_search = 0;
  stats->allocs = 0;
  stats->searches = 0;
  stats->frees = 0;
  stats->resizes = 0;
  stats->instance = instance++;

  _HAssert( _Heap_Is_aligned( CPU_ALIGNMENT, 4 ));
  _HAssert( _Heap_Is_aligned( heap->page_size, CPU_ALIGNMENT ));
  _HAssert( _Heap_Is_aligned( heap->min_block_size, page_size ));
  _HAssert(
    _Heap_Is_aligned( _Heap_Alloc_area_of_block( first_block ), page_size )
  );
  _HAssert(
    _Heap_Is_aligned( _Heap_Alloc_area_of_block( second_block ), page_size )
  );

  return alloc_area_size;
}

uintptr_t _Heap_Calc_block_size(
  uintptr_t alloc_size,
  uintptr_t page_size,
  uintptr_t min_block_size)
{
  uintptr_t block_size =
    _Heap_Align_up( alloc_size + HEAP_BLOCK_USED_OVERHEAD, page_size );

  if (block_size < min_block_size) {
    block_size = min_block_size;
  }

  if (block_size > alloc_size) {
    return block_size;
  } else {
    /* Integer overflow occured */
    return 0;
  }
}

uintptr_t _Heap_Block_allocate(
  Heap_Control *heap,
  Heap_Block *block,
  uintptr_t alloc_size
)
{
  Heap_Statistics * const stats = &heap->stats;
  uintptr_t const block_size = _Heap_Block_size( block );
  uintptr_t const unused_size = block_size - alloc_size;
  Heap_Block *next_block = _Heap_Block_at( block, block_size );

  _HAssert( _Heap_Is_aligned( block_size, heap->page_size ));
  _HAssert( _Heap_Is_aligned( alloc_size, heap->page_size ));
  _HAssert( alloc_size <= block_size );
  _HAssert( _Heap_Is_prev_used( block ));

  if (unused_size >= heap->min_block_size) {
    /*
     * Split the block so that the upper part is still free, and the lower part
     * becomes used.  This is slightly less optimal than leaving the lower part
     * free as it requires replacing block in the free blocks list, but it
     * makes it possible to reuse this code in the _Heap_Resize_block().
     */
    Heap_Block *new_block = _Heap_Block_at( block, alloc_size );
    block->size_and_flag = alloc_size | HEAP_PREV_BLOCK_USED;
    new_block->size_and_flag = unused_size | HEAP_PREV_BLOCK_USED;
    next_block->prev_size = unused_size;
    _Heap_Block_replace_in_free_list( block, new_block );
  } else {
    next_block->size_and_flag |= HEAP_PREV_BLOCK_USED;
    alloc_size = block_size;
    _Heap_Block_remove_from_free_list( block );

    /* Statistics */
    --stats->free_blocks;
  }

  /* Statistics */
  ++stats->used_blocks;
  stats->free_size -= alloc_size;
  if(stats->min_free_size > stats->free_size) {
    stats->min_free_size = stats->free_size;
  }

  return alloc_size;
}
