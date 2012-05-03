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

void *_Protected_heap_Allocate_aligned_with_boundary(
  Heap_Control *heap,
  uintptr_t     size,
  uintptr_t     alignment,
  uintptr_t     boundary
)
{
  void *p;

  _RTEMS_Lock_allocator();
    p = _Heap_Allocate_aligned_with_boundary(
      heap,
      size,
      alignment,
      boundary
    );
  _RTEMS_Unlock_allocator();

  return p;
}
