/*
 *  @file
 *
 *  @brief Object id to name
 *  @ingroup Score
 */

/*
 *  COPYRIGHT (c) 1989-2003.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>

Objects_Name_or_id_lookup_errors _Objects_Id_to_name (
  Objects_Id      id,
  Objects_Name   *name
)
{
  uint32_t             the_api;
  uint32_t             the_class;
  Objects_Id           tmpId;
  Objects_Information *information;
  Objects_Control     *the_object = (Objects_Control *) 0;
  Objects_Locations    ignored_location;
  ISR_lock_Context     lock_context;

  /*
   *  Caller is trusted for name != NULL.
   */

  tmpId = (id == OBJECTS_ID_OF_SELF) ? _Thread_Get_executing()->Object.id : id;

  the_api = _Objects_Get_API( tmpId );
  if ( !_Objects_Is_api_valid( the_api ) )
    return OBJECTS_INVALID_ID;

  if ( !_Objects_Information_table[ the_api ] )
    return OBJECTS_INVALID_ID;

  the_class = _Objects_Get_class( tmpId );

  information = _Objects_Information_table[ the_api ][ the_class ];
  if ( !information )
    return OBJECTS_INVALID_ID;

  #if defined(RTEMS_SCORE_OBJECT_ENABLE_STRING_NAMES)
    if ( information->is_string )
      return OBJECTS_INVALID_ID;
  #endif

  the_object = _Objects_Get_isr_disable(
    information,
    tmpId,
    &ignored_location,
    &lock_context
  );
  if ( !the_object )
    return OBJECTS_INVALID_ID;

  *name = the_object->name;
  _ISR_lock_ISR_enable( &lock_context );
  return OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL;
}
