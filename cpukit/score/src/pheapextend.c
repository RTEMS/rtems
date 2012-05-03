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

bool _Protected_heap_Extend(
  Heap_Control *the_heap,
  void         *starting_address,
  uintptr_t     size
)
{
  bool      extend_ok;
  uintptr_t amount_extended;

  _RTEMS_Lock_allocator();
    extend_ok = _Heap_Extend(the_heap, starting_address, size, &amount_extended);
  _RTEMS_Unlock_allocator();
  return extend_ok;
}

