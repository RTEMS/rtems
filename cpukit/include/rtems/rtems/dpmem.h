/**
 * @file rtems/rtems/dpmem.h
 *
 * @defgroup ClassicDPMEM Dual Ported Memory
 *
 * @ingroup ClassicRTEMS
 * @brief Dual Ported Memory Manager
 *
 * This include file contains all the constants and structures associated
 * with the Dual Ported Memory Manager. This manager provides a mechanism
 * for converting addresses between internal and external representations
 * for multiple dual-ported memory areas.
 *
 * Directives provided are:
 *
 * - create a port
 * - get ID of a port
 * - delete a port
 * - convert external to internal address
 * - convert internal to external address
 *
 */

/* COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_DPMEM_H
#define _RTEMS_RTEMS_DPMEM_H

#include <rtems/rtems/types.h>
#include <rtems/rtems/status.h>

#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief Creates a port into a dual-ported memory area.
 *
 * This routine implements the rtems_port_create directive. The port
 * will have the name @a name. The port maps onto an area of dual ported
 * memory of length bytes which has internal_start and external_start
 * as the internal and external starting addresses, respectively.
 * It returns the id of the created port in ID.
 *
 * @param[in] name is the user defined port name
 * @param[in] internal_start is the internal start address of port
 * @param[in] external_start is the external start address of port
 * @param[in] length is the physical length in bytes
 * @param[out] id is the address of port id to set
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error. If successful, the id will
 *         be filled in with the port id.
 */
rtems_status_code rtems_port_create(
  rtems_name    name,
  void         *internal_start,
  void         *external_start,
  uint32_t      length,
  rtems_id     *id
);

/**
 * @brief RTEMS Port Name to Id
 *
 * This routine implements the rtems_port_ident directive. This directive
 * returns the port ID associated with name. If more than one port is
 * named name, then the port to which the ID belongs is arbitrary.
 *
 * @param[in] name is the user defined port name
 * @param[out] id is the pointer to port id
 *
 * @retval RTEMS_SUCCESSFUL if successful or error code if unsuccessful
 */
rtems_status_code rtems_port_ident(
  rtems_name    name,
  rtems_id     *id
);

/**
 * @brief RTEMS Delete Port
 *
 * This routine implements the rtems_port_delete directive. It deletes
 * the port associated with ID.
 *
 * @param[in] id is the dual-ported memory area id
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code rtems_port_delete(
  rtems_id   id
);

/**
 * @brief RTEMS Port External to Internal
 *
 * This routine implements the rtems_port_external_to_internal directive.
 * It returns the internal port address which maps to the provided
 * external port address for the specified port ID. If the given external
 * address is an invalid dual-ported address, then the internal address is
 * set to the given external address.
 *
 * @param[in] id is the id of dp memory object
 * @param[in] external is the external address
 * @param[out] internal is the pointer of internal address to set
 *
 * @retval RTEMS_SUCCESSFUL
 */
rtems_status_code rtems_port_external_to_internal(
  rtems_id     id,
  void        *external,
  void       **internal
);

/**
 * @brief RTEMS Port Internal to External
 *
 * This routine implements the Port_internal_to_external directive.
 * It returns the external port address which maps to the provided
 * internal port address for the specified port ID. If the given
 * internal address is an invalid dual-ported address, then the
 * external address is set to the given internal address.
 *
 * @param[in] id is the id of dual-ported memory object
 * @param[in] internal is the internal address to set
 * @param[in] external is the pointer to external address
 *
 * @retval RTEMS_SUCCESSFUL and the external will be filled in
 * with the external addresses
 */
rtems_status_code rtems_port_internal_to_external(
  rtems_id     id,
  void        *internal,
  void       **external
);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
