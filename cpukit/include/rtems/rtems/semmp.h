/**
 * @file
 *
 * @ingroup ClassicSEM
 *
 * This include file contains all the constants and structures associated
 * with the Multiprocessing Support in the Semaphore Manager.
 */

/* COPYRIGHT (c) 1989-2013.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
 *  @defgroup ClassicSEM Semaphore MP Support
 *
 *  @ingroup ClassicMP
 *
 *  This encapsulates functionality related to the transparent multiprocessing
 *  support within the Classic API Semaphore Manager.
 */
/**@{*/

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

RTEMS_INLINE_ROUTINE bool _Semaphore_MP_Is_remote( Objects_Id id )
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
 *  @brief Semaphore MP Process Packet
 *
 *  This routine performs the actions specific to this package for
 *  the request from another node.
 */
void _Semaphore_MP_Process_packet (
  rtems_packet_prefix *the_packet_prefix
);

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
