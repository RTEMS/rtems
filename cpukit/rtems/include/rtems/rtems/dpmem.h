/*  dpmem.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Dual Ported Memory Manager. This manager provides a mechanism
 *  for converting addresses between internal and external representations
 *  for multiple dual-ported memory areas.
 *
 *  Directives provided are:
 *
 *     + create a port
 *     + get ID of a port
 *     + delete a port
 *     + convert external to internal address
 *     + convert internal to external address
 *
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

#ifndef __RTEMS_DUAL_PORTED_MEMORY_h
#define __RTEMS_DUAL_PORTED_MEMORY_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>

/*
 *  The following structure defines the port control block.  Each port
 *  has a control block associated with it.  This control block contains
 *  all information required to support the port related operations.
 */

typedef struct {
  Objects_Control  Object;
  void            *internal_base; /* base internal address */
  void            *external_base; /* base external address */
  unsigned32       length;        /* length of dual-ported area */
}   Dual_ported_memory_Control;

/*
 *  The following define the internal Dual Ported Memory information.
 */

RTEMS_EXTERN Objects_Information  _Dual_ported_memory_Information;

/*
 *  _Dual_ported_memory_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _Dual_ported_memory_Manager_initialization(
  unsigned32 maximum_ports
);

/*
 *  rtems_port_create
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_port_create directive.  The port
 *  will have the name name.  The port maps onto an area of dual ported
 *  memory of length bytes which has internal_start and external_start
 *  as the internal and external starting addresses, respectively.
 *  It returns the id of the created port in ID.
 */

rtems_status_code rtems_port_create(
  rtems_name    name,
  void         *internal_start,
  void         *external_start,
  unsigned32    length,
  Objects_Id   *id
);

/*
 *  rtems_port_ident
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_port_ident directive.  This directive
 *  returns the port ID associated with name.  If more than one port is
 *  named name, then the port to which the ID belongs is arbitrary.
 */

rtems_status_code rtems_port_ident(
  rtems_name    name,
  Objects_Id   *id
);

/*
 *  rtems_port_delete
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_port_delete directive.  It deletes
 *  the port associated with ID.
 */

rtems_status_code rtems_port_delete(
  Objects_Id id
);

/*
 *  rtems_port_external_to_internal
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_port_external_to_internal directive.
 *  It returns the internal port address which maps to the provided
 *  external port address for the specified port ID.
 */

rtems_status_code rtems_port_external_to_internal(
  Objects_Id   id,
  void        *external,
  void       **internal
);

/*
 *  rtems_port_internal_to_external
 *
 *  DESCRIPTION:
 *
 *  This routine implements the Port_internal_to_external directive.
 *  It returns the external port address which maps to the provided
 *  internal port address for the specified port ID.
 */

rtems_status_code rtems_port_internal_to_external(
  Objects_Id   id,
  void        *internal,
  void       **external
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/dpmem.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
