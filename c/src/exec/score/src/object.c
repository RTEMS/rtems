/*
 *  Object Handler
 *
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
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

/*PAGE
 *
 *  _Objects_Handler_initialization
 *
 *  This routine initializes the object handler.
 *
 *  Input parameters:
 *    node                   - local node
 *    maximum_nodes          - number of nodes in the system
 *    maximum_global_objects - number of configured global objects
 *
 *  Output parameters:  NONE
 */

void _Objects_Handler_initialization(
  unsigned32 node,
  unsigned32 maximum_nodes,
  unsigned32 maximum_global_objects
)
{
  if ( node < 1 || node > maximum_nodes )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      INTERNAL_ERROR_INVALID_NODE
    );

  _Objects_Local_node    = node;
  _Objects_Maximum_nodes = maximum_nodes;

#if defined(RTEMS_MULTIPROCESSING)
  _Objects_MP_Handler_initialization(
    node,
    maximum_nodes,
    maximum_global_objects
  );
#endif
}

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
  index_base = minimum_index;
  block = 0;
  
  if (information->maximum < minimum_index)
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
     *  Growing the tables means allocating a new area, doing a copy and updating
     *  the information table.
     *
     *  If the maximum is minimum we do not have a table to copy. First time through.
     *
     *  The allocation has :
     *
     *      void            *objects[block_count];
     *      unsiged32        inactive_count[block_count];
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
          block_count * (sizeof(void *) + sizeof(unsigned32) + sizeof(Objects_Name *)) +
          ((maximum + minimum_index) * sizeof(Objects_Control *))
          );

      if ( !object_blocks )
        return;
    }
    else {
      object_blocks = (void**)
        _Workspace_Allocate_or_fatal_error(
          block_count * (sizeof(void *) + sizeof(unsigned32) + sizeof(Objects_Name *)) +
          ((maximum + minimum_index) * sizeof(Objects_Control *))
        );
    }

    /*
     *  Break the block into the various sections.
     *
     */
     
    inactive_per_block =
      (unsigned32 *) _Addresses_Add_offset( object_blocks, block_count * sizeof(void*) );
    name_table =
        (Objects_Name *) _Addresses_Add_offset( inactive_per_block,
                                                block_count * sizeof(unsigned32) );
    local_table =
      (Objects_Control **) _Addresses_Add_offset( name_table,
                                                  block_count * sizeof(Objects_Name *) );
    
    /*
     *  Take the block count down. Saves all the (block_count - 1) in the copies.
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
  
  name_area = (Objects_Name *) information->object_blocks[ block ];
  information->name_table[ block ] = name_area;

  /*
   *  Initialize objects .. add to a local chain first.
   */

  _Chain_Initialize(
    &Inactive,
    _Addresses_Add_offset( information->object_blocks[ block ],
                           (information->allocation_size * information->name_length) ),
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

/*PAGE
 *
 *  _Objects_Initialize_information
 *
 *  This routine initializes all object information related data structures.
 *
 *  Input parameters:
 *    information     - object information table
 *    the_class       - object class
 *    supports_global - TRUE if this is a global object class
 *    maximum         - maximum objects of this class
 *    is_string       - TRUE if names for this object are strings
 *    size            - size of this object's control block
 *    is_thread       - TRUE if this class is threads
 *
 *  Output parameters:  NONE
 */

void _Objects_Initialize_information(
  Objects_Information *information,
  Objects_Classes      the_class,
  boolean              supports_global,
  unsigned32           maximum,
  unsigned32           size,
  boolean              is_string,
  unsigned32           maximum_name_length,
  boolean              is_thread
)
{
  unsigned32       minimum_index;
  unsigned32       index;
  unsigned32       name_length;

  information->the_class          = the_class; 
  information->is_string          = is_string; 
  information->is_thread          = is_thread;
  
  information->local_table        = 0;
  information->name_table         = 0;
  information->inactive_per_block = 0;
  information->object_blocks      = 0;
  
  information->inactive           = 0;
  
  /*
   *  Set the entry in the object information table.
   */

  _Objects_Information_table[ the_class ] = information;

  /*
   *  Set the size of the object
   */

  information->size = size;
  
  /*
   *  Are we operating in unlimited, or auto-extend mode
   */

  information->auto_extend = (maximum & OBJECTS_UNLIMITED_OBJECTS) ? TRUE : FALSE;
  maximum &= ~OBJECTS_UNLIMITED_OBJECTS;
  
  /*
   *  The allocation unit is the maximum value
   */

  information->allocation_size = maximum;

  /*
   *  Calculate minimum and maximum Id's
   */

  if ( maximum == 0 ) minimum_index = 0;
  else                minimum_index = 1;

  information->minimum_id =
    _Objects_Build_id( the_class, _Objects_Local_node, minimum_index );

  /*
   *  Calculate the maximum name length
   */

  name_length = maximum_name_length;

  if (name_length & (OBJECTS_NAME_ALIGNMENT-1))
    name_length = (name_length + OBJECTS_NAME_ALIGNMENT) & 
                  ~(OBJECTS_NAME_ALIGNMENT-1);

  information->name_length = name_length;

  _Chain_Initialize_empty( &information->Inactive );
    
  /*
   *  Initialize objects .. if there are any
   */

  if ( maximum ) {

    /*
     *  Reset the maximum value. It will be updated when the information is
     *  extended.
     */
    
    information->maximum = 0;
    
    /*
     *  Always have the maximum size available so the current performance
     *  figures are create are met.  If the user moves past the maximum
     *  number then a performance hit is taken.
     */
    
    _Objects_Extend_information( information );
    
  }

  /*
   *  Take care of multiprocessing
   */

  if ( supports_global == TRUE && _System_state_Is_multiprocessing ) {

    information->global_table =
      (Chain_Control *) _Workspace_Allocate_or_fatal_error(
        (_Objects_Maximum_nodes + 1) * sizeof(Chain_Control)
      );

    for ( index=1; index <= _Objects_Maximum_nodes ; index++ )
      _Chain_Initialize_empty( &information->global_table[ index ] );
   }
   else
     information->global_table = NULL;
}

/*PAGE
 *
 *  _Objects_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a object control block from
 *  the inactive chain of free object control blocks.
 */

Objects_Control *_Objects_Allocate(
  Objects_Information *information
)
{
  Objects_Control *the_object =  
    (Objects_Control *) _Chain_Get( &information->Inactive );

  if ( information->auto_extend ) {
    /*
     *  If the list is empty then we are out of objects and need to
     *  extend information base.
     */
  
    if ( !the_object ) {
      _Objects_Extend_information( information );
      the_object =  (Objects_Control *) _Chain_Get( &information->Inactive );
    }
  
    if ( the_object ) {
      unsigned32 block;
    
      block = 
        _Objects_Get_index( the_object->id ) - _Objects_Get_index( information->minimum_id );
      block /= information->allocation_size;
      
      information->inactive_per_block[ block ]--;
      information->inactive--;
    }
  }
  
  return the_object;
}

/*PAGE
 *
 *  _Objects_Free
 *
 *  DESCRIPTION:
 *
 *  This function frees a object control block to the
 *  inactive chain of free object control blocks.
 */

void _Objects_Free(
  Objects_Information *information,
  Objects_Control     *the_object
)
{
  unsigned32  allocation_size = information->allocation_size;

  _Chain_Append( &information->Inactive, &the_object->Node );

  if ( information->auto_extend ) {
    unsigned32  block;
    
    block = 
      _Objects_Get_index( the_object->id ) - _Objects_Get_index( information->minimum_id );
    block /= information->allocation_size;
      
    information->inactive_per_block[ block ]++;
    information->inactive++;
  
    /*
     *  Check if the threshold level has been met of
     *  1.5 x allocation_size are free.
     */

    if ( information->inactive > ( allocation_size + ( allocation_size >> 1 ) ) ) {
      _Objects_Shrink_information( information );
    }
  }
}

/*PAGE
 *
 *  _Objects_Clear_name
 *
 *  XXX
 */

void _Objects_Clear_name(
  void       *name,
  unsigned32  length
)
{
  unsigned32  index;
  unsigned32  maximum = length / OBJECTS_NAME_ALIGNMENT;
  unsigned32 *name_ptr = (unsigned32 *) name;

  for ( index=0 ; index < maximum ; index++ ) 
    *name_ptr++ = 0;
}
 
/*PAGE
 *
 *  _Objects_Copy_name_string
 *
 *  XXX
 */
 
void _Objects_Copy_name_string(
  void       *source,
  void       *destination
)
{
  unsigned8 *source_p = (unsigned8 *) source;
  unsigned8 *destination_p = (unsigned8 *) destination;
 
  do {
    *destination_p++ = *source_p;
  } while ( *source_p++ );
}

/*PAGE
 *
 *  _Objects_Copy_name_raw
 *
 *  XXX
 */
 
void _Objects_Copy_name_raw(
  void       *source,
  void       *destination,
  unsigned32  length
)
{
  unsigned32 *source_p = (unsigned32 *) source;
  unsigned32 *destination_p = (unsigned32 *) destination;
  unsigned32  tmp_length = length / OBJECTS_NAME_ALIGNMENT;
 
  while ( tmp_length-- )
    *destination_p++ = *source_p++;
}

/*PAGE
 *
 *  _Objects_Compare_name_string
 *
 *  XXX
 */
 
boolean _Objects_Compare_name_string(
  void       *name_1,
  void       *name_2,
  unsigned32  length
)
{
  unsigned8 *name_1_p = (unsigned8 *) name_1;
  unsigned8 *name_2_p = (unsigned8 *) name_2;
  unsigned32 tmp_length = length;
 
  do {
    if ( *name_1_p++ != *name_2_p++ )
      return FALSE;
    if ( !tmp_length-- )
      return FALSE;
  } while ( *name_1_p );

  return TRUE;
}
 
/*PAGE
 *
 *  _Objects_Compare_name_raw
 *
 *  XXX
 */
 
boolean _Objects_Compare_name_raw(
  void       *name_1,
  void       *name_2,
  unsigned32  length
)
{
  unsigned32 *name_1_p = (unsigned32 *) name_1;
  unsigned32 *name_2_p = (unsigned32 *) name_2;
  unsigned32  tmp_length = length / OBJECTS_NAME_ALIGNMENT;
 
  while ( tmp_length-- )
    if ( *name_1_p++ != *name_2_p++ )
      return FALSE;

  return TRUE;
}


/*PAGE
 *
 *  _Objects_Name_to_id
 *
 *  These kernel routines search the object table(s) for the given
 *  object name and returns the associated object id.
 *
 *  Input parameters:
 *    information - object information
 *    name        - user defined object name
 *    node        - node indentifier (0 indicates any node)
 *    id          - address of return ID
 *
 *  Output parameters:
 *    id                 - object id
 *    OBJECTS_SUCCESSFUL - if successful
 *    error code         - if unsuccessful
 */

Objects_Name_to_id_errors _Objects_Name_to_id(
  Objects_Information *information,
  Objects_Name         name,
  unsigned32           node,
  Objects_Id          *id
)
{
  boolean                    search_local_node;
  Objects_Control           *the_object;
  unsigned32                 index;
  unsigned32                 name_length;
  Objects_Name_comparators   compare_them;

  if ( name == 0 )
    return OBJECTS_INVALID_NAME;

  search_local_node = FALSE;

  if ( information->maximum != 0 &&
      (node == OBJECTS_SEARCH_ALL_NODES || node == OBJECTS_SEARCH_LOCAL_NODE ||
      _Objects_Is_local_node( node ) ) )
   search_local_node = TRUE;

  if ( search_local_node ) {
    name_length = information->name_length;

    if ( information->is_string ) compare_them = _Objects_Compare_name_string;
    else                          compare_them = _Objects_Compare_name_raw;

    for ( index = 1; index <= information->maximum; index++ ) {

      the_object = information->local_table[ index ];

      if ( !the_object || !the_object->name )
        continue;

      if ( (*compare_them)( name, the_object->name, name_length ) ) {
        *id = the_object->id;
        return OBJECTS_SUCCESSFUL;
      }
    }
  }

  if ( _Objects_Is_local_node( node ) || node == OBJECTS_SEARCH_LOCAL_NODE )
    return OBJECTS_INVALID_NAME;

#if defined(RTEMS_MULTIPROCESSING)
  return ( _Objects_MP_Global_name_search( information, name, node, id ) );
#else
  return OBJECTS_INVALID_NAME;
#endif
}

/*PAGE
 *
 * _Objects_Get
 *
 * This routine sets the object pointer for the given
 * object id based on the given object information structure.
 *
 * Input parameters:
 *   information - pointer to entry in table for this class
 *   id          - object id to search for
 *   location    - address of where to store the location
 *
 * Output parameters:
 *   returns - address of object if local
 *   location    - one of the following:
 *                  OBJECTS_ERROR  - invalid object ID
 *                  OBJECTS_REMOTE - remote object
 *                  OBJECTS_LOCAL  - local object
 */

Objects_Control *_Objects_Get(
  Objects_Information *information,
  Objects_Id           id,
  Objects_Locations   *location
)
{
  Objects_Control *the_object;
  unsigned32       index;

  index = _Objects_Get_index( id );

  if ( information->maximum >= index ) {
    _Thread_Disable_dispatch();
    if ( (the_object = _Objects_Get_local_object( information, index )) != NULL ) {
      *location = OBJECTS_LOCAL;
      return( the_object );
    }
    _Thread_Enable_dispatch();
    *location = OBJECTS_ERROR;
    return( NULL );
  }
  *location = OBJECTS_ERROR;
#if defined(RTEMS_MULTIPROCESSING)
  _Objects_MP_Is_remote( information, id, location, &the_object );
  return the_object;
#else
  return NULL;
#endif
}


/*PAGE
 *
 * _Objects_Get_next
 *
 * Like _Objects_Get, but considers the 'id' as a "hint" and
 * finds next valid one after that point.
 * Mostly used for monitor and debug traversal of an object.
 *
 * Input parameters:
 *   information - pointer to entry in table for this class
 *   id          - object id to search for
 *   location    - address of where to store the location
 *   next_id     - address to store next id to try
 *
 * Output parameters:
 *   returns     - address of object if local
 *   location    - one of the following:
 *                  OBJECTS_ERROR  - invalid object ID
 *                  OBJECTS_REMOTE - remote object
 *                  OBJECTS_LOCAL  - local object
 *   next_id     - will contain a reasonable "next" id to continue traversal
 *
 * NOTE:
 *      assumes can add '1' to an id to get to next index.
 */

Objects_Control *
_Objects_Get_next(
    Objects_Information *information,
    Objects_Id           id,
    Objects_Locations   *location_p,
    Objects_Id          *next_id_p
)
{
    Objects_Control *object;
    Objects_Id       next_id;
    
    if (_Objects_Get_index(id) == OBJECTS_ID_INITIAL_INDEX)
        next_id = information->minimum_id;
    else
        next_id = id;

    do {
        /* walked off end of list? */
        if (_Objects_Get_index(next_id) > information->maximum)
        {
            *location_p = OBJECTS_ERROR;
            goto final;
        }
        
        /* try to grab one */
        object = _Objects_Get(information, next_id, location_p);

        next_id++;

    } while (*location_p != OBJECTS_LOCAL);

    *next_id_p = next_id;
    return object;

final:
    *next_id_p = OBJECTS_ID_FINAL;
    return 0;
}

