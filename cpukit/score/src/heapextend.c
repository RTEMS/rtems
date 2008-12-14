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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/heap.h>

/*PAGE
 *
 *  _Heap_Extend
 *
 *  This routine grows the_heap memory area using the size bytes which
 *  begin at starting_address.
 *
 *  Input parameters:
 *    the_heap          - pointer to heap header.
 *    starting_address  - pointer to the memory area.
 *    size              - size in bytes of the memory block to allocate.
 *
 *  Output parameters:
 *    *amount_extended  - amount of memory added to the_heap
 */

Heap_Extend_status _Heap_Extend(
  Heap_Control        *the_heap,
  void                *starting_address,
  intptr_t             size,
  intptr_t            *amount_extended
)
{
  uint32_t         the_size;
  Heap_Statistics *const stats = &the_heap->stats;

  /*
   *  The overhead was taken from the original heap memory.
   */

  Heap_Block  *old_final;
  Heap_Block  *new_final;

  /*
   *  There are five possibilities for the location of starting
   *  address:
   *
   *    1. non-contiguous lower address     (NOT SUPPORTED)
   *    2. contiguous lower address         (NOT SUPPORTED)
   *    3. in the heap                      (ERROR)
   *    4. contiguous higher address        (SUPPORTED)
   *    5. non-contiguous higher address    (NOT SUPPORTED)
   *
   *  As noted, this code only supports (4).
   */

  if ( starting_address >= the_heap->begin &&        /* case 3 */
       starting_address < the_heap->end
     )
    return HEAP_EXTEND_ERROR;

  if ( starting_address != the_heap->end )
    return HEAP_EXTEND_NOT_IMPLEMENTED;         /* cases 1, 2, and 5 */

  /*
   *  Currently only case 4 should make it to this point.
   *  The basic trick is to make the extend area look like a used
   *  block and free it.
   */

  old_final = the_heap->final;
  the_heap->end = _Addresses_Add_offset( the_heap->end, size );
  the_size = _Addresses_Subtract( the_heap->end, old_final ) - HEAP_OVERHEAD;
  _Heap_Align_down( &the_size, the_heap->page_size );

  *amount_extended = size;

  if( the_size < the_heap->min_block_size )
    return HEAP_EXTEND_SUCCESSFUL;

  old_final->size = the_size | (old_final->size & HEAP_PREV_USED);
  new_final = _Heap_Block_at( old_final, the_size );
  new_final->size = HEAP_PREV_USED;
  the_heap->final = new_final;

  stats->size += size;
  stats->used_blocks += 1;
  stats->frees -= 1;    /* Don't count subsequent call as actual free() */

  _Heap_Free( the_heap, _Heap_User_area( old_final ) );

  return HEAP_EXTEND_SUCCESSFUL;
}
