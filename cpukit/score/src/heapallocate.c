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

/*PAGE
 *
 *  _Heap_Allocate
 *
 *  This kernel routine allocates the requested size of memory
 *  from the specified heap.
 *
 *  Input parameters:
 *    the_heap  - pointer to heap header.
 *    size      - size in bytes of the memory block to allocate.
 *
 *  Output parameters:
 *    returns - starting address of memory block allocated
 */

void *_Heap_Allocate(
  Heap_Control        *the_heap,
  ssize_t              size
)
{
  uint32_t  the_size;
  uint32_t  search_count;
  Heap_Block *the_block;
  void       *ptr = NULL;
  Heap_Statistics *const stats = &the_heap->stats;
  Heap_Block *const tail = _Heap_Tail(the_heap);

  the_size =
    _Heap_Calc_block_size(size, the_heap->page_size, the_heap->min_block_size);
  if(the_size == 0)
    return NULL;

  /* Find large enough free block. */
  for(the_block = _Heap_First(the_heap), search_count = 0;
      the_block != tail;
      the_block = the_block->next, ++search_count)
  {
    /* As we always coalesce free blocks, prev block must have been used. */
    _HAssert(_Heap_Is_prev_used(the_block));

    /* Don't bother to mask out the HEAP_PREV_USED bit as it won't change the
       result of the comparison. */
    if(the_block->size >= the_size) {
      (void)_Heap_Block_allocate(the_heap, the_block, the_size );

      ptr = _Heap_User_area(the_block);

      stats->allocs += 1;
      stats->searches += search_count + 1;

      _HAssert(_Heap_Is_aligned_ptr(ptr, the_heap->page_size));
      break;
    }
  }

  if(stats->max_search < search_count)
    stats->max_search = search_count;

  return ptr;
}
