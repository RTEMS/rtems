/**
 * @file rtems/rtems/mp.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Multiprocessing Manager.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_MP_H
#define _RTEMS_RTEMS_MP_H

/**
 *  @defgroup ClassicMP Multiprocessing
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality which XXX
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief Multiprocessing_Manager_initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _Multiprocessing_Manager_initialization ( void );

/**
 *  @brief rtems_multiprocessing_announce
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
