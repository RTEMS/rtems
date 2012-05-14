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
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <rtems/score/heap.h>

static bool _Heap_Get_information_visitor(
  const Heap_Block *block __attribute__((unused)),
  uintptr_t block_size,
  bool block_is_used,
  void *visitor_arg
)
{
  Heap_Information_block *info_block = visitor_arg;
  Heap_Information *info = block_is_used ?
    &info_block->Used : &info_block->Free;

  ++info->number;
  info->total += block_size;
  if ( info->largest < block_size )
    info->largest = block_size;

  return false;
}

void _Heap_Get_information(
  Heap_Control            *the_heap,
  Heap_Information_block  *the_info
)
{
  memset( the_info, 0, sizeof(*the_info) );
  _Heap_Iterate( the_heap, _Heap_Get_information_visitor, the_info );
}
