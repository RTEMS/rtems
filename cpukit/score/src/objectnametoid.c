/**
 * @file
 *
 * @ingroup RTEMSScoreObject
 *
 * @brief This source file contains the implementation of
 *   _Objects_Name_to_id_u32().
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/objectimpl.h>

static bool _Objects_Is_local_node_search( uint32_t node )
{
  return node == OBJECTS_SEARCH_LOCAL_NODE || _Objects_Is_local_node( node );
}

Objects_Name_or_id_lookup_errors _Objects_Name_to_id_u32(
  uint32_t                   name,
  uint32_t                   node,
  Objects_Id                *id,
  const Objects_Information *information
)
{
#if defined(RTEMS_MULTIPROCESSING)
  Objects_Name name_for_mp;
#endif

  _Assert( !_Objects_Has_string_name( information ) );

  if ( id == NULL ) {
    return OBJECTS_INVALID_ADDRESS;
  }

  if (
    node == OBJECTS_SEARCH_ALL_NODES ||
    _Objects_Is_local_node_search( node )
  ) {
    Objects_Maximum maximum;
    Objects_Maximum index;

    maximum = _Objects_Get_maximum_index( information );

    for ( index = 0; index < maximum; ++index ) {
      const Objects_Control *the_object;

      the_object = information->local_table[ index ];

      if ( the_object != NULL && name == the_object->name.name_u32 ) {
        *id = the_object->id;
        _Assert( name != 0 );
        return OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL;
      }
    }
  }

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Objects_Is_local_node_search( node ) ) {
    return OBJECTS_INVALID_NAME;
  }

  name_for_mp.name_u32 = name;
  return _Objects_MP_Global_name_search( information, name_for_mp, node, id );
#else
  return OBJECTS_INVALID_NAME;
#endif
}
