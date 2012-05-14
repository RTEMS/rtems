/*
 *  Dual Port Memory Manager
 *
 *  COPYRIGHT (c) 1989-1999.
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
 *  rtems_port_create
 *
 *  This directive creates a port into a dual-ported memory area.
 *
 *  Input parameters:
 *    name           - user defined port name
 *    internal_start - internal start address of port
 *    external_start - external start address of port
 *    length         - physical length in bytes
 *    id             - address of port id to set
 *
 *  Output parameters:
 *    id       - port id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_port_create(
  rtems_name    name,
  void         *internal_start,
  void         *external_start,
  uint32_t      length,
  rtems_id     *id
)
{
  register Dual_ported_memory_Control *the_port;

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

  if ( !id )
    return RTEMS_INVALID_ADDRESS;

  if ( !_Addresses_Is_aligned( internal_start ) ||
       !_Addresses_Is_aligned( external_start ) )
    return RTEMS_INVALID_ADDRESS;

  _Thread_Disable_dispatch();             /* to prevent deletion */

  the_port = _Dual_ported_memory_Allocate();

  if ( !the_port ) {
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }

  the_port->internal_base = internal_start;
  the_port->external_base = external_start;
  the_port->length        = length - 1;

  _Objects_Open(
    &_Dual_ported_memory_Information,
    &the_port->Object,
    (Objects_Name) name
  );

  *id = the_port->Object.id;
  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}
