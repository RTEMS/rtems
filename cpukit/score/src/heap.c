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
 *  _Heap_Initialize
 *
 *  This kernel routine initializes a heap.
 *
 *  Input parameters:
 *    the_heap         - pointer to heap header
 *    starting_address - starting address of heap
 *    size             - size of heap
 *    page_size        - allocatable unit of memory
 *
 *  Output parameters:
 *    returns - maximum memory available if RTEMS_SUCCESSFUL
 *    0       - otherwise
 *
 *    This is what a heap looks like in memory immediately
 *    after initialization:
 *
 *            +--------------------------------+
 *     0      |  size = 0      | status = used |  a.k.a.  dummy back flag
 *            +--------------------------------+
 *     4      |  size = size-8 | status = free |  a.k.a.  front flag
 *            +--------------------------------+
 *     8      |  next     = PERM HEAP_TAIL     |
 *            +--------------------------------+
 *    12      |  previous = PERM HEAP_HEAD     |
 *            +--------------------------------+
 *            |                                |
 *            |      memory available          |
 *            |       for allocation           |
 *            |                                |
 *            +--------------------------------+
 *  size - 8  |  size = size-8 | status = free |  a.k.a.  back flag
 *            +--------------------------------+
 *  size - 4  |  size = 0      | status = used |  a.k.a.  dummy front flag
 *            +--------------------------------+
 */

unsigned32 _Heap_Initialize(
  Heap_Control        *the_heap,
  void                *starting_address,
  unsigned32           size,
  unsigned32           page_size
)
{
  Heap_Block        *the_block;
  unsigned32         the_size;

  if ( !_Heap_Is_page_size_valid( page_size ) ||
       (size < HEAP_MINIMUM_SIZE) )
    return 0;

  the_heap->page_size = page_size;
  the_size = size - HEAP_OVERHEAD;

  the_block             = (Heap_Block *) starting_address;
  the_block->back_flag  = HEAP_DUMMY_FLAG;
  the_block->front_flag = the_size;
  the_block->next       = _Heap_Tail( the_heap );
  the_block->previous   = _Heap_Head( the_heap );

  the_heap->start          = the_block;
  the_heap->first          = the_block;
  the_heap->permanent_null = NULL;
  the_heap->last           = the_block;

  the_block             = _Heap_Next_block( the_block );
  the_block->back_flag  = the_size;
  the_block->front_flag = HEAP_DUMMY_FLAG;
  the_heap->final       = the_block;

  return ( the_size - HEAP_BLOCK_USED_OVERHEAD );
}

