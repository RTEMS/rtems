/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Copyright (c) 2012 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
