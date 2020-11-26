/**
 * @file
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This header file provides parts of the multiprocessing (MP) API.
 */

/* COPYRIGHT (c) 1989-2013.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_MP_H
#define _RTEMS_RTEMS_MP_H

#include <rtems/score/mppkt.h>

/**
 *  @defgroup RTEMSAPIClassicMP Multiprocessing
 *
 *  @ingroup RTEMSAPIClassic
 *
 *  This encapsulates functionality related to the distributed
 *  Multiprocessing support in the Classic API.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief An MPCI must support packets of at least this size.
 */
#define RTEMS_MINIMUM_PACKET_SIZE  MP_PACKET_MINIMUM_PACKET_SIZE

/**
 * @brief Defines the count of @c uint32_t numbers in a packet which must be
 * converted to native format in a heterogeneous system.
 *
 * In packets longer than this value, some of the extra data may be a user
 * message buffer which is not automatically endian swapped.
 */
#define RTEMS_MINIMUN_HETERO_CONVERSION  MP_PACKET_MINIMUN_HETERO_CONVERSION

/**
 *  @brief RTEMS Multiprocessing Announce
 *
 *  This routine implements the MULTIPROCESSING_ANNOUNCE directive.
 *  It is invoked by the MPCI layer to indicate that an MPCI packet
 *  has been received.
 */
void rtems_multiprocessing_announce ( void );

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/*  end of include file */
