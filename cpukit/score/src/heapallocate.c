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
 *  _Heap_Allocate
 *
 *  This kernel routine allocates the requested size of memory
 *  from the specified heap.
 *
 *  Input parameters:
 *    the_heap  - pointer to heap header.
 *    size      - size in bytes of the memory block to allocate.
 *
 *  Output parameters:
 *    returns - starting address of memory block allocated
 */

void *_Heap_Allocate(
  Heap_Control        *the_heap,
  unsigned32           size
)
{
  unsigned32  excess;
  unsigned32  the_size;
  Heap_Block *the_block;
  Heap_Block *next_block;
  Heap_Block *temporary_block;
  void       *ptr;
  unsigned32  offset;

  /*
   * Catch the case of a user allocating close to the limit of the
   * unsigned32.
   */

  if ( size >= (-1 - HEAP_BLOCK_USED_OVERHEAD) )
    return( NULL );

  excess   = size % the_heap->page_size;
  the_size = size + the_heap->page_size + HEAP_BLOCK_USED_OVERHEAD;
  
  if ( excess )
    the_size += the_heap->page_size - excess;

  if ( the_size < sizeof( Heap_Block ) )
    the_size = sizeof( Heap_Block );

  for ( the_block = the_heap->first;
        ;
        the_block = the_block->next ) {
    if ( the_block == _Heap_Tail( the_heap ) )
      return( NULL );
    if ( the_block->front_flag >= the_size )
      break;
  }

  if ( (the_block->front_flag - the_size) >
       (the_heap->page_size + HEAP_BLOCK_USED_OVERHEAD) ) {
    the_block->front_flag -= the_size;
    next_block             = _Heap_Next_block( the_block );
    next_block->back_flag  = the_block->front_flag;

    temporary_block            = _Heap_Block_at( next_block, the_size );
    temporary_block->back_flag =
    next_block->front_flag     = _Heap_Build_flag( the_size,
                                    HEAP_BLOCK_USED );
    ptr = _Heap_Start_of_user_area( next_block );
  } else {
    next_block                = _Heap_Next_block( the_block );
    next_block->back_flag     = _Heap_Build_flag( the_block->front_flag,
                                   HEAP_BLOCK_USED );
    the_block->front_flag     = next_block->back_flag;
    the_block->next->previous = the_block->previous;
    the_block->previous->next = the_block->next;
    ptr = _Heap_Start_of_user_area( the_block );
  }
  
  /*
   * round ptr up to a multiple of page size
   * Have to save the bump amount in the buffer so that free can figure it out
   */
  
  offset = the_heap->page_size - (((unsigned32) ptr) & (the_heap->page_size - 1));
  ptr = _Addresses_Add_offset( ptr, offset );
  *(((unsigned32 *) ptr) - 1) = offset;

#ifdef RTEMS_DEBUG
  {
      unsigned32 ptr_u32;
      ptr_u32 = (unsigned32) ptr;
      if (ptr_u32 & (the_heap->page_size - 1))
          abort();
  }
#endif

  return ptr;
}

