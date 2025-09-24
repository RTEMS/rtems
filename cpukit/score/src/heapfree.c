/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreHeap
 *
 * @brief This source file contains the implementation of
 *   _Heap_Free().
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/score/threaddispatch.h>

#ifndef HEAP_PROTECTION
  #define _Heap_Protection_determine_block_free( heap, block ) true
#else
static void _Heap_Protection_delay_block_free(
  Heap_Control *heap,
  Heap_Block   *block
)
{
  uintptr_t *const pattern_begin = (uintptr_t *) _Heap_Alloc_area_of_block(
    block
  );
  uintptr_t *const pattern_end = (uintptr_t *) ( (uintptr_t) block +
                                                 _Heap_Block_size( block ) +
                                                 HEAP_ALLOC_BONUS );
  uintptr_t const
    delayed_free_block_count = heap->Protection.delayed_free_block_count;
  uintptr_t *current = NULL;

  block->Protection_begin.next_delayed_free_block = block;
  block->Protection_begin.task = _Thread_Get_executing();

  if ( delayed_free_block_count > 0 ) {
    Heap_Block *const last = heap->Protection.last_delayed_free_block;

    last->Protection_begin.next_delayed_free_block = block;
  } else {
    heap->Protection.first_delayed_free_block = block;
  }
  heap->Protection.last_delayed_free_block = block;
  heap->Protection.delayed_free_block_count = delayed_free_block_count + 1;

  for ( current = pattern_begin; current != pattern_end; ++current ) {
    *current = HEAP_FREE_PATTERN;
  }
}

static void _Heap_Protection_check_free_block(
  Heap_Control *heap,
  Heap_Block   *block
)
{
  uintptr_t *const pattern_begin = (uintptr_t *) _Heap_Alloc_area_of_block(
    block
  );
  uintptr_t *const pattern_end = (uintptr_t *) ( (uintptr_t) block +
                                                 _Heap_Block_size( block ) +
                                                 HEAP_ALLOC_BONUS );
  uintptr_t       *current = NULL;

  for ( current = pattern_begin; current != pattern_end; ++current ) {
    if ( *current != HEAP_FREE_PATTERN ) {
      _Heap_Protection_block_error( heap, block, HEAP_ERROR_FREE_PATTERN );
      break;
    }
  }
}

static bool _Heap_Protection_determine_block_free(
  Heap_Control *heap,
  Heap_Block   *block
)
{
  bool              do_free = true;
  Heap_Block *const next = block->Protection_begin.next_delayed_free_block;

  if ( next == NULL ) {
    _Heap_Protection_delay_block_free( heap, block );
    do_free = false;
  } else if ( next == HEAP_PROTECTION_OBOLUS ) {
    _Heap_Protection_check_free_block( heap, block );
  } else {
    _Heap_Protection_block_error( heap, block, HEAP_ERROR_DOUBLE_FREE );
  }

  return do_free;
}
#endif

bool _Heap_Free( Heap_Control *heap, void *alloc_begin_ptr )
{
  Heap_Statistics *const stats = &heap->stats;
  uintptr_t              alloc_begin;
  Heap_Block            *block;
  Heap_Block            *next_block = NULL;
  uintptr_t              block_size = 0;
  uintptr_t              next_block_size = 0;
  bool                   next_is_free = false;

  /*
   * If NULL return true so a free on NULL is considered a valid release. This
   * is a special case that could be handled by the in heap check how-ever that
   * would result in false being returned which is wrong.
   */
  if ( alloc_begin_ptr == NULL ) {
    return true;
  }

  alloc_begin = (uintptr_t) alloc_begin_ptr;
  block = _Heap_Block_of_alloc_area( alloc_begin, heap->page_size );

  if ( !_Heap_Is_block_in_heap( heap, block ) ) {
    return false;
  }

  _Heap_Protection_block_check( heap, block );

  block_size = _Heap_Block_size( block );
  next_block = _Heap_Block_at( block, block_size );

  if ( !_Heap_Is_block_in_heap( heap, next_block ) ) {
    return false;
  }

  _Heap_Protection_block_check( heap, next_block );

  if ( !_Heap_Is_prev_used( next_block ) ) {
    _Heap_Protection_block_error( heap, block, HEAP_ERROR_BAD_USED_BLOCK );
    return false;
  }

  if ( !_Heap_Protection_determine_block_free( heap, block ) ) {
    return true;
  }

  next_block_size = _Heap_Block_size( next_block );
  next_is_free = next_block != heap->last_block &&
                 !_Heap_Is_prev_used(
                   _Heap_Block_at( next_block, next_block_size )
                 );

  if ( !_Heap_Is_prev_used( block ) ) {
    uintptr_t const   prev_size = block->prev_size;
    Heap_Block *const prev_block = _Heap_Block_at( block, -prev_size );

    if ( !_Heap_Is_block_in_heap( heap, prev_block ) ) {
      _HAssert( false );
      return ( false );
    }

    /* As we always coalesce free blocks, the block that preceedes prev_block
       must have been used. */
    if ( !_Heap_Is_prev_used( prev_block ) ) {
      _HAssert( false );
      return ( false );
    }

    if ( next_is_free ) { /* coalesce both */
      uintptr_t const size = block_size + prev_size + next_block_size;
      _Heap_Free_list_remove( next_block );
      stats->free_blocks -= 1;
      prev_block->size_and_flag = size | HEAP_PREV_BLOCK_USED;
      next_block = _Heap_Block_at( prev_block, size );
      _HAssert( !_Heap_Is_prev_used( next_block ) );
      next_block->prev_size = size;
    } else { /* coalesce prev */
      uintptr_t const size = block_size + prev_size;
      prev_block->size_and_flag = size | HEAP_PREV_BLOCK_USED;
      next_block->size_and_flag &= ~HEAP_PREV_BLOCK_USED;
      next_block->prev_size = size;
    }
  } else if ( next_is_free ) { /* coalesce next */
    uintptr_t const size = block_size + next_block_size;
    _Heap_Free_list_replace( next_block, block );
    block->size_and_flag = size | HEAP_PREV_BLOCK_USED;
    next_block = _Heap_Block_at( block, size );
    next_block->prev_size = size;
  } else { /* no coalesce */
    /* Add 'block' to the head of the free blocks list as it tends to
       produce less fragmentation than adding to the tail. */
    _Heap_Free_list_insert_after( _Heap_Free_list_head( heap ), block );
    block->size_and_flag = block_size | HEAP_PREV_BLOCK_USED;
    next_block->size_and_flag &= ~HEAP_PREV_BLOCK_USED;
    next_block->prev_size = block_size;

    /* Statistics */
    ++stats->free_blocks;
    if ( stats->max_free_blocks < stats->free_blocks ) {
      stats->max_free_blocks = stats->free_blocks;
    }
  }

  /* Statistics */
  --stats->used_blocks;
  ++stats->frees;
  stats->free_size += block_size;
  stats->lifetime_freed += block_size;

  return ( true );
}
