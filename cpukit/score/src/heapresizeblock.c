/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreHeap
 *
 * @brief This source file contains the implementation of
 *   _Heap_Resize_block().
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2009 embedded brains GmbH & Co. KG
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

static Heap_Resize_status _Heap_Resize_block_checked(
  Heap_Control *heap,
  Heap_Block *block,
  uintptr_t alloc_begin,
  uintptr_t new_alloc_size,
  uintptr_t *old_size,
  uintptr_t *new_size
)
{
  Heap_Statistics *const stats = &heap->stats;

  uintptr_t const block_begin = (uintptr_t) block;
  uintptr_t block_size = _Heap_Block_size( block );
  uintptr_t block_end = block_begin + block_size;

  uintptr_t alloc_size = block_end - alloc_begin + HEAP_ALLOC_BONUS;

  Heap_Block *next_block = _Heap_Block_at( block, block_size );
  uintptr_t next_block_size = _Heap_Block_size( next_block );
  bool next_block_is_free = _Heap_Is_free( next_block );

  _HAssert( _Heap_Is_block_in_heap( heap, next_block ) );
  _HAssert( _Heap_Is_prev_used( next_block ) );

  *old_size = alloc_size;

  if ( next_block_is_free ) {
    block_size += next_block_size;
    alloc_size += next_block_size;
  }

  if ( new_alloc_size > alloc_size ) {
    return HEAP_RESIZE_UNSATISFIED;
  }

  if ( next_block_is_free ) {
    _Heap_Block_set_size( block, block_size );

    _Heap_Free_list_remove( next_block );

    next_block = _Heap_Block_at( block, block_size );
    next_block->size_and_flag |= HEAP_PREV_BLOCK_USED;

    /* Statistics */
    --stats->free_blocks;
    stats->free_size -= next_block_size;
  }

  block = _Heap_Block_allocate( heap, block, alloc_begin, new_alloc_size );

  block_size = _Heap_Block_size( block );
  next_block = _Heap_Block_at( block, block_size );
  *new_size = (uintptr_t) next_block - alloc_begin + HEAP_ALLOC_BONUS;

  /* Statistics */
  ++stats->resizes;

  return HEAP_RESIZE_SUCCESSFUL;
}

Heap_Resize_status _Heap_Resize_block(
  Heap_Control *heap,
  void *alloc_begin_ptr,
  uintptr_t new_alloc_size,
  uintptr_t *old_size,
  uintptr_t *new_size
)
{
  uintptr_t const page_size = heap->page_size;

  uintptr_t const alloc_begin = (uintptr_t) alloc_begin_ptr;

  Heap_Block *const block = _Heap_Block_of_alloc_area( alloc_begin, page_size );

  *old_size = 0;
  *new_size = 0;

  if ( _Heap_Is_block_in_heap( heap, block ) ) {
    _Heap_Protection_block_check( heap, block );

    /* TODO: Free only the next block if necessary */
    _Heap_Protection_free_all_delayed_blocks( heap );

    return _Heap_Resize_block_checked(
      heap,
      block,
      alloc_begin,
      new_alloc_size,
      old_size,
      new_size
    );
  }
  return HEAP_RESIZE_FATAL_ERROR;
}
