/*
 *  Dual Port Memory Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/address.h>
#include <rtems/rtems/dpmem.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/dpmem.h>

/*PAGE
 *
 *  rtems_port_delete
 *
 *  This directive allows a thread to delete a dual-ported memory area
 *  specified by the dual-ported memory identifier.
 *
 *  Input parameters:
 *    id - dual-ported memory area id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_port_delete(
  Objects_Id id
)
{
  register Dual_ported_memory_Control *the_port;
  Objects_Locations                    location;

  the_port = _Dual_ported_memory_Get( id, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:        /* this error cannot be returned */
      return RTEMS_INTERNAL_ERROR;

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      _Objects_Close( &_Dual_ported_memory_Information, &the_port->Object );
      _Dual_ported_memory_Free( the_port );
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
