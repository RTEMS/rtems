/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicEventMP
 *
 * @brief This header file provides the implementation interfaces of the
 *   @ref RTEMSImplClassicEventMP.
 */

/* COPYRIGHT (c) 1989-2013.
 * On-Line Applications Research Corporation (OAR).
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
 * @defgroup RTEMSImplClassicEventMP Event Manager Multiprocessing (MP) Support
 *
 * @ingroup RTEMSImplClassicEvent
 *
 * @brief This group contains the implementation to support the Event Manager
 *   in multiprocessing (MP) configurations.
 *
 * This encapsulates functionality related to the transparent multiprocessing
 * support within the Classic API Event Manager.
 *
 * @{
 */

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
