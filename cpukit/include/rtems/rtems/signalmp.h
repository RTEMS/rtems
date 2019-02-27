/**
 * @file
 *
 * @brief Signal MP Support
 *
 * This include file contains all the constants and structures associated
 * with the Multiprocessing Support in the Signal Manager.
 */

/* COPYRIGHT (c) 1989-2013.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_SIGNALMP_H
#define _RTEMS_RTEMS_SIGNALMP_H

#ifndef _RTEMS_RTEMS_SIGNALIMPL_H
# error "Never use <rtems/rtems/signalmp.h> directly; include <rtems/rtems/signalimpl.h> instead."
#endif

#include <rtems/score/mpciimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicSignalMP Signal MP Support
 *
 *  @ingroup ClassicMP
 *
 *  This encapsulates functionality related to the transparent multiprocessing
 *  support within the Classic API Signal Manager.
 */
/*{*/

/*
 *  @brief Signal_MP_Send_process_packet
 *
 *  This routine performs a remote procedure call so that a
 *  process operation can be performed on another node.
 *
 *  This routine is not needed since there are no process
 *  packets to be sent by this manager.
 */

/**
 * @brief Issues a remote rtems_signal_send() request.
 */
rtems_status_code _Signal_MP_Send(
  rtems_id         id,
  rtems_signal_set signal_set
);

/**
 *  @brief Signal MP Process Packet
 *
 *  This routine performs the actions specific to this package for
 *  the request from another node.
 */
void _Signal_MP_Process_packet (
  rtems_packet_prefix *the_packet_prefix
);

/*
 *  @brief Signal_MP_Send_object_was_deleted
 *
 *  This routine is invoked indirectly by the thread queue
 *  when a proxy has been removed from the thread queue and
 *  the remote node must be informed of this.
 *
 *  This routine is not needed since there are no objects
 *  deleted by this manager.
 */

/*
 *  @brief Signal_MP_Send_extract_proxy
 *
 *  This routine is invoked when a task is deleted and it
 *  has a proxy which must be removed from a thread queue and
 *  the remote node must be informed of this.
 *
 *  This routine is not needed since there are no objects
 *  deleted by this manager.
 */

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of file */
