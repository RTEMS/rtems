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
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
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

EXTERN Objects_Information  _Dual_ported_memory_Information;

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

/*
 *  _Dual_ported_memory_Allocate
 *
 *  DESCRIPTION:
 *
 *  This routine allocates a port control block from the inactive chain
 *  of free port control blocks.
 */

STATIC INLINE Dual_ported_memory_Control
  *_Dual_ported_memory_Allocate ( void );

/*
 *  _Dual_ported_memory_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a port control block to the inactive chain
 *  of free port control blocks.
 */

STATIC INLINE void _Dual_ported_memory_Free (
   Dual_ported_memory_Control *the_port
);

/*
 *  _Dual_ported_memory_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps port IDs to port control blocks.  If ID
 *  corresponds to a local port, then it returns the_port control
 *  pointer which maps to ID and location is set to OBJECTS_LOCAL.
 *  Global ports are not supported, thus if ID  does not map to a
 *  local port, location is set to OBJECTS_ERROR and the_port is
 *  undefined.
 */

STATIC INLINE Dual_ported_memory_Control *_Dual_ported_memory_Get (
  Objects_Id         id,
  Objects_Locations *location
);

/*
 *  _Dual_ported_memory_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_port is NULL and FALSE otherwise.
 */

STATIC INLINE boolean _Dual_ported_memory_Is_null(
  Dual_ported_memory_Control *the_port
);

#include <rtems/rtems/dpmem.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
