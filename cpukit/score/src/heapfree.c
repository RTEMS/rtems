/*
 *  Heap Handler
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */


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

boolean _Heap_Free(
  Heap_Control        *the_heap,
  void                *starting_address
)
{
  Heap_Block        *the_block;
  Heap_Block        *next_block;
  Heap_Block        *new_next_block;
  Heap_Block        *previous_block;
  Heap_Block        *temporary_block;
  unsigned32         the_size;

  the_block = _Heap_User_block_at( starting_address );

  if ( !_Heap_Is_block_in( the_heap, the_block ) ||
        _Heap_Is_block_free( the_block ) ) {
      return( FALSE );
  }

  the_size   = _Heap_Block_size( the_block );
  next_block = _Heap_Block_at( the_block, the_size );

  if ( !_Heap_Is_block_in( the_heap, next_block ) ||
       (the_block->front_flag != next_block->back_flag) ) {
      return( FALSE );
  }

  if ( _Heap_Is_previous_block_free( the_block ) ) {
    previous_block = _Heap_Previous_block( the_block );

    if ( !_Heap_Is_block_in( the_heap, previous_block ) ) {
        return( FALSE );
    }

    if ( _Heap_Is_block_free( next_block ) ) {      /* coalesce both */
      previous_block->front_flag += next_block->front_flag + the_size;
      temporary_block             = _Heap_Next_block( previous_block );
      temporary_block->back_flag  = previous_block->front_flag;
      next_block->next->previous  = next_block->previous;
      next_block->previous->next  = next_block->next;
    }
    else {                     /* coalesce prev */
      previous_block->front_flag =
      next_block->back_flag      = previous_block->front_flag + the_size;
    }
  }
  else if ( _Heap_Is_block_free( next_block ) ) { /* coalesce next */
    the_block->front_flag     = the_size + next_block->front_flag;
    new_next_block            = _Heap_Next_block( the_block );
    new_next_block->back_flag = the_block->front_flag;
    the_block->next           = next_block->next;
    the_block->previous       = next_block->previous;
    next_block->previous->next = the_block;
    next_block->next->previous = the_block;

    if (the_heap->first == next_block)
        the_heap->first = the_block;
  }
  else {                          /* no coalesce */
    next_block->back_flag     =
    the_block->front_flag     = the_size;
    the_block->previous       = _Heap_Head( the_heap );
    the_block->next           = the_heap->first;
    the_heap->first           = the_block;
    the_block->next->previous = the_block;
  }

  return( TRUE );
}

