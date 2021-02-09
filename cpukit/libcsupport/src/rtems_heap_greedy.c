/**
 *  @file
 *
 *  @brief Greedy Allocate that Empties the Heap
 *  @ingroup MallocSupport
 */

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
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "malloc_p.h"

#define SBRK_ALLOC_SIZE (128 * 1024UL * 1024UL)

void *rtems_heap_greedy_allocate(
  const uintptr_t *block_sizes,
  size_t block_count
)
{
  Heap_Control *heap = RTEMS_Malloc_Heap;
  void *opaque;

  rtems_heap_sbrk_greedy_allocate( heap, SBRK_ALLOC_SIZE );

  _RTEMS_Lock_allocator();
  opaque = _Heap_Greedy_allocate( RTEMS_Malloc_Heap, block_sizes, block_count );
  _RTEMS_Unlock_allocator();

  return opaque;
}

void *rtems_heap_greedy_allocate_all_except_largest(
  uintptr_t *allocatable_size
)
{
  Heap_Control *heap = RTEMS_Malloc_Heap;
  void *opaque;

  rtems_heap_sbrk_greedy_allocate( heap, SBRK_ALLOC_SIZE );

  _RTEMS_Lock_allocator();
  opaque = _Heap_Greedy_allocate_all_except_largest(
    heap,
    allocatable_size
  );
  _RTEMS_Unlock_allocator();

  return opaque;
}

void rtems_heap_greedy_free( void *opaque )
{
  _RTEMS_Lock_allocator();
  _Heap_Greedy_free( RTEMS_Malloc_Heap, opaque );
  _RTEMS_Unlock_allocator();
}
