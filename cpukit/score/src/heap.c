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

static uint32_t instance = 0;

/*PAGE
 *
 *  _Heap_Initialize
 *
 *  This kernel routine initializes a heap.
 *
 *  Input parameters:
 *    the_heap         - pointer to heap header
 *    starting_address - starting address of heap
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
 *            +--------------------------------+ <- begin = starting_address
 *            |  unused space due to alignment |
 *            |       size < page_size         |
 *         0  +--------------------------------+ <- first block
 *            |  prev_size = 1 (arbitrary)     |
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
 *            |  size = 0 (arbitrary)      | 0 | <- prev block is free
 *        +8  +--------------------------------+ <- aligned on page_size
 *            |  unused space due to alignment |
 *            |       size < page_size         |
 *            +--------------------------------+ <- end = begin + size
 *
 *  This is what a heap looks like after first allocation of SIZE bytes.
 *  BSIZE stands for SIZE + 4 aligned up on 'page_size' boundary if allocation
 *  has been performed by _Heap_Allocate(). If allocation has been performed
 *  by _Heap_Allocate_aligned(), the block size BSIZE is defined differently
 *  (see 'heapallocatealigned.c' for details).
 *
 *            +--------------------------------+ <- begin = starting_address
 *            |  unused space due to alignment |
 *            |       size < page_size         |
 *         0  +--------------------------------+ <- first block
 *            |  prev_size = 1 (arbitrary)     |
 *         4  +--------------------------------+
 *            |  size = S = size0 - BSIZE  | 1 |
 *         8  +---------------------+----------+ <- aligned on page_size
 *            |  next = HEAP_TAIL   |          |
 *        12  +---------------------+          |
 *            |  prev = HEAP_HEAD   |  memory  |
 *            +---------------------+          |
 *            |                     available  |
 *            |                                |
 *            |                for allocation  |
 *            |                                |
 *         S  +--------------------------------+ <- used block
 *            |  prev_size = size0 - BSIZE     |
 *        +4  +--------------------------------+
 *            |  size = BSIZE              | 0 | <- prev block is free
 *        +8  +--------------------------------+ <- aligned on page_size
 *            |              .                 | Pointer returned to the user
 *            |              .                 | is (S+8) for _Heap_Allocate()
 *            |              .                 | and is in range
 * S + 8 +    |         user-accessible        | [S+8,S+8+page_size) for
 *   page_size+- - -                      - - -+ _Heap_Allocate_aligned()
 *            |             area               |
 *            |              .                 |
 * S + BSIZE  +- - - - -     .        - - - - -+ <- last dummy block
 *            |              .                 |
 *        +4  +--------------------------------+
 *            |  size = 0 (arbitrary)      | 1 | <- prev block is used
 *        +8  +--------------------------------+ <- aligned on page_size
 *            |  unused space due to alignment |
 *            |       size < page_size         |
 *            +--------------------------------+ <- end = begin + size
 *
 */

