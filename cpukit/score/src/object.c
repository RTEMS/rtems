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
 *    information     - object class
 *    supports_global - TRUE if this is a global object class
 *    maximum         - maximum objects of this class
 *    size            - size of this object's control block
 *
 *  Output parameters:  NONE
 */

void _Objects_Initialize_information(
  Objects_Information *information,
  boolean                     supports_global,
  unsigned32                  maximum,
  unsigned32                  size
)
{
  unsigned32       minimum_index;
  unsigned32       index;
  Objects_Control *the_object;

  information->maximum = maximum;

  if ( maximum == 0 ) minimum_index = 0;
  else                minimum_index = 1;

  information->minimum_id =
    _Objects_Build_id( _Objects_Local_node, minimum_index );

  information->maximum_id =
    _Objects_Build_id( _Objects_Local_node, maximum );

  information->local_table = _Workspace_Allocate_or_fatal_error(
    (maximum + 1) * sizeof(Objects_Control *)
  );

  information->name_table = _Workspace_Allocate_or_fatal_error(
    (maximum + 1) * sizeof(Objects_Name)
  );

  for ( index=0 ; index < maximum ; index++ ) {
     information->local_table[ index ] = NULL;
     information->name_table[ index ]  = 0;
  }

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
    for ( index=1;
          index <= maximum ;
          index++ ) {
      the_object->id = _Objects_Build_id( _Objects_Local_node, index );
      the_object = (Objects_Control *) the_object->Node.next;
    }

  }

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
  Objects_Name                name,
  unsigned32                  node,
  Objects_Id                 *id
)
{
  Objects_Name *names;
  unsigned32    index;

  if ( name == 0 )
    return( RTEMS_INVALID_NAME );

  if ( (information->maximum != 0) &&
       (node == RTEMS_SEARCH_ALL_NODES ||
        node == RTEMS_SEARCH_LOCAL_NODE ||
        _Objects_Is_local_node( node )) ) {
    for ( names = information->name_table, index = 1;
          index <= information->maximum;
          index++
         )
      if ( name == names[ index ] ) {
        *id = _Objects_Build_id( _Objects_Local_node, index );
        return( RTEMS_SUCCESSFUL );
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
  Objects_Id                  id,
  Objects_Locations          *location
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
        if (next_id > information->maximum_id)
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
