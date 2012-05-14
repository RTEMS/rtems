/**
 * @file
 *
 * @ingroup ScoreHeap
 *
 * @brief Heap Handler implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
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

void _Heap_Get_free_information(
  Heap_Control        *the_heap,
  Heap_Information    *info
)
{
  Heap_Block *the_block;
  Heap_Block *const tail = _Heap_Free_list_tail(the_heap);

  info->number = 0;
  info->largest = 0;
  info->total = 0;

  for(the_block = _Heap_Free_list_first(the_heap);
      the_block != tail;
      the_block = the_block->next)
  {
    uint32_t const the_size = _Heap_Block_size(the_block);

    /* As we always coalesce free blocks, prev block must have been used. */
    _HAssert(_Heap_Is_prev_used(the_block));

    info->number++;
    info->total += the_size;
    if ( info->largest < the_size )
        info->largest = the_size;
  }
}
