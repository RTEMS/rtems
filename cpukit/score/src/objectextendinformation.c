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

#include <string.h>  /* for memcpy() */

/*
 *  _Objects_Extend_information
 *
 *  This routine extends all object information related data structures.
 *
 *  Input parameters:
 *    information     - object information table
 *
 *  Output parameters:  NONE
 */

void _Objects_Extend_information(
  Objects_Information *information
)
{
  Objects_Control  *the_object;
  Chain_Control     Inactive;
  uint32_t          block_count;
  uint32_t          block;
  uint32_t          index_base;
  uint32_t          minimum_index;
  uint32_t          index;
  uint32_t          maximum;
  size_t            block_size;
  void             *new_object_block;
  bool              do_extend;

  /*
   *  Search for a free block of indexes. If we do NOT need to allocate or
   *  extend the block table, then we will change do_extend.
   */
  do_extend     = true;
  minimum_index = _Objects_Get_index( information->minimum_id );
  index_base    = minimum_index;
  block         = 0;

  /* if ( information->maximum < minimum_index ) */
  if ( information->object_blocks == NULL )
    block_count = 0;
  else {
    block_count = information->maximum / information->allocation_size;

    for ( ; block < block_count; block++ ) {
      if ( information->object_blocks[ block ] == NULL ) {
        do_extend = false;
        break;
      } else
        index_base += information->allocation_size;
    }
  }

  maximum = (uint32_t) information->maximum + information->allocation_size;

  /*
   *  We need to limit the number of objects to the maximum number
   *  representable in the index portion of the object Id.  In the
   *  case of 16-bit Ids, this is only 256 object instances.
   */
  if ( maximum > OBJECTS_ID_FINAL_INDEX ) {
    return;
  }

  /*
   * Allocate the name table, and the objects and if it fails either return or
   * generate a fatal error depending on auto-extending being active.
   */
  block_size = information->allocation_size * information->size;
  if ( information->auto_extend ) {
    new_object_block = _Workspace_Allocate( block_size );
    if ( !new_object_block )
      return;
  } else {
    new_object_block = _Workspace_Allocate_or_fatal_error( block_size );
  }

  /*
   *  Do we need to grow the tables?
   */
  if ( do_extend ) {
    ISR_Level         level;
    void            **object_blocks;
    uint32_t         *inactive_per_block;
    Objects_Control **local_table;
    void             *old_tables;
    size_t            block_size;

    /*
     *  Growing the tables means allocating a new area, doing a copy and
     *  updating the information table.
     *
     *  If the maximum is minimum we do not have a table to copy. First
     *  time through.
     *
     *  The allocation has :
     *
     *      void            *objects[block_count];
     *      uint32_t         inactive_count[block_count];
     *      Objects_Control *local_table[maximum];
     *
     *  This is the order in memory. Watch changing the order. See the memcpy
     *  below.
     */

    /*
     *  Up the block count and maximum
     */
    block_count++;

    /*
     *  Allocate the tables and break it up.
     */
    block_size = block_count *
           (sizeof(void *) + sizeof(uint32_t) + sizeof(Objects_Name *)) +
          ((maximum + minimum_index) * sizeof(Objects_Control *));
    object_blocks = (void**) _Workspace_Allocate( block_size );

    if ( !object_blocks ) {
      _Workspace_Free( new_object_block );
      return;
    }

    /*
     *  Break the block into the various sections.
     */
    inactive_per_block = (uint32_t *) _Addresses_Add_offset(
        object_blocks, block_count * sizeof(void*) );
    local_table = (Objects_Control **) _Addresses_Add_offset(
        inactive_per_block, block_count * sizeof(uint32_t) );

    /*
     *  Take the block count down. Saves all the (block_count - 1)
     *  in the copies.
     */
    block_count--;

    if ( information->maximum > minimum_index ) {

      /*
       *  Copy each section of the table over. This has to be performed as
       *  separate parts as size of each block has changed.
       */

      memcpy( object_blocks,
              information->object_blocks,
              block_count * sizeof(void*) );
      memcpy( inactive_per_block,
              information->inactive_per_block,
              block_count * sizeof(uint32_t) );
      memcpy( local_table,
              information->local_table,
              (information->maximum + minimum_index) * sizeof(Objects_Control *) );
    } else {

      /*
       *  Deal with the special case of the 0 to minimum_index
       */
      for ( index = 0; index < minimum_index; index++ ) {
        local_table[ index ] = NULL;
      }
    }

    /*
     *  Initialise the new entries in the table.
     */
    object_blocks[block_count] = NULL;
    inactive_per_block[block_count] = 0;

    for ( index=index_base ;
          index < ( information->allocation_size + index_base );
          index++ ) {
      local_table[ index ] = NULL;
    }

    _ISR_Disable( level );

    old_tables = information->object_blocks;

    information->object_blocks = object_blocks;
    information->inactive_per_block = inactive_per_block;
    information->local_table = local_table;
    information->maximum = (Objects_Maximum) maximum;
    information->maximum_id = _Objects_Build_id(
        information->the_api,
        information->the_class,
        _Objects_Local_node,
        information->maximum
      );

    _ISR_Enable( level );

    _Workspace_Free( old_tables );

    block_count++;
  }

  /*
   *  Assign the new object block to the object block table.
   */
  information->object_blocks[ block ] = new_object_block;

  /*
   *  Initialize objects .. add to a local chain first.
   */
  _Chain_Initialize(
    &Inactive,
    information->object_blocks[ block ],
    information->allocation_size,
    information->size
  );

  /*
   *  Move from the local chain, initialise, then append to the inactive chain
   */
  index = index_base;

  while ((the_object = (Objects_Control *) _Chain_Get( &Inactive )) != NULL ) {

    the_object->id = _Objects_Build_id(
        information->the_api,
        information->the_class,
        _Objects_Local_node,
        index
      );

    _Chain_Append( &information->Inactive, &the_object->Node );

    index++;
  }

  information->inactive_per_block[ block ] = information->allocation_size;
  information->inactive =
    (Objects_Maximum)(information->inactive + information->allocation_size);
}
