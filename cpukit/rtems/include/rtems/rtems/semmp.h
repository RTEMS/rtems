/*  semmp.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Multiprocessing Support in the Semaphore Manager.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __RTEMS_SEMAPHORE_MP_h
#define __RTEMS_SEMAPHORE_MP_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/mppkt.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/sem.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>

/*
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

/*
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

/*
 *  _Semaphore_MP_Send_process_packet
 *
 *  DESCRIPTION:
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

/*
 *  _Semaphore_MP_Send_request_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  directive operation can be initiated on another node.
 */

rtems_status_code _Semaphore_MP_Send_request_packet (
  Semaphore_MP_Remote_operations operation,
  Objects_Id                     semaphore_id,
  rtems_option                   option_set,
  rtems_interval                 timeout
);

/*
 *  _Semaphore_MP_Send_response_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  directive can be performed on another node.
 */

void _Semaphore_MP_Send_response_packet (
  Semaphore_MP_Remote_operations  operation,
  Objects_Id                      semaphore_id,
  Thread_Control                 *the_thread
);

/*
 *
 *  _Semaphore_MP_Process_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs the actions specific to this package for
 *  the request from another node.
 */

void _Semaphore_MP_Process_packet (
  rtems_packet_prefix *the_packet_prefix
);

/*
 *  _Semaphore_MP_Send_object_was_deleted
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked indirectly by the thread queue
 *  when a proxy has been removed from the thread queue and
 *  the remote node must be informed of this.
 */

void _Semaphore_MP_Send_object_was_deleted (
  Thread_Control *the_proxy
);

/*
 *  _Semaphore_MP_Send_extract_proxy
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked when a task is deleted and it
 *  has a proxy which must be removed from a thread queue and
 *  the remote node must be informed of this.
 */

void _Semaphore_MP_Send_extract_proxy (
  void           *argument
);

/*
 *  _Semaphore_MP_Get_packet
 *
 *  DESCRIPTION:
 *
 *  This function is used to obtain a semaphore mp packet.
 */

Semaphore_MP_Packet *_Semaphore_MP_Get_packet ( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of file */
