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
 *
 *  $Id$
 */

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

/*PAGE
 *
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
  unsigned32        block_count;
  unsigned32        block;
  unsigned32        index_base;
  unsigned32        index;

  /*
   * Search the list to find block or chunnk with all objects inactive.
   */

  index_base = _Objects_Get_index( information->minimum_id );
  block_count = ( information->maximum - index_base ) / information->allocation_size;
  
  for ( block = 0; block < block_count; block++ ) {
    if ( information->inactive_per_block[ block ] == information->allocation_size ) {

      /*
       * XXX - Not to sure how to use a chain where you need to iterate and
       *       and remove elements.
       */
      
      the_object = (Objects_Control *) information->Inactive.first;

      /*
       *  Assume the Inactive chain is never empty at this point
       */

      do {
        index = _Objects_Get_index( the_object->id );

        if ((index >= index_base) &&
            (index < (index_base + information->allocation_size))) {
          
          /*
           *  Get the next node before the node is extracted
           */
          
          extract_me = the_object;

          if ( !_Chain_Is_last( &the_object->Node ) )
            the_object = (Objects_Control *) the_object->Node.next;
          else
            the_object = NULL;
          
          _Chain_Extract( &extract_me->Node );
        }
        else {
          the_object = (Objects_Control *) the_object->Node.next;
        }
      }
      while ( the_object && !_Chain_Is_last( &the_object->Node ) );

      /*
       *  Free the memory and reset the structures in the object' information
       */

      _Workspace_Free( information->object_blocks[ block ] );
      information->name_table[ block ] = NULL;
      information->object_blocks[ block ] = NULL;
      information->inactive_per_block[ block ] = 0;

      information->inactive -= information->allocation_size;
      
      return;
    }
    
    index_base += information->allocation_size;
  }
}
