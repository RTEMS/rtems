/**
 * @file
 *
 * @ingroup ScoreHeap
 *
 * @brief Heap Handler implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
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
#include <rtems/score/heap.h>

#if CPU_ALIGNMENT == 0 || CPU_ALIGNMENT % 2 != 0
  #error "invalid CPU_ALIGNMENT value"
#endif

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
  void *heap_area_begin_ptr,
  uintptr_t heap_area_size,
  uintptr_t page_size
)
{
  Heap_Statistics *const stats = &heap->stats;
  uintptr_t const heap_area_begin = (uintptr_t) heap_area_begin_ptr;
  uintptr_t const heap_area_end = heap_area_begin + heap_area_size;
  uintptr_t alloc_area_begin = heap_area_begin + HEAP_BLOCK_HEADER_SIZE;
  uintptr_t alloc_area_size = 0;
  uintptr_t first_block_begin = 0;
  uintptr_t first_block_size = 0;
  uintptr_t min_block_size = 0;
  uintptr_t overhead = 0;
  Heap_Block *first_block = NULL;
  Heap_Block *last_block = NULL;

  if ( page_size == 0 ) {
    page_size = CPU_ALIGNMENT;
  } else {
    page_size = _Heap_Align_up( page_size, CPU_ALIGNMENT );

    if ( page_size < CPU_ALIGNMENT ) {
      /* Integer overflow */
      return 0;
    }
  }
  min_block_size = _Heap_Align_up( sizeof( Heap_Block ), page_size );

  alloc_area_begin = _Heap_Align_up( alloc_area_begin, page_size );
  first_block_begin = alloc_area_begin - HEAP_BLOCK_HEADER_SIZE;
  overhead = HEAP_BLOCK_HEADER_SIZE + (first_block_begin - heap_area_begin);
  first_block_size = heap_area_size - overhead;
  first_block_size = _Heap_Align_down ( first_block_size, page_size );
  alloc_area_size = first_block_size - HEAP_BLOCK_HEADER_SIZE;

  if (
    heap_area_end < heap_area_begin
      || heap_area_size <= overhead
      || first_block_size < min_block_size
  ) {
    /* Invalid area or area too small */
    return 0;
  }

  /* First block */
  first_block = (Heap_Block *) first_block_begin;
  first_block->prev_size = page_size;
  first_block->size_and_flag = first_block_size | HEAP_PREV_BLOCK_USED;
  first_block->next = _Heap_Free_list_tail( heap );
  first_block->prev = _Heap_Free_list_head( heap );

  /*
   * Last block.
   *
   * The next block of the last block is the first block.  Since the first
   * block indicates that the previous block is used, this ensures that the
   * last block appears as used for the _Heap_Is_used() and _Heap_Is_free()
   * functions.
   */
  last_block = _Heap_Block_at( first_block, first_block_size );
  last_block->prev_size = first_block_size;
  last_block->size_and_flag = first_block_begin - (uintptr_t) last_block;

  /* Heap control */
  heap->page_size = page_size;
  heap->min_block_size = min_block_size;
  heap->area_begin = heap_area_begin;
  heap->area_end = heap_area_end;
  heap->first_block = first_block;
  heap->last_block = last_block;
  _Heap_Free_list_head( heap )->next = first_block;
  _Heap_Free_list_tail( heap )->prev = first_block;

  /* Statistics */
  stats->size = first_block_size;
  stats->free_size = first_block_size;
  stats->min_free_size = first_block_size;
  stats->free_blocks = 1;
  stats->max_free_blocks = 1;
  stats->used_blocks = 0;
  stats->max_search = 0;
  stats->allocs = 0;
  stats->searches = 0;
  stats->frees = 0;
  stats->resizes = 0;
  stats->instance = instance++;

  _HAssert( _Heap_Is_aligned( heap->page_size, CPU_ALIGNMENT ) );
  _HAssert( _Heap_Is_aligned( heap->min_block_size, page_size ) );
  _HAssert(
    _Heap_Is_aligned( _Heap_Alloc_area_of_block( first_block ), page_size )
  );
  _HAssert(
    _Heap_Is_aligned( _Heap_Alloc_area_of_block( last_block ), page_size )
  );

  return alloc_area_size;
}

