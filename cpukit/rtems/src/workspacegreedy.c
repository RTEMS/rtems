/**
 *  @file
 *
 *  @brief Greedy Allocate that Empties the Workspace and Free
 *  @ingroup ClassicRTEMS
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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/rtems/support.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/heapimpl.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/wkspace.h>

void *rtems_workspace_greedy_allocate(
  const uintptr_t *block_sizes,
  size_t block_count
)
{
  void *opaque;

  _RTEMS_Lock_allocator();
  opaque = _Heap_Greedy_allocate( &_Workspace_Area, block_sizes, block_count );
  _RTEMS_Unlock_allocator();

  return opaque;
}

void *rtems_workspace_greedy_allocate_all_except_largest(
  uintptr_t *allocatable_size
)
{
  void *opaque;

  _RTEMS_Lock_allocator();
  opaque = _Heap_Greedy_allocate_all_except_largest(
    &_Workspace_Area,
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
