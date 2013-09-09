/**
 *  @file
 *
 *  @brief RTEMS Port External to Internal
 *  @ingroup ClassicDPMEM
 */

/*
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
#include <rtems/rtems/dpmemimpl.h>
#include <rtems/score/thread.h>

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
      _Objects_Put( &the_port->Object );
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:        /* this error cannot be returned */
#endif
    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
