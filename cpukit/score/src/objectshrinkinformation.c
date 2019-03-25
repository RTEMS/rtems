/**
 * @file
 *
 * @brief Shrink an Object Class Information Record
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/objectimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/wkspace.h>

void _Objects_Shrink_information(
  Objects_Information *information
)
{
  Objects_Maximum objects_per_block;
  Objects_Maximum block_count;
  Objects_Maximum block;
  Objects_Maximum index_base;

  _Assert( _Objects_Allocator_is_owner() );
  _Assert( _Objects_Is_auto_extend( information ) );

  /*
   * Search the list to find block or chunk with all objects inactive.
   */

  objects_per_block = information->objects_per_block;
  block_count = _Objects_Get_maximum_index( information ) / objects_per_block;
  index_base = objects_per_block;

  for ( block = 1; block < block_count; block++ ) {
    if ( information->inactive_per_block[ block ] == objects_per_block ) {
      Chain_Node       *node;
      const Chain_Node *tail;
      Objects_Maximum   index_end;

      node = _Chain_First( &information->Inactive );
      tail = _Chain_Immutable_tail( &information->Inactive );
      index_end = index_base + objects_per_block;

      while ( node != tail ) {
        Objects_Control *object;
        uint32_t         index;

        object = (Objects_Control *) node;
        index = _Objects_Get_index( object->id ) - OBJECTS_INDEX_MINIMUM;

        /*
         *  Get the next node before the node is extracted
         */
        node = _Chain_Next( node );

        if ( index >= index_base && index < index_end ) {
          _Chain_Extract_unprotected( &object->Node );
        }
      }

      /*
       *  Free the memory and reset the structures in the object' information
       */

      _Workspace_Free( information->object_blocks[ block ] );
      information->object_blocks[ block ] = NULL;
      information->inactive_per_block[ block ] = 0;

      information->inactive -= objects_per_block;

      return;
    }

    index_base += objects_per_block;
  }
}
