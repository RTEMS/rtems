/**
 * @file
 *
 * @ingroup ScoreProtHeap
 *
 * @brief Protected Heap Handler implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/score/protectedheap.h>

bool _Protected_heap_Resize_block(
  Heap_Control *the_heap,
  void         *starting_address,
  uintptr_t     size
)
{
  Heap_Resize_status status;
  uintptr_t          old_mem_size;
  uintptr_t          avail_mem_size;

  _RTEMS_Lock_allocator();
    status = _Heap_Resize_block(
      the_heap, starting_address, size, &old_mem_size, &avail_mem_size );
  _RTEMS_Unlock_allocator();
  return (status == HEAP_RESIZE_SUCCESSFUL);
}

