/**
 *  @file
 *
 *  @brief RTEMS Delete Port
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

rtems_status_code rtems_port_delete(
  rtems_id id
)
{
  register Dual_ported_memory_Control *the_port;
  Objects_Locations                    location;

  the_port = _Dual_ported_memory_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      _Objects_Close( &_Dual_ported_memory_Information, &the_port->Object );
      _Dual_ported_memory_Free( the_port );
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
