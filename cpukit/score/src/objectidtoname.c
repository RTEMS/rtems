/*
 *  @file
 *
 *  @brief Object id to name
 *  @ingroup RTEMSScore
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
  Objects_Id           tmpId;
  Objects_Information *information;
  Objects_Control     *the_object;
  ISR_lock_Context     lock_context;

  /*
   *  Caller is trusted for name != NULL.
   */

  tmpId = (id == OBJECTS_ID_OF_SELF) ? _Thread_Get_executing()->Object.id : id;

  information = _Objects_Get_information_id( tmpId );
  if ( !information )
    return OBJECTS_INVALID_ID;

  if ( _Objects_Has_string_name( information ) )
    return OBJECTS_INVALID_ID;

  the_object = _Objects_Get(
    tmpId,
    &lock_context,
    information
  );
  if ( !the_object )
    return OBJECTS_INVALID_ID;

  *name = the_object->name;
  _ISR_lock_ISR_enable( &lock_context );
  return OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL;
}
