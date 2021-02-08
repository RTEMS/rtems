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

void *rtems_heap_greedy_allocate(
  const uintptr_t *block_sizes,
  size_t block_count
)
{
  Heap_Control *heap = RTEMS_Malloc_Heap;
  size_t size = 128 * 1024 * 1024;
  void *opaque;

  while ( size > 0 ) {
    opaque = (*rtems_malloc_extend_handler)( heap, size );
    if ( opaque == NULL ) {
      size >>= 1;
    } else {
      if ( rtems_malloc_dirty_helper != NULL ) {
	(*rtems_malloc_dirty_helper)( opaque, size );
      }
    }
  }

  _RTEMS_Lock_allocator();
  opaque = _Heap_Greedy_allocate( RTEMS_Malloc_Heap, block_sizes, block_count );
  _RTEMS_Unlock_allocator();

  return opaque;
}

void *rtems_heap_greedy_allocate_all_except_largest(
  uintptr_t *allocatable_size
)
{
  void *opaque;

  _RTEMS_Lock_allocator();
  opaque = _Heap_Greedy_allocate_all_except_largest(
    RTEMS_Malloc_Heap,
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
