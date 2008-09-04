/*
 *  Heap Handler
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/heap.h>

/*PAGE
 *
 *  _Heap_Free
 *
 *  This kernel routine returns the memory designated by the
 *  given heap and given starting address to the memory pool.
 *
 *  Input parameters:
 *    the_heap         - pointer to heap header
 *    starting_address - starting address of the memory block to free.
 *
 *  Output parameters:
 *    TRUE  - if starting_address is valid heap address
 *    FALSE - if starting_address is invalid heap address
 */

bool _Heap_Free(
  Heap_Control        *the_heap,
  void                *starting_address
)
{
  Heap_Block      *the_block;
  Heap_Block      *next_block;
  uint32_t         the_size;
  uint32_t         next_size;
  Heap_Statistics *const stats = &the_heap->stats;
  bool             next_is_free;

  if ( !_Addresses_Is_in_range(
       starting_address, (void *)the_heap->start, (void *)the_heap->final ) ) {
    _HAssert(starting_address != NULL);
    return( FALSE );
  }

  _Heap_Start_of_block( the_heap, starting_address, &the_block );

  if ( !_Heap_Is_block_in( the_heap, the_block ) ) {
    _HAssert( FALSE );
    return( FALSE );
  }

  the_size = _Heap_Block_size( the_block );
  next_block = _Heap_Block_at( the_block, the_size );

  if ( !_Heap_Is_block_in( the_heap, next_block ) ) {
    _HAssert( FALSE );
    return( FALSE );
  }

  if ( !_Heap_Is_prev_used( next_block ) ) {
    _HAssert( FALSE );
    return( FALSE );
  }

  next_size = _Heap_Block_size( next_block );
  next_is_free = next_block < the_heap->final &&
    !_Heap_Is_prev_used(_Heap_Block_at(next_block, next_size));

  if ( !_Heap_Is_prev_used( the_block ) ) {
    uint32_t const prev_size = the_block->prev_size;
    Heap_Block *const prev_block = _Heap_Block_at( the_block, -prev_size );

    if ( !_Heap_Is_block_in( the_heap, prev_block ) ) {
      _HAssert( FALSE );
      return( FALSE );
    }

    /* As we always coalesce free blocks, the block that preceedes prev_block
       must have been used. */
    if ( !_Heap_Is_prev_used ( prev_block) ) {
      _HAssert( FALSE );
      return( FALSE );
    }

    if ( next_is_free ) {       /* coalesce both */
      uint32_t const size = the_size + prev_size + next_size;
      _Heap_Block_remove( next_block );
      stats->free_blocks -= 1;
      prev_block->size = size | HEAP_PREV_USED;
      next_block = _Heap_Block_at( prev_block, size );
      _HAssert(!_Heap_Is_prev_used( next_block));
      next_block->prev_size = size;
    }
    else {                      /* coalesce prev */
      uint32_t const size = the_size + prev_size;
      prev_block->size = size | HEAP_PREV_USED;
      next_block->size &= ~HEAP_PREV_USED;
      next_block->prev_size = size;
    }
  }
  else if ( next_is_free ) {    /* coalesce next */
    uint32_t const size = the_size + next_size;
    _Heap_Block_replace( next_block, the_block );
    the_block->size = size | HEAP_PREV_USED;
    next_block  = _Heap_Block_at( the_block, size );
    next_block->prev_size = size;
  }
  else {                        /* no coalesce */
    /* Add 'the_block' to the head of the free blocks list as it tends to
       produce less fragmentation than adding to the tail. */
    _Heap_Block_insert_after( _Heap_Head( the_heap), the_block );
    the_block->size = the_size | HEAP_PREV_USED;
    next_block->size &= ~HEAP_PREV_USED;
    next_block->prev_size = the_size;

    stats->free_blocks += 1;
    if ( stats->max_free_blocks < stats->free_blocks )
      stats->max_free_blocks = stats->free_blocks;
  }

  stats->used_blocks -= 1;
  stats->free_size += the_size;
  stats->frees += 1;

  return( TRUE );
}
