/*
 *  Object Handler
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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

#include <string.h>  /* for memcpy() */

/*PAGE
 *
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
  void             *name_area;
  Chain_Control     Inactive;
  unsigned32        block_count;
  unsigned32        block;
  unsigned32        index_base;
  unsigned32        minimum_index;
  unsigned32        index;

  /*
   *  Search for a free block of indexes. The block variable ends up set
   *  to block_count + 1 if the table needs to be extended.
   */

  minimum_index = _Objects_Get_index( information->minimum_id );
  index_base    = minimum_index;
  block         = 0;
  
  if ( information->maximum < minimum_index )
    block_count = 0;
  else {
    block_count = information->maximum / information->allocation_size;
  
    for ( ; block < block_count; block++ ) {
      if ( information->object_blocks[ block ] == NULL )
        break;
      else
        index_base += information->allocation_size;
    }
  }

  /*
   *  If the index_base is the maximum we need to grow the tables.
   */

  if (index_base >= information->maximum ) {
    ISR_Level         level;
    void            **object_blocks;
    Objects_Name     *name_table;
    unsigned32       *inactive_per_block;
    Objects_Control **local_table;
    unsigned32        maximum;
    void             *old_tables;    
    
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
     *      unsigned32       inactive_count[block_count];
     *      Objects_Name    *name_table[block_count];
     *      Objects_Control *local_table[maximum];
     *
     *  This is the order in memory. Watch changing the order. See the memcpy
     *  below.
     */

    /*
     *  Up the block count and maximum
     */

    block_count++;
    
    maximum = information->maximum + information->allocation_size;

    /*
     *  Allocate the tables and break it up.
     */
    
    if ( information->auto_extend ) {
      object_blocks = (void**)
        _Workspace_Allocate(
          block_count *
             (sizeof(void *) + sizeof(unsigned32) + sizeof(Objects_Name *)) +
          ((maximum + minimum_index) * sizeof(Objects_Control *))
          );

      if ( !object_blocks )
        return;
    }
    else {
      object_blocks = (void**)
        _Workspace_Allocate_or_fatal_error(
          block_count * 
             (sizeof(void *) + sizeof(unsigned32) + sizeof(Objects_Name *)) +
          ((maximum + minimum_index) * sizeof(Objects_Control *))
        );
    }

    /*
     *  Break the block into the various sections.
     *
     */
     
    inactive_per_block = (unsigned32 *) _Addresses_Add_offset(
        object_blocks, block_count * sizeof(void*) );
    name_table = (Objects_Name *) _Addresses_Add_offset(
        inactive_per_block, block_count * sizeof(unsigned32) );
    local_table = (Objects_Control **) _Addresses_Add_offset(
        name_table, block_count * sizeof(Objects_Name *) );
    
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
              block_count * sizeof(unsigned32) );
      memcpy( name_table,
              information->name_table,
              block_count * sizeof(Objects_Name *) );
      memcpy( local_table,
              information->local_table,
              (information->maximum + minimum_index) * sizeof(Objects_Control *) );
    }
    else {

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
    name_table[block_count] = NULL;

    for ( index=index_base ;
          index < ( information->allocation_size + index_base );
          index++ ) {
      local_table[ index ] = NULL;
    }
    
    _ISR_Disable( level );

    old_tables = information->object_blocks;
    
    information->object_blocks = object_blocks;
    information->inactive_per_block = inactive_per_block;
    information->name_table = name_table;
    information->local_table = local_table;
    information->maximum = maximum;
    information->maximum_id =
      _Objects_Build_id(
        information->the_class, _Objects_Local_node, information->maximum
      );

    _ISR_Enable( level );

    if ( old_tables )
      _Workspace_Free( old_tables );
    
    block_count++;
  }
           
  /*
   *  Allocate the name table, and the objects
   */

  if ( information->auto_extend ) {
    information->object_blocks[ block ] = 
      _Workspace_Allocate(
        (information->allocation_size * information->name_length) +
        (information->allocation_size * information->size)
      );

    if ( !information->object_blocks[ block ] )
      return;
  }
  else {
    information->object_blocks[ block ] = 
      _Workspace_Allocate_or_fatal_error(
        (information->allocation_size * information->name_length) +
        (information->allocation_size * information->size)
      );
  }
  
  name_area = (Objects_Name *) _Addresses_Add_offset(
    information->object_blocks[ block ],
    (information->allocation_size * information->size)
  );
  information->name_table[ block ] = name_area;

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
  
  while ( (the_object = (Objects_Control *) _Chain_Get( &Inactive ) ) != NULL ) {
    
    the_object->id = 
      _Objects_Build_id(
        information->the_class, _Objects_Local_node, index
      );
      
    the_object->name = (void *) name_area;

    name_area = _Addresses_Add_offset( name_area, information->name_length );

    _Chain_Append( &information->Inactive, &the_object->Node );

    index++;
  }
  
  information->inactive_per_block[ block ] = information->allocation_size;
  information->inactive += information->allocation_size;
}
