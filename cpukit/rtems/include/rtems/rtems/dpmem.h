/**
 * @file rtems/rtems/dpmem.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Dual Ported Memory Manager. This manager provides a mechanism
 *  for converting addresses between internal and external representations
 *  for multiple dual-ported memory areas.
 *
 *  Directives provided are:
 *
 *     - create a port
 *     - get ID of a port
 *     - delete a port
 *     - convert external to internal address
 *     - convert internal to external address
 *
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_DPMEM_H
#define _RTEMS_RTEMS_DPMEM_H

/**
 *  This constant is defined to extern most of the time when using
 *  this header file.  However by defining it to nothing, the data
 *  declared in this header file can be instantiated.  This is done
 *  in a single per manager file.
 */
#ifndef RTEMS_DPMEM_EXTERN
#define RTEMS_DPMEM_EXTERN extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/status.h>

/**
 *  @defgroup ClassicDPMEM Dual Ported Memory
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality related to the
 *  Classic API Dual Ported Memory Manager.
 */
/**@{*/

/**
 *  The following structure defines the port control block.  Each port
 *  has a control block associated with it.  This control block contains
 *  all information required to support the port related operations.
 */
typedef struct {
  /** This field is the object management portion of a Port instance. */
  Objects_Control  Object;
  /** This field is the base internal address of the port. */
  void            *internal_base;
  /** This field is the base external address of the port. */
  void            *external_base;
  /** This field is the length of dual-ported area of the port. */
  uint32_t         length;
}   Dual_ported_memory_Control;

/**
 *  The following define the internal Dual Ported Memory information.
 */
RTEMS_DPMEM_EXTERN Objects_Information  _Dual_ported_memory_Information;

/**
 *  @brief _Dual_ported_memory_Manager_initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _Dual_ported_memory_Manager_initialization(void);

/**
 *  @brief rtems_port_create
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
  uint32_t      length,
  rtems_id     *id
);

/**
 *  @brief rtems_port_ident
 *
 *  This routine implements the rtems_port_ident directive.  This directive
 *  returns the port ID associated with name.  If more than one port is
 *  named name, then the port to which the ID belongs is arbitrary.
 */
rtems_status_code rtems_port_ident(
  rtems_name    name,
  rtems_id     *id
);

/**
 *  @brief rtems_port_delete
 *
 *  This routine implements the rtems_port_delete directive.  It deletes
 *  the port associated with ID.
 */
rtems_status_code rtems_port_delete(
  rtems_id   id
);

/**
 *  @brief rtems_port_external_to_internal
 *
 *  This routine implements the rtems_port_external_to_internal directive.
 *  It returns the internal port address which maps to the provided
 *  external port address for the specified port ID.
 */
rtems_status_code rtems_port_external_to_internal(
  rtems_id     id,
  void        *external,
  void       **internal
);

/**
 *  @brief rtems_port_internal_to_external
 *
 *  This routine implements the Port_internal_to_external directive.
 *  It returns the external port address which maps to the provided
 *  internal port address for the specified port ID.
 */
rtems_status_code rtems_port_internal_to_external(
  rtems_id     id,
  void        *internal,
  void       **external
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/dpmem.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
