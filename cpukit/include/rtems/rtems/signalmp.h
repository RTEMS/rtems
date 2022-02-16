/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicSignalMP
 *
 * @brief This header file provides the implementation interfaces of the
 *   @ref RTEMSImplClassicSignalMP.
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
 * @defgroup RTEMSImplClassicSignalMP \
 *   Signal Manager Multiprocessing (MP) Support
 *
 * @ingroup RTEMSImplClassicSignal
 *
 * @brief This group contains the implementation to support the Signal Manager
 *   in multiprocessing (MP) configurations.
 *
 * @{
 */

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
