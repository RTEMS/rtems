/*
 *  RTEMS ID To Name Lookup
 *
 *
 *  COPYRIGHT (c) 1989-2003.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

/*PAGE
 *
 *  rtems_object_id_to_name
 *
 *  This directive returns the name associated with the specified 
 *  object ID.
 *
 *  Input parameters:
 *    id   - message queue id
 *
 *  Output parameters:
 *    *name            - user defined object name
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_object_id_to_name(
  rtems_id      id,
  rtems_name   *name
)
{
  Objects_Name_or_id_lookup_errors  status;

  status = _Objects_Id_to_name( id, (Objects_Name *) name );

  return _Status_Object_name_errors_to_status[ status ];
}
