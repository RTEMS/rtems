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
 *  _Heap_Size_of_user_area
 *
 *  This kernel routine returns the size of the memory area
 *  given heap block.
 *
 *  Input parameters:
 *    the_heap         - pointer to heap header
 *    starting_address - starting address of the memory block to free.
 *    size             - pointer to size of area
 *
 *  Output parameters:
 *    size  - size of area filled in
 *    TRUE  - if starting_address is valid heap address
 *    FALSE - if starting_address is invalid heap address
 */

boolean _Heap_Size_of_user_area(
  Heap_Control        *the_heap,
  void                *starting_address,
  unsigned32          *size
)
{
  Heap_Block        *the_block;
  Heap_Block        *next_block;
  unsigned32         the_size;

  the_block = _Heap_User_block_at( starting_address );
  
  if ( !_Heap_Is_block_in( the_heap, the_block ) ||
        _Heap_Is_block_free( the_block ) )
    return( FALSE );

  the_size   = _Heap_Block_size( the_block );
  next_block = _Heap_Block_at( the_block, the_size );

  if ( !_Heap_Is_block_in( the_heap, next_block ) ||
       (the_block->front_flag != next_block->back_flag) )
    return( FALSE );

  *size = the_size;
  return( TRUE );
}

