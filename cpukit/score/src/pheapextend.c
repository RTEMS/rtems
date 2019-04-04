/**
 * @file
 *
 * @ingroup RTEMSScoreProtHeap
 *
 * @brief Protected Heap Handler implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/protectedheap.h>

bool _Protected_heap_Extend(
  Heap_Control *the_heap,
  void         *starting_address,
  uintptr_t     size
)
{
  uintptr_t amount_extended;

  _RTEMS_Lock_allocator();
    amount_extended = _Heap_Extend( the_heap, starting_address, size, 0 );
  _RTEMS_Unlock_allocator();

  return amount_extended != 0;
}
