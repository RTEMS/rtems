/*
 *  Object Handler
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/chain.h>
#include <rtems/config.h>
#include <rtems/object.h>
#include <rtems/objectmp.h>
#include <rtems/thread.h>
#include <rtems/wkspace.h>

/*PAGE
 *
 *  _Objects_Handler_initialization
 *
 *  This routine initializes the object handler.
 *
 *  Input parameters:
 *    node                   - local node
 *    maximum_global_objects - number of configured global objects
 *
 *  Output parameters:  NONE
 */

void _Objects_Handler_initialization(
  unsigned32 node,
  unsigned32 maximum_global_objects
)
{
  _Objects_Local_node = node;

  _Objects_MP_Handler_initialization( maximum_global_objects );
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
  Objects_Control *the_object;
  unsigned32       name_length;
  void            *name_area;

  information->maximum   = maximum;
  information->the_class = the_class; 
  information->is_string = is_string; 
  information->is_thread = is_thread; 

  /*
   *  Set the entry in the object information table.
   */

  _Objects_Information_table[ the_class ] = information;

  /*
   *  Calculate minimum and maximum Id's
   */

  if ( maximum == 0 ) minimum_index = 0;
  else                minimum_index = 1;

  information->minimum_id =
    _Objects_Build_id( the_class, _Objects_Local_node, minimum_index );

  information->maximum_id =
    _Objects_Build_id( the_class, _Objects_Local_node, maximum );

  /*
   *  Allocate local pointer table
   */

  information->local_table = _Workspace_Allocate_or_fatal_error(
    (maximum + 1) * sizeof(Objects_Control *)
  );

  /*
   *  Allocate name table
   */

  name_length = maximum_name_length;

  if (name_length & (OBJECTS_NAME_ALIGNMENT-1))
    name_length = (name_length + OBJECTS_NAME_ALIGNMENT) & 
                  ~(OBJECTS_NAME_ALIGNMENT-1);

  information->name_length = name_length;

  name_area = _Workspace_Allocate_or_fatal_error( (maximum + 1) * name_length );
  information->name_table = name_area;

  /*
   *  Initialize local pointer table
   */

  for ( index=0 ; index <= maximum ; index++ ) {
     information->local_table[ index ] = NULL;
  }

  /*
   *  Initialize objects .. if there are any
   */

  if ( maximum == 0 ) {
    _Chain_Initialize_empty( &information->Inactive );
  } else {

    _Chain_Initialize(
      &information->Inactive,
      _Workspace_Allocate_or_fatal_error( maximum * size ),
      maximum,
      size
    );

    the_object = (Objects_Control *) information->Inactive.first;
    for ( index=1; index <= maximum ; index++ ) {
      the_object->id = 
        _Objects_Build_id( the_class, _Objects_Local_node, index );
      
      the_object->name = (void *) name_area;

      name_area = _Addresses_Add_offset( name_area, name_length );

      the_object = (Objects_Control *) the_object->Node.next;
    }

  }

  /*
   *  Take care of multiprocessing
   */

  if ( supports_global == TRUE && _Configuration_Is_multiprocessing() ) {

    information->global_table = _Workspace_Allocate_or_fatal_error(
      (_Configuration_MP_table->maximum_nodes + 1) * sizeof(Chain_Control)
    );

    for ( index=1;
          index <= _Configuration_MP_table->maximum_nodes ;
          index++ )
      _Chain_Initialize_empty( &information->global_table[ index ] );
   }
   else
     information->global_table = NULL;
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
  unsigned32 *name_ptr = name;

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
  unsigned8 *source_p = source;
  unsigned8 *destination_p = destination;
 
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
  unsigned32 *source_p = source;
  unsigned32 *destination_p = destination;
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
  unsigned8 *name_1_p = name_1;
  unsigned8 *name_2_p = name_2;
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
  unsigned32 *name_1_p = name_1;
  unsigned32 *name_2_p = name_2;
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
 *    obj_id     - object id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code _Objects_Name_to_id(
  Objects_Information *information,
  Objects_Name         name,
  unsigned32           node,
  Objects_Id          *id
)
{
  boolean                    search_local_node;
  Objects_Control          **objects;
  Objects_Control           *the_object;
  unsigned32                 index;
  unsigned32                 name_length;
  Objects_Name_comparators   compare_them;

  if ( name == 0 )
    return( RTEMS_INVALID_NAME );

  search_local_node = FALSE;

  if ( information->maximum != 0 &&
      (node == RTEMS_SEARCH_ALL_NODES || node == RTEMS_SEARCH_LOCAL_NODE ||
      _Objects_Is_local_node( node ) ) )
   search_local_node = TRUE;

  if ( search_local_node ) {
    objects = information->local_table;

    name_length = information->name_length;

    if ( information->is_string ) compare_them = _Objects_Compare_name_string;
    else                          compare_them = _Objects_Compare_name_raw;

    for ( index = 1; index <= information->maximum; index++ ) {

      the_object = objects[ index ];

      if ( !the_object || !the_object->name )
        continue;

      if ( (*compare_them)( name, the_object->name, name_length ) ) {
        *id = the_object->id;
        return( RTEMS_SUCCESSFUL );
      }
    }
  }

  if ( _Objects_Is_local_node( node ) || node == RTEMS_SEARCH_LOCAL_NODE )
    return( RTEMS_INVALID_NAME );

  return ( _Objects_MP_Global_name_search( information, name, node, id ) );
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

  index = id - information->minimum_id;

  if ( information->maximum >= index ) {
    _Thread_Disable_dispatch();
    if ( (the_object = information->local_table[index+1]) != NULL ) {
      *location = OBJECTS_LOCAL;
      return( the_object );
    }
    _Thread_Enable_dispatch();
    *location = OBJECTS_ERROR;
    return( NULL );
  }
  *location = OBJECTS_ERROR;
  _Objects_MP_Is_remote( information, id, location, &the_object );
  return the_object;
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
    unsigned32          *location_p,
    Objects_Id          *next_id_p
)
{
    Objects_Control *object;
    Objects_Id       next_id;
    
    if (rtems_get_index(id) == RTEMS_OBJECT_ID_INITIAL_INDEX)
        next_id = information->minimum_id;
    else
        next_id = id;

    do {
        /* walked off end of list? */
        if (rtems_get_index(next_id) > information->maximum)
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
    *next_id_p = RTEMS_OBJECT_ID_FINAL;
    return 0;
}
