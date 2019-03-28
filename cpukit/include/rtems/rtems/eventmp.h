/**
 * @file
 *
 * @ingroup ClassicEventMP
 *
 * This include file contains all the constants and structures associated
 * with the Multiprocessing Support in the Event Manager.
 */

/* COPYRIGHT (c) 1989-2013.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_EVENTMP_H
#define _RTEMS_RTEMS_EVENTMP_H

#ifndef _RTEMS_RTEMS_EVENTIMPL_H
# error "Never use <rtems/rtems/eventmp.h> directly; include <rtems/rtems/eventimpl.h> instead."
#endif

#include <rtems/score/mpciimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicEventMP Event MP Support
 *
 *  @ingroup ClassicMP
 *
 *  This encapsulates functionality related to the transparent multiprocessing
 *  support within the Classic API Event Manager.
 */
/**@{*/

/*
 *  @brief Event_MP_Send_process_packet
 *
 *  This routine performs a remote procedure call so that a
 *  process operation can be performed on another node.
 *
 *  @note This routine is not needed since there are no process
 *  packets to be sent by this manager.
 */

/**
 * @brief Issues a remote rtems_event_send() request.
 */
rtems_status_code _Event_MP_Send(
  rtems_id        id,
  rtems_event_set event_in
);

/**
 *  @brief Event MP Packet Process
 *
 *  This routine performs the actions specific to this package for
 *  the request from another node.
 */
void _Event_MP_Process_packet (
  rtems_packet_prefix *the_packet_prefix
);

/*
 *  @brief Event_MP_Send_object_was_deleted
 *
 *  This routine is invoked indirectly by the thread queue
 *  when a proxy has been removed from the thread queue and
 *  the remote node must be informed of this.
 *
 *  This routine is not needed since there are no objects
 *  deleted by this manager.
 */

/*
 *  @brief Event_MP_Send_extract_proxy
 *
 *  This routine is invoked when a task is deleted and it
 *  has a proxy which must be removed from a thread queue and
 *  the remote node must be informed of this.
 *
 *  This routine is not needed since there are no objects
 *  deleted by this manager.
 */

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of file */
