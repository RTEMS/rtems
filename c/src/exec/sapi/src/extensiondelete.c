/*
 *  Extension Manager -- rtems_extension_delete
 *
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/support.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/extension.h>

/*PAGE
 *
 *  rtems_extension_delete
 *
 *  This directive allows a thread to delete a extension.
 *
 *  Input parameters:
 *    id - extension id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_extension_delete(
  Objects_Id id
)
{
  Extension_Control   *the_extension;
  Objects_Locations    location;

  the_extension = _Extension_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:            /* should never return this */
      return RTEMS_INVALID_ID;
    case OBJECTS_LOCAL:
      _User_extensions_Remove_set( &the_extension->Extension );
      _Objects_Close( &_Extension_Information, &the_extension->Object );
      _Extension_Free( the_extension );
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
