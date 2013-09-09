/**
 *  @file
 *
 *  @brief RTEMS Port Internal to External
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

rtems_status_code rtems_port_internal_to_external(
  rtems_id   id,
  void      *internal,
  void     **external
)
{
  register Dual_ported_memory_Control *the_port;
  Objects_Locations                    location;
  uint32_t                             ending;

  if ( !external )
    return RTEMS_INVALID_ADDRESS;

  the_port = _Dual_ported_memory_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      ending = _Addresses_Subtract( internal, the_port->internal_base );
      if ( ending > the_port->length )
        *external = internal;
      else
        *external = _Addresses_Add_offset( the_port->external_base,
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
