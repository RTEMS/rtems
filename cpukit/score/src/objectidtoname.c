/*
 *  Obtain Object Name Given ID
 *
 *
 *  COPYRIGHT (c) 1989-2003.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/object.h>

/*PAGE
 *
 *  _Objects_Id_to_name
 *
 *  DESCRIPTION:
 *
 *  This routine returns the name associated with the given ID.
 *
 *  INPUT:
 *
 *  id   - id of object to lookup name
 *  name - pointer to location in which to store name
 *
 */


Objects_Name_or_id_lookup_errors _Objects_Id_to_name (
  Objects_Id      id,
  Objects_Name   *name
)
{
  unsigned32           the_api;
  unsigned32           the_class;
  Objects_Information *information;
  Objects_Control     *the_object = (Objects_Control *) 0;
 
  if ( !name )
    return OBJECTS_INVALID_NAME;

  the_api = _Objects_Get_API( id );
  if ( the_api && the_api > OBJECTS_APIS_LAST )
    return OBJECTS_INVALID_ID;
  
  the_class = _Objects_Get_class( id );

  information = _Objects_Information_table[ the_api ][ the_class ];
  if ( !information )
    return OBJECTS_INVALID_ID;
 
  if ( information->is_string )
    return OBJECTS_INVALID_ID;

  the_object = _Objects_Get( information, id, OBJECTS_SEARCH_LOCAL_NODE );
  if (!the_object)
    return OBJECTS_INVALID_ID;

  *name = the_object->name;
  return OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL;
}
