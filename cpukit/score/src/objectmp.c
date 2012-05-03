/*
 *  Multiprocessing Support for the Object Handler
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
#include <rtems/config.h>
#include <rtems/score/interr.h>
#include <rtems/score/object.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/thread.h>

/*
 *  _Objects_MP_Handler_early_initialization
 *
 */
void _Objects_MP_Handler_early_initialization(void)
{
  uint32_t   node;
  uint32_t   maximum_nodes;

  node                   = _Configuration_MP_table->node;
  maximum_nodes          = _Configuration_MP_table->maximum_nodes;

  if ( node < 1 || node > maximum_nodes )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      true,
      INTERNAL_ERROR_INVALID_NODE
    );

  _Objects_Local_node    = node;
  _Objects_Maximum_nodes = maximum_nodes;
}

/*
 *  _Objects_MP_Handler_initialization
 *
 */

void _Objects_MP_Handler_initialization(void)
{

  uint32_t   maximum_global_objects;

  maximum_global_objects = _Configuration_MP_table->maximum_global_objects;

  _Objects_MP_Maximum_global_objects = maximum_global_objects;

  if ( maximum_global_objects == 0 ) {
    _Chain_Initialize_empty( &_Objects_MP_Inactive_global_objects );
    return;
  }

  _Chain_Initialize(
    &_Objects_MP_Inactive_global_objects,
    _Workspace_Allocate_or_fatal_error(
      maximum_global_objects * sizeof( Objects_MP_Control )
    ),
    maximum_global_objects,
    sizeof( Objects_MP_Control )
  );

}

/*
 *  _Objects_MP_Open
 *
 */

void _Objects_MP_Open (
  Objects_Information *information,
  Objects_MP_Control  *the_global_object,
  uint32_t             the_name,      /* XXX -- wrong for variable */
  Objects_Id           the_id
)
{
  the_global_object->Object.id = the_id;
  the_global_object->name      = the_name;

  _Chain_Prepend(
    &information->global_table[ _Objects_Get_node( the_id ) ],
    &the_global_object->Object.Node
  );

}

/*
 *  _Objects_MP_Allocate_and_open
 *
 */

bool _Objects_MP_Allocate_and_open (
  Objects_Information *information,
  uint32_t             the_name,      /* XXX -- wrong for variable */
  Objects_Id           the_id,
  bool                 is_fatal_error
)
{
  Objects_MP_Control  *the_global_object;

  the_global_object = _Objects_MP_Allocate_global_object();
  if ( _Objects_MP_Is_null_global_object( the_global_object ) ) {

    if ( is_fatal_error == false )
      return false;

    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      true,
      INTERNAL_ERROR_OUT_OF_GLOBAL_OBJECTS
    );

  }

  _Objects_MP_Open( information, the_global_object, the_name, the_id );

  return true;
}

/*
 *  _Objects_MP_Close
 *
 */

void _Objects_MP_Close (
  Objects_Information *information,
  Objects_Id           the_id
)
{
  Chain_Control      *the_chain;
  Chain_Node         *the_node;
  Objects_MP_Control *the_object;

  the_chain = &information->global_table[ _Objects_Get_node( the_id ) ];

  for ( the_node = _Chain_First( the_chain ) ;
        !_Chain_Is_tail( the_chain, the_node ) ;
        the_node = _Chain_Next( the_node ) ) {

    the_object = (Objects_MP_Control *) the_node;

    if ( _Objects_Are_ids_equal( the_object->Object.id, the_id ) ) {

      _Chain_Extract( the_node );
      _Objects_MP_Free_global_object( the_object );
      return;
    }

  }

  _Internal_error_Occurred(
    INTERNAL_ERROR_CORE,
    true,
    INTERNAL_ERROR_INVALID_GLOBAL_ID
  );
}

/*
 *  _Objects_MP_Global_name_search
 *
 */

Objects_Name_or_id_lookup_errors _Objects_MP_Global_name_search (
  Objects_Information *information,
  Objects_Name         the_name,
  uint32_t             nodes_to_search,
  Objects_Id          *the_id
)
{
  uint32_t            low_node;
  uint32_t            high_node;
  uint32_t            node_index;
  Chain_Control      *the_chain;
  Chain_Node         *the_node;
  Objects_MP_Control *the_object;
  uint32_t            name_to_use;

  name_to_use = the_name.name_u32;  /* XXX only fixed length names */

  if ( nodes_to_search > _Objects_Maximum_nodes )
    return OBJECTS_INVALID_NODE;

  if ( information->global_table == NULL )
    return OBJECTS_INVALID_NAME;

  if ( nodes_to_search == OBJECTS_SEARCH_ALL_NODES ||
       nodes_to_search == OBJECTS_SEARCH_OTHER_NODES ) {
    low_node = 1;
    high_node = _Objects_Maximum_nodes;
  } else {
    low_node  =
    high_node = nodes_to_search;
  }

  _Thread_Disable_dispatch();

  for ( node_index = low_node ; node_index <= high_node ; node_index++ ) {

    /*
     *  NOTE: The local node was search (if necessary) by
     *        _Objects_Name_to_id_XXX before this was invoked.
     */

    if ( !_Objects_Is_local_node( node_index ) ) {
      the_chain = &information->global_table[ node_index ];

      for ( the_node = _Chain_First( the_chain ) ;
            !_Chain_Is_tail( the_chain, the_node ) ;
            the_node = _Chain_Next( the_node ) ) {

        the_object = (Objects_MP_Control *) the_node;

        if ( the_object->name == name_to_use ) {
          *the_id = the_object->Object.id;
          _Thread_Enable_dispatch();
          return OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL;
        }
      }
    }
  }

  _Thread_Enable_dispatch();
  return OBJECTS_INVALID_NAME;
}

/*
 *  _Objects_MP_Is_remote
 *
 */

void _Objects_MP_Is_remote (
  Objects_Information  *information,
  Objects_Id            the_id,
  Objects_Locations    *location,
  Objects_Control     **the_object
)
{
  uint32_t            node;
  Chain_Control      *the_chain;
  Chain_Node         *the_node;
  Objects_MP_Control *the_global_object;

  node = _Objects_Get_node( the_id );

  /*
   *  NOTE: The local node was search (if necessary) by
   *        _Objects_Name_to_id_XXX before this was invoked.
   *
   *        The NODE field of an object id cannot be 0
   *        because 0 is an invalid node number.
   */

  if ( node == 0 ||
       _Objects_Is_local_node( node ) ||
       node > _Objects_Maximum_nodes ||
       information->global_table == NULL ) {

    *location   = OBJECTS_ERROR;
    *the_object = NULL;
    return;
  }

  _Thread_Disable_dispatch();

  the_chain = &information->global_table[ node ];

  for ( the_node = _Chain_First( the_chain ) ;
        !_Chain_Is_tail( the_chain, the_node ) ;
        the_node = _Chain_Next( the_node ) ) {

    the_global_object = (Objects_MP_Control *) the_node;

    if ( _Objects_Are_ids_equal( the_global_object->Object.id, the_id ) ) {
      _Thread_Unnest_dispatch();
      *location   = OBJECTS_REMOTE;
      *the_object = (Objects_Control *) the_global_object;
      return;
    }
  }

  _Thread_Enable_dispatch();
  *location   = OBJECTS_ERROR;
  *the_object = NULL;

}

