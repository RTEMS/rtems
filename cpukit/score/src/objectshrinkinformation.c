/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreObject
 *
 * @brief This source file contains the implementation of
 *   _Objects_Free_objects_block() and _Objects_Shrink_information().
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/assert.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/wkspace.h>

void _Objects_Free_objects_block(
  Objects_Information *information,
  Objects_Maximum      block
)
{
  Objects_Maximum   objects_per_block;
  Objects_Maximum   index_base;
  Objects_Maximum   index_end;
  Chain_Node       *node;
  const Chain_Node *tail;

  objects_per_block = information->objects_per_block;

  _Assert( _Objects_Allocator_is_owner() );
  _Assert( _Objects_Is_auto_extend( information ) );
  _Assert( block >= 1 );
  _Assert(
    block < _Objects_Get_maximum_index( information ) / objects_per_block
  );

  index_base = block * objects_per_block;
  index_end = index_base + objects_per_block;
  node = _Chain_First( &information->Inactive );
  tail = _Chain_Immutable_tail( &information->Inactive );

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
}

void _Objects_Shrink_information( Objects_Information *information )
{
  Objects_Maximum objects_per_block;
  Objects_Maximum block_count;
  Objects_Maximum block;

  _Assert( _Objects_Allocator_is_owner() );
  _Assert( _Objects_Is_auto_extend( information ) );

  /*
   * Search the list to find block or chunk with all objects inactive.
   */

  objects_per_block = information->objects_per_block;
  block_count = _Objects_Get_maximum_index( information ) / objects_per_block;

  for ( block = 1; block < block_count; block++ ) {
    if ( information->inactive_per_block[ block ] == objects_per_block ) {
      _Objects_Free_objects_block( information, block );
      return;
    }
  }
}
