/*
 *  RTEMS Object Helper -- Set Name of Object as String
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/rtems/object.h>

/*
 *  This method will set the object name based upon the user string.
 *  If the object class uses 32-bit names, then only the first 4 bytes
 *  of the string will be used.
 */
rtems_status_code rtems_object_set_name(
  rtems_id       id,
  const char    *name
)
{
  Objects_Information *information;
  Objects_Locations    location;
  Objects_Control     *the_object;
  Objects_Id           tmpId;

  if ( !name )
    return RTEMS_INVALID_ADDRESS;

  tmpId = (id == OBJECTS_ID_OF_SELF) ? _Thread_Executing->Object.id : id;

  information  = _Objects_Get_information_id( tmpId );
  if ( !information )
    return RTEMS_INVALID_ID;

  the_object = _Objects_Get( information, tmpId, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      _Objects_Set_name( information, the_object, name );
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
