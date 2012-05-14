/**
 * @file rtems/rtems/barriermp.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Multiprocessing Support in the Barrier Manager.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_SEMMP_H
#define _RTEMS_RTEMS_SEMMP_H

/**
 *  @defgroup ClassicBarrierMP Barrier MP Support
 *
 *  @ingroup ClassicMP
 *
 *  This encapsulates functionality which XXX
 */
/**{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/mppkt.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/barrier.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>

/**
 *  The following enumerated type defines the list of
 *  remote barrier operations.
 */
typedef enum {
  BARRIER_MP_ANNOUNCE_CREATE  =  0,
  BARRIER_MP_ANNOUNCE_DELETE  =  1,
  BARRIER_MP_EXTRACT_PROXY    =  2,
  BARRIER_MP_WAIT_REQUEST     =  3,
  BARRIER_MP_WAIT_RESPONSE    =  4,
  BARRIER_MP_RELEASE_REQUEST  =  5,
  BARRIER_MP_RELEASE_RESPONSE =  6
}   Barrier_MP_Remote_operations;

/**
 *  The following data structure defines the packet used to perform
 *  remote barrier operations.
 */
typedef struct {
  rtems_packet_prefix           Prefix;
  Barrier_MP_Remote_operations  operation;
  rtems_name                    name;
  rtems_option                  option_set;
  Objects_Id                    proxy_id;
}   Barrier_MP_Packet;

/**
 *  @brief _Barrier_MP_Send_process_packet
 *
 *  This routine performs a remote procedure call so that a
 *  process operation can be performed on another node.
 */
void _Barrier_MP_Send_process_packet (
  Barrier_MP_Remote_operations operation,
  Objects_Id                   barrier_id,
  rtems_name                   name,
  Objects_Id                   proxy_id
);

/**
 *  @brief _Barrier_MP_Send_request_packet
 *
 *  This routine performs a remote procedure call so that a
 *  directive operation can be initiated on another node.
 */
rtems_status_code _Barrier_MP_Send_request_packet (
  Barrier_MP_Remote_operations operation,
  Objects_Id                   barrier_id,
  rtems_interval               timeout
);

/**
 *  @brief _Barrier_MP_Send_response_packet
 *
 *  This routine performs a remote procedure call so that a
 *  directive can be performed on another node.
 */

void _Barrier_MP_Send_response_packet (
  Barrier_MP_Remote_operations  operation,
  Objects_Id                    barrier_id,
  Thread_Control               *the_thread
);

/**
 *  @brief _Barrier_MP_Process_packet
 *
 *  This routine performs the actions specific to this package for
 *  the request from another node.
 */
void _Barrier_MP_Process_packet (
  rtems_packet_prefix *the_packet_prefix
);

/**
 *  @brief _Barrier_MP_Send_object_was_deleted
 *
 *  This routine is invoked indirectly by the thread queue
 *  when a proxy has been removed from the thread queue and
 *  the remote node must be informed of this.
 */
void _Barrier_MP_Send_object_was_deleted (
  Thread_Control *the_proxy
);

/**
 *  @brief _Barrier_MP_Send_extract_proxy
 *
 *  This routine is invoked when a task is deleted and it
 *  has a proxy which must be removed from a thread queue and
 *  the remote node must be informed of this.
 */
void _Barrier_MP_Send_extract_proxy (
  void           *argument
);

/**
 *  @brief _Barrier_MP_Get_packet
 *
 *  This function is used to obtain a barrier mp packet.
 */
Barrier_MP_Packet *_Barrier_MP_Get_packet ( void );

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of file */
