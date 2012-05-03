/**
 * @file rtems/rtems/eventmp.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Multiprocessing Support in the Event Manager.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_EVENTMP_H
#define _RTEMS_RTEMS_EVENTMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/event.h>
#include <rtems/score/mppkt.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>

/**
 *  @defgroup ClassicEventMP Event MP Support
 *
 *  @ingroup ClassicMP
 *
 *  This encapsulates functionality which XXX
 */
/**@{*/

/**
 *  The following enumerated type defines the list of
 *  remote event operations.
 */
typedef enum {
  EVENT_MP_SEND_REQUEST  =  0,
  EVENT_MP_SEND_RESPONSE =  1
}   Event_MP_Remote_operations;

/**
 *  The following data structure defines the packet used to perform
 *  remote event operations.
 */
typedef struct {
  rtems_packet_prefix         Prefix;
  Event_MP_Remote_operations  operation;
  rtems_event_set             event_in;
}   Event_MP_Packet;

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
 *  @brief Event_MP_Send_request_packet
 *
 *  This routine performs a remote procedure call so that a
 *  directive operation can be initiated on another node.
 */
rtems_status_code _Event_MP_Send_request_packet (
  Event_MP_Remote_operations operation,
  Objects_Id                 event_id,
  rtems_event_set         event_in
);

/**
 *  @brief Event_MP_Send_response_packet
 *
 *  This routine performs a remote procedure call so that a
 *  directive can be performed on another node.
 */
void _Event_MP_Send_response_packet (
  Event_MP_Remote_operations  operation,
  Thread_Control             *the_thread
);

/**
 *  @brief Event_MP_Process_packet
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

/**
 *  @brief Event_MP_Get_packet
 *
 *  This function is used to obtain a event mp packet.
 */
Event_MP_Packet *_Event_MP_Get_packet ( void );

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of file */
