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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <rtems/system.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/heap.h>

void _Heap_Get_information(
  Heap_Control            *the_heap,
  Heap_Information_block  *the_info
)
{
  Heap_Block *the_block = the_heap->first_block;
  Heap_Block *const end = the_heap->last_block;

  memset(the_info, 0, sizeof(*the_info));

  while ( the_block != end ) {
    uintptr_t const     the_size = _Heap_Block_size(the_block);
    Heap_Block *const  next_block = _Heap_Block_at(the_block, the_size);
    Heap_Information  *info;

    if ( _Heap_Is_prev_used(next_block) )
      info = &the_info->Used;
    else
      info = &the_info->Free;

    info->number++;
    info->total += the_size;
    if ( info->largest < the_size )
      info->largest = the_size;

    the_block = next_block;
  }
}
