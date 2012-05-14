/**
 * @file
 *
 * @ingroup ScoreProtHeap
 *
 * @brief _Heap_Iterate() implementation.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/protectedheap.h>

void _Protected_heap_Iterate(
  Heap_Control *heap,
  Heap_Block_visitor visitor,
  void *visitor_arg
)
{
  _RTEMS_Lock_allocator();
  _Heap_Iterate( heap, visitor, visitor_arg );
  _RTEMS_Unlock_allocator();
}
