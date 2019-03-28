/**
 * @file
 *
 * @ingroup ClassicMP
 *
 * This include file contains all the constants and structures associated
 * with the Multiprocessing Manager.
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

/**
 *  @defgroup ClassicMP Multiprocessing
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
