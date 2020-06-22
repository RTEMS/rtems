/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file defines the Dual-Ported Memory Manager API.
 */

/*
 * Copyright (C) 2020, 2021 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 1988, 2008 On-Line Applications Research Corporation (OAR)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/rtems/dpmem/if/header */

#ifndef _RTEMS_RTEMS_DPMEM_H
#define _RTEMS_RTEMS_DPMEM_H

#include <stdint.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/dpmem/if/group */

/**
 * @defgroup RTEMSAPIClassicDPMem Dual-Ported Memory Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Dual-Ported Memory Manager provides a mechanism for converting
 *   addresses between internal and external representations for multiple
 *   dual-ported memory areas (DPMA).
 */

/* Generated from spec:/rtems/dpmem/if/create */

/**
 * @ingroup RTEMSAPIClassicDPMem
 *
 * @brief Creates a port.
 *
 * @param name is the object name of the port.
 *
 * @param internal_start is the internal start address of the memory area.
 *
 * @param external_start is the external start address of the memory area.
 *
 * @param length is the length in bytes of the memory area.
 *
 * @param[out] id is the pointer to an object identifier variable.  When the
 *   directive call is successful, the identifier of the created port will be
 *   stored in this variable.
 *
 * This directive creates a port which resides on the local node.  The port has
 * the user-defined object name specified in ``name``.  The assigned object
 * identifier is returned in ``id``.  This identifier is used to access the
 * port with other dual-ported memory port related directives.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NAME The ``name`` parameter was invalid.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``internal_start`` parameter was not
 *   properly aligned.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``external_start`` parameter was not
 *   properly aligned.
 *
 * @retval ::RTEMS_TOO_MANY There was no inactive object available to create a
 *   port.  The number of port available to the application is configured
 *   through the #CONFIGURE_MAXIMUM_PORTS application configuration option.
 *
 * @par Notes
 * @parblock
 * The ``internal_start`` and ``external_start`` parameters must be on a
 * boundary defined by the target processor architecture.
 *
 * For control and maintenance of the port, RTEMS allocates a DPCB from the
 * local DPCB free pool and initializes it.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 *
 * * The number of ports available to the application is configured through the
 *   #CONFIGURE_MAXIMUM_PORTS application configuration option.
 *
 * * Where the object class corresponding to the directive is configured to use
 *   unlimited objects, the directive may allocate memory from the RTEMS
 *   Workspace.
 * @endparblock
 */
rtems_status_code rtems_port_create(
  rtems_name name,
  void      *internal_start,
  void      *external_start,
  uint32_t   length,
  rtems_id  *id
);

/* Generated from spec:/rtems/dpmem/if/ident */

/**
 * @ingroup RTEMSAPIClassicDPMem
 *
 * @brief Identifies a port by the object name.
 *
 * @param name is the object name to look up.
 *
 * @param[out] id is the pointer to an object identifier variable.  When the
 *   directive call is successful, the object identifier of an object with the
 *   specified name will be stored in this variable.
 *
 * This directive obtains a port identifier associated with the port name
 * specified in ``name``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_NAME The ``name`` parameter was 0.
 *
 * @retval ::RTEMS_INVALID_NAME There was no object with the specified name on
 *   the local node.
 *
 * @par Notes
 * @parblock
 * If the port name is not unique, then the port identifier will match the
 * first port with that name in the search order.  However, this port
 * identifier is not guaranteed to correspond to the desired port.
 *
 * The objects are searched from lowest to the highest index.  Only the local
 * node is searched.
 *
 * The port identifier is used with other dual-ported memory related directives
 * to access the port.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_port_ident( rtems_name name, rtems_id *id );

/* Generated from spec:/rtems/dpmem/if/delete */

/**
 * @ingroup RTEMSAPIClassicDPMem
 *
 * @brief Deletes the port.
 *
 * @param id is the port identifier.
 *
 * This directive deletes the port specified by ``id``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no port associated with the identifier
 *   specified by ``id``.
 *
 * @par Notes
 * The DPCB for the deleted port is reclaimed by RTEMS.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 *
 * * The calling task does not have to be the task that created the object.
 *   Any local task that knows the object identifier can delete the object.
 *
 * * Where the object class corresponding to the directive is configured to use
 *   unlimited objects, the directive may free memory to the RTEMS Workspace.
 * @endparblock
 */
rtems_status_code rtems_port_delete( rtems_id id );

/* Generated from spec:/rtems/dpmem/if/external-to-internal */

/**
 * @ingroup RTEMSAPIClassicDPMem
 *
 * @brief Converts the external address to the internal address.
 *
 * @param id is the port identifier.
 *
 * @param external is the external address to convert.
 *
 * @param[out] internal is the pointer to a pointer variable.  When the
 *   directive call is successful, the external address associated with the
 *   internal address will be stored in this variable.
 *
 * This directive converts a dual-ported memory address from external to
 * internal representation for the specified port.  If the given external
 * address is invalid for the specified port, then the internal address is set
 * to the given external address.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NAME The ``id`` parameter was invalid.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``internal`` parameter was NULL.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_port_external_to_internal(
  rtems_id id,
  void    *external,
  void   **internal
);

/* Generated from spec:/rtems/dpmem/if/internal-to-external */

/**
 * @ingroup RTEMSAPIClassicDPMem
 *
 * @brief Converts the internal address to the external address.
 *
 * @param id is the port identifier.
 *
 * @param internal is the internal address to convert.
 *
 * @param[out] external is the pointer to a pointer variable.  When the
 *   directive call is successful, the external address associated with the
 *   internal address will be stored in this variable.
 *
 * This directive converts a dual-ported memory address from internal to
 * external representation so that it can be passed to owner of the DPMA
 * represented by the specified port.  If the given internal address is an
 * invalid dual-ported address, then the external address is set to the given
 * internal address.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NAME The ``id`` parameter was invalid.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``external`` parameter was NULL.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_port_internal_to_external(
  rtems_id id,
  void    *internal,
  void   **external
);

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_DPMEM_H */
