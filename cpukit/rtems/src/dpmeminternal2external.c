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
 *  rtems_port_internal_to_external
 *
 *  This directive converts an internal dual-ported memory address to an
 *  external dual-ported memory address.  If the given internal address
 *  is an invalid dual-ported address, then the external address is set
 *  to the given internal address.
 *
 *  Input parameters:
 *    id       - id of dual-ported memory object
 *    internal - internal address to set
 *    external - pointer to external address
 *
 *  Output parameters:
 *    external          - external address
 *    RTEMS_SUCCESSFUL - always succeeds
 */

rtems_status_code rtems_port_internal_to_external(
  Objects_Id   id,
  void        *internal,
  void       **external
)
{
  register Dual_ported_memory_Control *the_port;
  Objects_Locations                    location;
  unsigned32                           ending;

  the_port = _Dual_ported_memory_Get( id, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:        /* this error cannot be returned */
      return RTEMS_INTERNAL_ERROR;

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      ending = _Addresses_Subtract( internal, the_port->internal_base );
      if ( ending > the_port->length )
        *external = internal;
      else
        *external = _Addresses_Add_offset( the_port->external_base,
                                           ending );
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