void _Heap_Block_split(
  Heap_Control *heap,
  Heap_Block *block,
  Heap_Block *free_list_anchor,
  uintptr_t alloc_size
)
{
  Heap_Statistics *const stats = &heap->stats;

  uintptr_t const page_size = heap->page_size;
  uintptr_t const min_block_size = heap->min_block_size;
  uintptr_t const min_alloc_size = min_block_size - HEAP_BLOCK_HEADER_SIZE;

  uintptr_t const block_size = _Heap_Block_size( block );

  uintptr_t const used_size =
    _Heap_Max( alloc_size, min_alloc_size ) + HEAP_BLOCK_HEADER_SIZE;
  uintptr_t const used_block_size = _Heap_Align_up( used_size, page_size );

  uintptr_t const free_size = block_size + HEAP_BLOCK_SIZE_OFFSET - used_size;
  uintptr_t const free_size_limit = min_block_size + HEAP_BLOCK_SIZE_OFFSET;

  Heap_Block *next_block = _Heap_Block_at( block, block_size );

  _HAssert( used_size <= block_size + HEAP_BLOCK_SIZE_OFFSET );
  _HAssert( used_size + free_size == block_size + HEAP_BLOCK_SIZE_OFFSET );

  if ( free_size >= free_size_limit ) {
    Heap_Block *const free_block = _Heap_Block_at( block, used_block_size );
    uintptr_t free_block_size = block_size - used_block_size;

    _HAssert( used_block_size + free_block_size == block_size );

    _Heap_Block_set_size( block, used_block_size );

    /* Statistics */
    stats->free_size += free_block_size;

    if ( _Heap_Is_used( next_block ) ) {
      _Heap_Free_list_insert_after( free_list_anchor, free_block );

      /* Statistics */
      ++stats->free_blocks;
    } else {
      uintptr_t const next_block_size = _Heap_Block_size( next_block );

      _Heap_Free_list_replace( next_block, free_block );

      free_block_size += next_block_size;

      next_block = _Heap_Block_at( free_block, free_block_size );
    }

    free_block->size_and_flag = free_block_size | HEAP_PREV_BLOCK_USED;

    next_block->prev_size = free_block_size;
    next_block->size_and_flag &= ~HEAP_PREV_BLOCK_USED;
  } else {
    next_block->size_and_flag |= HEAP_PREV_BLOCK_USED;
  }
}

static Heap_Block *_Heap_Block_allocate_from_begin(
  Heap_Control *heap,
  Heap_Block *block,
  Heap_Block *free_list_anchor,
  uintptr_t alloc_size
)
{
  _Heap_Block_split( heap, block, free_list_anchor, alloc_size );

  return block;
}

static Heap_Block *_Heap_Block_allocate_from_end(
  Heap_Control *heap,
  Heap_Block *block,
  Heap_Block *free_list_anchor,
  uintptr_t alloc_begin,
  uintptr_t alloc_size
)
{
  Heap_Statistics *const stats = &heap->stats;

  uintptr_t block_begin = (uintptr_t) block;
  uintptr_t block_size = _Heap_Block_size( block );
  uintptr_t block_end = block_begin + block_size;

  Heap_Block *const new_block =
    _Heap_Block_of_alloc_area( alloc_begin, heap->page_size );
  uintptr_t const new_block_begin = (uintptr_t) new_block;
  uintptr_t const new_block_size = block_end - new_block_begin;

  block_end = new_block_begin;
  block_size = block_end - block_begin;

  _HAssert( block_size >= heap->min_block_size );
  _HAssert( new_block_size >= heap->min_block_size );

  /* Statistics */
  stats->free_size += block_size;

  if ( _Heap_Is_prev_used( block ) ) {
    _Heap_Free_list_insert_after( free_list_anchor, block );

    free_list_anchor = block;

    /* Statistics */
    ++stats->free_blocks;
  } else {
    Heap_Block *const prev_block = _Heap_Prev_block( block );
    uintptr_t const prev_block_size = _Heap_Block_size( prev_block );

    block = prev_block;
    block_begin = (uintptr_t) block;
    block_size += prev_block_size;
  }

  block->size_and_flag = block_size | HEAP_PREV_BLOCK_USED;

  new_block->prev_size = block_size;
  new_block->size_and_flag = new_block_size;

  _Heap_Block_split( heap, new_block, free_list_anchor, alloc_size );

  return new_block;
}

Heap_Block *_Heap_Block_allocate(
  Heap_Control *heap,
  Heap_Block *block,
  uintptr_t alloc_begin,
  uintptr_t alloc_size
)
{
  Heap_Statistics *const stats = &heap->stats;

  uintptr_t const alloc_area_begin = _Heap_Alloc_area_of_block( block );
  uintptr_t const alloc_area_offset = alloc_begin - alloc_area_begin;

  Heap_Block *free_list_anchor = NULL;

  _HAssert( alloc_area_begin <= alloc_begin );

  if ( _Heap_Is_free( block ) ) {
    free_list_anchor = block->prev;

    _Heap_Free_list_remove( block );

    /* Statistics */
    --stats->free_blocks;
    ++stats->used_blocks;
    stats->free_size -= _Heap_Block_size( block );
  } else {
    free_list_anchor = _Heap_Free_list_head( heap );
  }

  if ( alloc_area_offset < heap->page_size ) {
    alloc_size += alloc_area_offset;

    block = _Heap_Block_allocate_from_begin(
      heap,
      block,
      free_list_anchor,
      alloc_size
    );
  } else {
    block = _Heap_Block_allocate_from_end(
      heap,
      block,
      free_list_anchor,
      alloc_begin,
      alloc_size
    );
  }

  /* Statistics */
  if ( stats->min_free_size > stats->free_size ) {
    stats->min_free_size = stats->free_size;
  }

  return block;
}
