/**
 * @file
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This source file contains the implementation of
 *   rtems_workspace_greedy_allocate(),
 *   rtems_workspace_greedy_allocate_all_except_largest(), and
 *   rtems_workspace_greedy_free().
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

#include <rtems/rtems/support.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/heapimpl.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/wkspace.h>
#include <rtems/malloc.h>

#define SBRK_ALLOC_SIZE (128 * 1024UL * 1024UL)

void *rtems_workspace_greedy_allocate(
  const uintptr_t *block_sizes,
  size_t block_count
)
{
  Heap_Control *heap = &_Workspace_Area;
  void *opaque;

  rtems_heap_sbrk_greedy_allocate( heap, SBRK_ALLOC_SIZE );

  _RTEMS_Lock_allocator();
  opaque = _Heap_Greedy_allocate( heap, block_sizes, block_count );
  _RTEMS_Unlock_allocator();

  return opaque;
}

void *rtems_workspace_greedy_allocate_all_except_largest(
  uintptr_t *allocatable_size
)
{
  Heap_Control *heap = &_Workspace_Area;
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

void rtems_workspace_greedy_free( void *opaque )
{
  _RTEMS_Lock_allocator();
  _Heap_Greedy_free( &_Workspace_Area, opaque );
  _RTEMS_Unlock_allocator();
}
