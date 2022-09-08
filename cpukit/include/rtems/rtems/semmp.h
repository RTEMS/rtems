/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicSemaphoreMP
 *
 * @brief This header file provides the implementation interfaces of the
 *   @ref RTEMSImplClassicSemaphoreMP.
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

#ifndef _RTEMS_RTEMS_SEMMP_H
#define _RTEMS_RTEMS_SEMMP_H

#ifndef _RTEMS_RTEMS_SEMIMPL_H
# error "Never use <rtems/rtems/semmp.h> directly; include <rtems/rtems/semimpl.h> instead."
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplClassicSemaphoreMP \
 *   Semaphore Manager Multiprocessing (MP) Support
 *
 * @ingroup RTEMSImplClassicSemaphore
 *
 * @brief This group contains the implementation to support the Semaphore Manager
 *   in multiprocessing (MP) configurations.
 *
 * @{
 */

/**
 *  The following enumerated type defines the list of
 *  remote semaphore operations.
 */
typedef enum {
  SEMAPHORE_MP_ANNOUNCE_CREATE  =  0,
  SEMAPHORE_MP_ANNOUNCE_DELETE  =  1,
  SEMAPHORE_MP_EXTRACT_PROXY    =  2,
  SEMAPHORE_MP_OBTAIN_REQUEST   =  3,
  SEMAPHORE_MP_OBTAIN_RESPONSE  =  4,
  SEMAPHORE_MP_RELEASE_REQUEST  =  5,
  SEMAPHORE_MP_RELEASE_RESPONSE =  6
}   Semaphore_MP_Remote_operations;

/**
 *  The following data structure defines the packet used to perform
 *  remote semaphore operations.
 */
typedef struct {
  rtems_packet_prefix             Prefix;
  Semaphore_MP_Remote_operations  operation;
  rtems_name                      name;
  rtems_option                    option_set;
  Objects_Id                      proxy_id;
}   Semaphore_MP_Packet;

static inline bool _Semaphore_MP_Is_remote( Objects_Id id )
{
  return _Objects_MP_Is_remote( id, &_Semaphore_Information );
}

/**
 *  @brief Semaphore MP Send Process Packet
 *
 *  This routine performs a remote procedure call so that a
 *  process operation can be performed on another node.
 */
void _Semaphore_MP_Send_process_packet (
  Semaphore_MP_Remote_operations operation,
  Objects_Id                     semaphore_id,
  rtems_name                     name,
  Objects_Id                     proxy_id
);

/**
 * @brief Issues a remote rtems_semaphore_obtain() request.
 */
rtems_status_code _Semaphore_MP_Obtain(
  rtems_id        id,
  rtems_option    option_set,
  rtems_interval  timeout
);

/**
 * @brief Issues a remote rtems_semaphore_release() request.
 */
rtems_status_code _Semaphore_MP_Release( rtems_id id );

/**
 *  @brief Semaphore MP Send Object was Deleted
 *
 *  This routine is invoked indirectly by the thread queue
 *  when a proxy has been removed from the thread queue and
 *  the remote node must be informed of this.
 */
void _Semaphore_MP_Send_object_was_deleted (
  Thread_Control *the_proxy,
  Objects_Id      mp_id
);

/**
 * @brief Semaphore Core Mutex MP Support
 *
 * This function processes the global actions necessary for remote
 * accesses to a global semaphore based on a core mutex. This function
 * is called by the core.
 *
 * @param[in] the_thread the remote thread the semaphore was surrendered to
 * @param[in] id is the id of the surrendered semaphore
 */
void  _Semaphore_Core_mutex_mp_support (
  Thread_Control *the_thread,
  Objects_Id      id
);

/**
 * @brief Semaphore Core MP Support
 *
 * This function processes the global actions necessary for remote
 * accesses to a global semaphore based on a core semaphore. This function
 * is called by the core.
 *
 * @param[in] the_thread the remote thread the semaphore was surrendered to
 * @param[in] id is the id of the surrendered semaphore
 */
void  _Semaphore_Core_semaphore_mp_support (
  Thread_Control *the_thread,
  Objects_Id      id
);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of file */
