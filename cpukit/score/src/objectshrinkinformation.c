/*
 *  Object Handler
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/address.h>
#include <rtems/score/chain.h>
#include <rtems/score/object.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/objectmp.h>
#endif
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/isr.h>

/*
 *  _Objects_Shrink_information
 *
 *  This routine shrinks object information related data structures.
 *  The object's name and object space are released. The local_table
 *  etc block does not shrink. The InActive list needs to be scanned
 *  to find the objects are remove them.
 *  Input parameters:
 *    information     - object information table
 *    the_block       - the block to remove
 *
 *  Output parameters:  NONE
 */

void _Objects_Shrink_information(
  Objects_Information *information
)
{
  Objects_Control  *the_object;
  Objects_Control  *extract_me;
  uint32_t          block_count;
  uint32_t          block;
  uint32_t          index_base;
  uint32_t          index;

  /*
   * Search the list to find block or chunk with all objects inactive.
   */

  index_base = _Objects_Get_index( information->minimum_id );
  block_count = (information->maximum - index_base) /
                 information->allocation_size;

  for ( block = 0; block < block_count; block++ ) {
    if ( information->inactive_per_block[ block ] ==
         information->allocation_size ) {

      /*
       *  Assume the Inactive chain is never empty at this point
       */
      the_object = (Objects_Control *) _Chain_First( &information->Inactive );

      do {
         index = _Objects_Get_index( the_object->id );
         /*
          *  Get the next node before the node is extracted
          */
         extract_me = the_object;
         the_object = (Objects_Control *) the_object->Node.next;
         if ((index >= index_base) &&
             (index < (index_base + information->allocation_size))) {
           _Chain_Extract( &extract_me->Node );
         }
       }
       while ( the_object );
      /*
       *  Free the memory and reset the structures in the object' information
       */

      _Workspace_Free( information->object_blocks[ block ] );
      information->object_blocks[ block ] = NULL;
      information->inactive_per_block[ block ] = 0;

      information->inactive -= information->allocation_size;

      return;
    }

    index_base += information->allocation_size;
  }
}
