/*
 *  Dual Port Memory Manager
 *
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/address.h>
#include <rtems/rtems/dpmem.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/dpmem.h>

/*
 *  rtems_port_external_to_internal
 *
 *  This directive converts an external dual-ported memory address to an
 *  internal dual-ported memory address.  If the given external address
 *  is an invalid dual-ported address, then the internal address is set
 *  to the given external address.
 *
 *  Input parameters:
 *    id       - id of dp memory object
 *    external - external address
 *    internal - pointer of internal address to set
 *
 *  Output parameters:
 *    internal          - internal address
 *    RTEMS_SUCCESSFUL - always succeeds
 */

rtems_status_code rtems_port_external_to_internal(
  rtems_id   id,
  void      *external,
  void     **internal
)
{
  register Dual_ported_memory_Control *the_port;
  Objects_Locations                    location;
  uint32_t                             ending;

  if ( !internal )
    return RTEMS_INVALID_ADDRESS;

  the_port = _Dual_ported_memory_Get( id, &location );
  switch ( location ) {
    case OBJECTS_LOCAL:
      ending = _Addresses_Subtract( external, the_port->external_base );
      if ( ending > the_port->length )
        *internal = external;
      else
        *internal = _Addresses_Add_offset( the_port->internal_base,
                                           ending );
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:        /* this error cannot be returned */
#endif
    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
