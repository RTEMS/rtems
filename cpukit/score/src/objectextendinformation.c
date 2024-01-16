/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreObject
 *
 * @brief This source file contains the implementation of
 *   _Objects_Extend_information().
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

#include <rtems/score/objectimpl.h>
#include <rtems/score/address.h>
#include <rtems/score/assert.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/isrlevel.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/wkspace.h>

#include <string.h>  /* for memcpy() */

Objects_Maximum _Objects_Extend_information(
  Objects_Information *information
)
{
  Objects_Control  *the_object;
  uint32_t          block_count;
  uint32_t          block;
  uint32_t          index_base;
  uint32_t          index_end;
  uint32_t          index;
  uint32_t          extend_count;
  Objects_Maximum   old_maximum;
  uint32_t          new_maximum;
  size_t            object_block_size;
  Objects_Control  *new_object_block;
  bool              do_extend;
  Objects_Id        api_class_and_node;

  _Assert(
    _Objects_Allocator_is_owner()
      || !_System_state_Is_up( _System_state_Get() )
  );
  _Assert( _Objects_Is_auto_extend( information ) );

  extend_count = _Objects_Extend_size( information );
  old_maximum = _Objects_Get_maximum_index( information );
  new_maximum = (uint32_t) old_maximum + extend_count;
  api_class_and_node = information->maximum_id & ~OBJECTS_INDEX_MASK;

  /*
   *  Search for a free block of indexes. If we do NOT need to allocate or
   *  extend the block table, then we will change do_extend.
   */
  do_extend     = true;
  index_base    = extend_count;
  block         = 1;

  if ( information->object_blocks == NULL ) {
    block_count = 1;
  } else {
    block_count = old_maximum / extend_count;

    for ( ; block < block_count; block++ ) {
      if ( information->object_blocks[ block ] == NULL ) {
        do_extend = false;
        break;
      } else
        index_base += extend_count;
    }
  }

  index_end = index_base + extend_count;

  /*
   *  We need to limit the number of objects to the maximum number
   *  representable in the index portion of the object Id.  In the
   *  case of 16-bit Ids, this is only 256 object instances.
   */
  if ( new_maximum > OBJECTS_ID_FINAL_INDEX ) {
    return 0;
  }

  /*
   * Allocate the name table, and the objects and if it fails either return or
   * generate a fatal error depending on auto-extending being active.
   */
  object_block_size = extend_count * information->object_size;
  new_object_block = _Workspace_Allocate( object_block_size );
  if ( new_object_block == NULL ) {
    return 0;
  }

  /*
   *  Do we need to grow the tables?
   */
  if ( do_extend ) {
    ISR_lock_Context  lock_context;
    Objects_Control **object_blocks;
    Objects_Control **local_table;
    Objects_Maximum  *inactive_per_block;
    void             *old_tables;
    size_t            table_size;
    uintptr_t         object_blocks_size;
    uintptr_t         local_table_size;

    /*
     *  Growing the tables means allocating a new area, doing a copy and
     *  updating the information table.
     *
     *  If the maximum is minimum we do not have a table to copy. First
     *  time through.
     *
     *  The allocation has:
     *
     *      Objects_Control *object_blocks[ block_count ];
     *      Objects_Control *local_table[ maximum ];
     *      Objects_Maximum  inactive_count[ block_count ];
     *
     *  This is the order in memory. Watch changing the order. See the memcpy
     *  below.
     */

    /*
     *  Up the block count and maximum.
     */
    block_count++;

    /*
     *  Allocate the tables and break it up.
     */
    object_blocks_size = block_count * sizeof( *object_blocks );
    local_table_size =  new_maximum * sizeof( *local_table );
    table_size = object_blocks_size
      + local_table_size
      + block_count * sizeof( *inactive_per_block );
    object_blocks = _Workspace_Allocate( table_size );
    if ( object_blocks == NULL ) {
      _Workspace_Free( new_object_block );
      return 0;
    }

    /*
     *  Break the block into the various sections.
     */
    local_table = _Addresses_Add_offset(
      object_blocks,
      object_blocks_size
    );
    inactive_per_block = _Addresses_Add_offset(
      local_table,
      local_table_size
    );

    /*
     *  Take the block count down. Saves all the (block_count - 1)
     *  in the copies.
     */
    block_count--;

    if ( old_maximum > extend_count ) {
      /*
       * Coverity thinks there is a way for this to be NULL (CID #26033).
       * After much time spent analyzing this, no one has identified the
       * conditions where this can actually occur. Adding this _Assert ensures
       * that it is never NULL. If this assert is triggered, condition
       * generating this case will have been identified and it can be revisted.
       * This is being done out of an abundance of caution since we could have
       * easily flagged this as a false positive and ignored it completely.
       */
      _Assert(information->object_blocks != NULL);

      /*
       *  Copy each section of the table over. This has to be performed as
       *  separate parts as size of each block has changed.
       */
      memcpy(
        object_blocks,
        information->object_blocks,
        block_count * sizeof( *object_blocks )
      );
      memcpy(
        inactive_per_block,
        information->inactive_per_block,
        block_count * sizeof( *inactive_per_block )
      );
    } else {
      object_blocks[ 0 ] = NULL;
      inactive_per_block[ 0 ] = 0;
    }

    memcpy(
      local_table,
      information->local_table,
      old_maximum * sizeof( *local_table )
    );

    /*
     *  Initialise the new entries in the table.
     */
    for ( index = index_base ; index < index_end ; ++index ) {
      local_table[ index ] = NULL;
    }

    /* FIXME: https://devel.rtems.org/ticket/2280 */
    _ISR_lock_ISR_disable( &lock_context );

    old_tables = information->object_blocks;

    information->object_blocks = object_blocks;
    information->inactive_per_block = inactive_per_block;
    information->local_table = local_table;
    information->maximum_id = api_class_and_node
      | (new_maximum << OBJECTS_INDEX_START_BIT);

    _ISR_lock_ISR_enable( &lock_context );

    _Workspace_Free( old_tables );

    block_count++;
  }

  /*
   *  Assign the new object block to the object block table.
   */
  information->object_blocks[ block ] = new_object_block;
  information->inactive_per_block[ block ] = information->objects_per_block;
  information->inactive += information->objects_per_block;

  /*
   *  Append to inactive chain.
   */
  the_object = new_object_block;
  for ( index = index_base ; index < index_end ; ++index ) {
    the_object->id = api_class_and_node
      | ( ( index + OBJECTS_INDEX_MINIMUM ) << OBJECTS_INDEX_START_BIT );

    _Chain_Initialize_node( &the_object->Node );
    _Chain_Append_unprotected( &information->Inactive, &the_object->Node );

    the_object = _Addresses_Add_offset( the_object, information->object_size );
  }

  return block;
}
