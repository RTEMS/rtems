/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
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

#include "malloc_p.h"

void *rtems_heap_greedy_allocate(
  const uintptr_t *block_sizes,
  size_t block_count
)
{
  void *opaque;

  _RTEMS_Lock_allocator();
  opaque = _Heap_Greedy_allocate( RTEMS_Malloc_Heap, block_sizes, block_count );
  _RTEMS_Unlock_allocator();

  return opaque;
}

void rtems_heap_greedy_free( void *opaque )
{
  _RTEMS_Lock_allocator();
  _Heap_Greedy_free( RTEMS_Malloc_Heap, opaque );
  _RTEMS_Unlock_allocator();
}
