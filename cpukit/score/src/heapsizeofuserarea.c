/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreHeap
 *
 * @brief This source file contains the implementation of
 *   _Heap_Size_of_alloc_area().
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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

#include <rtems/score/heapimpl.h>

bool _Heap_Size_of_alloc_area(
  Heap_Control *heap,
  void *alloc_begin_ptr,
  uintptr_t *alloc_size
)
{
  uintptr_t const page_size = heap->page_size;
  uintptr_t const alloc_begin = (uintptr_t) alloc_begin_ptr;
  Heap_Block *block = _Heap_Block_of_alloc_area( alloc_begin, page_size );
  Heap_Block *next_block = NULL;
  uintptr_t block_size = 0;

  if ( !_Heap_Is_block_in_heap( heap, block ) ) {
    return false;
  }

  block_size = _Heap_Block_size( block );
  next_block = _Heap_Block_at( block, block_size );

  if (
    !_Heap_Is_block_in_heap( heap, next_block )
      || !_Heap_Is_prev_used( next_block )
  ) {
    return false;
  }

  *alloc_size = (uintptr_t) next_block + HEAP_ALLOC_BONUS - alloc_begin;

  return true;
}

