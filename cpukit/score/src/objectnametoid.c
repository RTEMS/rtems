/*
 *  Object Handler
 *
 *  COPYRIGHT (c) 1989-2010.
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

/*
 *  _Objects_Name_to_id_u32
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
 *    id                                   - object id
 *    OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL - if successful
 *    error code                           - if unsuccessful
 */

Objects_Name_or_id_lookup_errors _Objects_Name_to_id_u32(
  Objects_Information *information,
  uint32_t             name,
  uint32_t             node,
  Objects_Id          *id
)
{
  bool                       search_local_node;
  Objects_Control           *the_object;
  uint32_t                   index;
#if defined(RTEMS_MULTIPROCESSING)
  Objects_Name               name_for_mp;
#endif

  /* ASSERT: information->is_string == false */

  if ( !id )
    return OBJECTS_INVALID_ADDRESS;

  if ( name == 0 )
    return OBJECTS_INVALID_NAME;

  search_local_node = false;

  if ( information->maximum != 0 &&
      (node == OBJECTS_SEARCH_ALL_NODES ||
       node == OBJECTS_SEARCH_LOCAL_NODE ||
       _Objects_Is_local_node( node )
      ))
   search_local_node = true;

  if ( search_local_node ) {
    for ( index = 1; index <= information->maximum; index++ ) {
      the_object = information->local_table[ index ];
      if ( !the_object )
        continue;

      if ( name == the_object->name.name_u32 ) {
        *id = the_object->id;
        return OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL;
      }
    }
  }

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Objects_Is_local_node( node ) || node == OBJECTS_SEARCH_LOCAL_NODE )
    return OBJECTS_INVALID_NAME;

  name_for_mp.name_u32 = name;
  return _Objects_MP_Global_name_search( information, name_for_mp, node, id );
#else
  return OBJECTS_INVALID_NAME;
#endif
}