uint32_t   _Heap_Initialize(
  Heap_Control        *the_heap,
  void                *starting_address,
  uint32_t             size,
  uint32_t             page_size
)
{
  Heap_Block *the_block;
  uint32_t  the_size;
  _H_uptr_t   start;
  _H_uptr_t   aligned_start;
  uint32_t  overhead;
  Heap_Statistics *const stats = &the_heap->stats;

  if(page_size == 0)
    page_size = CPU_ALIGNMENT;
  else
    _Heap_Align_up( &page_size, CPU_ALIGNMENT );

  /* Calculate aligned_start so that aligned_start + HEAP_BLOCK_USER_OFFSET
     (value of user pointer) is aligned on 'page_size' boundary. Make sure
     resulting 'aligned_start' is not below 'starting_address'. */
  start = _H_p2u(starting_address);
  aligned_start = start + HEAP_BLOCK_USER_OFFSET;
  _Heap_Align_up_uptr ( &aligned_start, page_size );
  aligned_start -= HEAP_BLOCK_USER_OFFSET;

  /* Calculate 'min_block_size'. It's HEAP_MIN_BLOCK_SIZE aligned up to the
     nearest multiple of 'page_size'. */
  the_heap->min_block_size = HEAP_MIN_BLOCK_SIZE;
  _Heap_Align_up ( &the_heap->min_block_size, page_size );

  /* Calculate 'the_size' -- size of the first block so that there is enough
     space at the end for the permanent last block. It is equal to 'size'
     minus total overhead aligned down to the nearest multiple of
     'page_size'. */
  overhead = HEAP_OVERHEAD + (aligned_start - start);
  if ( size < overhead )
    return 0;   /* Too small area for the heap */
  the_size = size - overhead;
  _Heap_Align_down ( &the_size, page_size );
  if ( the_size == 0 )
    return 0;   /* Too small area for the heap */

  the_heap->page_size = page_size;
  the_heap->begin = starting_address;
  the_heap->end = starting_address + size;

  the_block = (Heap_Block *) aligned_start;

  the_block->prev_size = HEAP_PREV_USED;
  the_block->size = the_size | HEAP_PREV_USED;
  the_block->next = _Heap_Tail( the_heap );
  the_block->prev = _Heap_Head( the_heap );
  _Heap_Head(the_heap)->next = the_block;
  _Heap_Tail(the_heap)->prev = the_block;
  the_heap->start = the_block;

  _HAssert(_Heap_Is_aligned(the_heap->page_size, CPU_ALIGNMENT));
  _HAssert(_Heap_Is_aligned(the_heap->min_block_size, page_size));
  _HAssert(_Heap_Is_aligned_ptr(_Heap_User_area(the_block), page_size));

  the_block = _Heap_Block_at( the_block, the_size );
  the_heap->final = the_block;       /* Permanent final block of the heap */
  the_block->prev_size = the_size;   /* Previous block is free */
  the_block->size = 0;  /* This is the only block with size=0  */

  stats->size = size;
  stats->free_size = the_size;
  stats->min_free_size = the_size;
  stats->free_blocks = 1;
  stats->max_free_blocks = 1;
  stats->used_blocks = 0;
  stats->max_search = 0;
  stats->allocs = 0;
  stats->searches = 0;
  stats->frees = 0;
  stats->instance = instance++;

  return ( the_size - HEAP_BLOCK_USED_OVERHEAD );
}

/*PAGE
 *
 * Internal routines shared by _Heap_Allocate() and _Heap_Allocate_aligned().
 *
 * Note: there is no reason to put them into a separate file(s) as they are
 * always required for heap to be usefull.
 *
 */

/*
 * Convert user requested 'size' of memory block to the block size.
 * Return block size on success, 0 if overflow occured
 */
uint32_t _Heap_Calc_block_size(
  uint32_t size,
  uint32_t page_size,
  uint32_t min_size)
{
  uint32_t block_size = size + HEAP_BLOCK_USED_OVERHEAD;
  _Heap_Align_up(&block_size, page_size);
  if(block_size < min_size) block_size = min_size;
  return (block_size > size) ? block_size : 0;
}

/*
 * Allocate block of size 'alloc_size' from 'the_block' belonging to
 * 'the_heap'. Either split 'the_block' or allocate it entirely.
 * Return the block allocated.
 */
Heap_Block* _Heap_Block_allocate(
  Heap_Control* the_heap,
  Heap_Block* the_block,
  uint32_t alloc_size)
{
  Heap_Statistics *const stats = &the_heap->stats;
  uint32_t const block_size = _Heap_Block_size(the_block);
  uint32_t const the_rest = block_size - alloc_size;

  _HAssert(_Heap_Is_aligned(block_size, the_heap->page_size));
  _HAssert(_Heap_Is_aligned(alloc_size, the_heap->page_size));
  _HAssert(alloc_size <= block_size);

  if(the_rest >= the_heap->min_block_size) {
    /* Split the block so that lower part is still free, and upper part
       becomes used. */
    the_block->size = the_rest | HEAP_PREV_USED;
    the_block = _Heap_Block_at(the_block, the_rest);
    the_block->prev_size = the_rest;
    the_block->size = alloc_size;
  }
  else {
    /* Don't split the block as remainder is either zero or too small to be
       used as a separate free block. Change 'alloc_size' to the size of the
       block and remove the block from the list of free blocks. */
    _Heap_Block_remove(the_block);
    alloc_size = block_size;
    stats->free_blocks -= 1;
  }
  /* Mark the block as used (in the next block). */
  _Heap_Block_at(the_block, alloc_size)->size |= HEAP_PREV_USED;
  /* Update statistics */
  stats->free_size -= alloc_size;
  if(stats->min_free_size > stats->free_size)
    stats->min_free_size = stats->free_size;
  stats->used_blocks += 1;
  return the_block;
}
