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
  unsigned32           size,
  unsigned32          *amount_extended
)
{
  Heap_Block        *the_block;
  unsigned32        *p;
  
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

  if ( starting_address >= (void *) the_heap->start &&        /* case 3 */
       starting_address <= (void *) the_heap->final
     )
    return HEAP_EXTEND_ERROR;

  if ( starting_address < (void *) the_heap->start ) {  /* cases 1 and 2 */

      return HEAP_EXTEND_NOT_IMPLEMENTED;               /* cases 1 and 2 */

  } else {                                              /* cases 4 and 5 */

    the_block = (Heap_Block *)
       _Addresses_Subtract_offset( starting_address, HEAP_OVERHEAD );
    if ( the_block != the_heap->final )
      return HEAP_EXTEND_NOT_IMPLEMENTED;                   /* case 5 */
  }

  /*
   *  Currently only case 4 should make it to this point.
   *  The basic trick is to make the extend area look like a used
   *  block and free it.
   */

  *amount_extended = size;

  old_final = the_heap->final;
  new_final = _Addresses_Add_offset( old_final, size );
  /* SAME AS: _Addresses_Add_offset( starting_address, size-HEAP_OVERHEAD ); */

  the_heap->final = new_final;

  old_final->front_flag = 
  new_final->back_flag  = _Heap_Build_flag( size, HEAP_BLOCK_USED );
  new_final->front_flag = HEAP_DUMMY_FLAG;

  /*
   *  Must pass in address of "user" area
   *  So add in the offset field.
   */

  p = (unsigned32 *) &old_final->next;
  *p = sizeof(unsigned32);
  p++;
  _Heap_Free( the_heap, p );
  
  return HEAP_EXTEND_SUCCESSFUL;
}

