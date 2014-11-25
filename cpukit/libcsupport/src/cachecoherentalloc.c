/**
 * @file
 *
 * @ingroup ClassicCache
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems.h>
#include <rtems/malloc.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/heapimpl.h>
#include <rtems/score/sysstate.h>

static Heap_Control cache_coherent_heap_instance;

static Heap_Control *cache_coherent_heap;

void *rtems_cache_coherent_allocate(
  size_t size,
  uintptr_t alignment,
  uintptr_t boundary
)
{
  void *ptr;
  Heap_Control *heap;

  _RTEMS_Lock_allocator();

  heap = cache_coherent_heap;
  if ( heap == NULL ) {
    heap = RTEMS_Malloc_Heap;
  }

  ptr = _Heap_Allocate_aligned_with_boundary(
    heap,
    size,
    alignment,
    boundary
  );

  _RTEMS_Unlock_allocator();

  return ptr;
}

void rtems_cache_coherent_free( void *ptr )
{
  Heap_Control *heap;

  _RTEMS_Lock_allocator();

  heap = cache_coherent_heap;
  if ( heap != NULL ) {
    if ( _Heap_Free( heap, ptr ) ) {
      heap = NULL;
    } else {
      heap = RTEMS_Malloc_Heap;
    }
  } else {
    heap = RTEMS_Malloc_Heap;
  }

  if ( heap != NULL ) {
    _Heap_Free( heap, ptr );
  }

  _RTEMS_Unlock_allocator();
}

static void add_area(
  void *area_begin,
  uintptr_t area_size
)
{
  Heap_Control *heap = cache_coherent_heap;

  if ( heap == NULL ) {
    bool ok;

    heap = &cache_coherent_heap_instance;

    ok = _Heap_Initialize( heap, area_begin, area_size, 0 );
    if ( ok ) {
      cache_coherent_heap = heap;
    }
  } else {
    _Heap_Extend( heap, area_begin, area_size, 0 );
  }
}

void rtems_cache_coherent_add_area(
  void *area_begin,
  uintptr_t area_size
)
{
  if ( _System_state_Is_up( _System_state_Get()) ) {
    _RTEMS_Lock_allocator();

    add_area( area_begin, area_size );

    _RTEMS_Unlock_allocator();
  } else {
    add_area( area_begin, area_size );
  }
}
