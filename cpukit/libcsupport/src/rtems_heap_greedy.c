/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Greedy Allocate that Empties the Heap
 *  @ingroup MallocSupport
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
