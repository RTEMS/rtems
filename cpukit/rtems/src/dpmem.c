/*
 *  Dual Port Memory Manager
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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
 *  _Dual_ported_memory_Manager_initialization
 *
 *  This routine initializes all dual-ported memory manager related
 *  data structures.
 *
 *  Input parameters:
 *    maximum_ports - number of ports to initialize
 *
 *  Output parameters:  NONE
 */

void _Dual_ported_memory_Manager_initialization(
  unsigned32 maximum_ports
)
{
  _Objects_Initialize_information(
    &_Dual_ported_memory_Information,
    OBJECTS_RTEMS_PORTS,
    FALSE,
    maximum_ports,
    sizeof( Dual_ported_memory_Control ),
    FALSE,
    RTEMS_MAXIMUM_NAME_LENGTH,
    FALSE
  );
}

/*PAGE
 *
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
  unsigned32    length,
  Objects_Id   *id
)
{
  register Dual_ported_memory_Control *the_port;

  if ( !rtems_is_name_valid( name) )
    return RTEMS_INVALID_NAME;

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
    &name
  );

  *id = the_port->Object.id;
  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}

/*PAGE
 *
 *  rtems_port_ident
 *
 *  This directive returns the system ID associated with
 *  the port name.
 *
 *  Input parameters:
 *    name - user defined port name
 *    id   - pointer to port id
 *
 *  Output parameters:
 *    *id      - port id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_port_ident(
  rtems_name    name,
  Objects_Id   *id
)
{
  Objects_Name_to_id_errors  status;

  status = _Objects_Name_to_id(
    &_Dual_ported_memory_Information,
    &name,
    OBJECTS_SEARCH_ALL_NODES,
    id
  );

  return _Status_Object_name_errors_to_status[ status ];
}

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

/*PAGE
 *
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
  Objects_Id   id,
  void        *external,
  void       **internal
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
      ending = _Addresses_Subtract( external, the_port->external_base );
      if ( ending > the_port->length )
        *internal = external;
      else
        *internal = _Addresses_Add_offset( the_port->internal_base,
                                           ending );
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
