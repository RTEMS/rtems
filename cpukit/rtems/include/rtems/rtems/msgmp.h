/*  msgmp.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Multiprocessing Support in the Message Manager.
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

#ifndef __RTEMS_MESSAGE_QUEUE_MP_h
#define __RTEMS_MESSAGE_QUEUE_MP_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/message.h>
#include <rtems/score/mppkt.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>

/*
 *  The following enumerated type defines the list of
 *  remote message queue operations.
 */

typedef enum {
  MESSAGE_QUEUE_MP_ANNOUNCE_CREATE             =  0,
  MESSAGE_QUEUE_MP_ANNOUNCE_DELETE             =  1,
  MESSAGE_QUEUE_MP_EXTRACT_PROXY               =  2,
  MESSAGE_QUEUE_MP_RECEIVE_REQUEST             =  3,
  MESSAGE_QUEUE_MP_RECEIVE_RESPONSE            =  4,
  MESSAGE_QUEUE_MP_SEND_REQUEST                =  5,
  MESSAGE_QUEUE_MP_SEND_RESPONSE               =  6,
  MESSAGE_QUEUE_MP_URGENT_REQUEST              =  7,
  MESSAGE_QUEUE_MP_URGENT_RESPONSE             =  8,
  MESSAGE_QUEUE_MP_BROADCAST_REQUEST           =  9,
  MESSAGE_QUEUE_MP_BROADCAST_RESPONSE          = 10,
  MESSAGE_QUEUE_MP_FLUSH_REQUEST               = 11,
  MESSAGE_QUEUE_MP_FLUSH_RESPONSE              = 12,
  MESSAGE_QUEUE_MP_GET_NUMBER_PENDING_REQUEST  = 13,
  MESSAGE_QUEUE_MP_GET_NUMBER_PENDING_RESPONSE = 14
}   Message_queue_MP_Remote_operations;

/*
 *  The following data structure defines the packet used to perform
 *  remote message queue operations.
 */

typedef struct {
  rtems_packet_prefix                Prefix;
  Message_queue_MP_Remote_operations operation;
  rtems_name                         name;
  rtems_option                       option_set;
  Objects_Id                         proxy_id;
  unsigned32                         count;
  unsigned32                         size;
  unsigned32                         pad0;
  CORE_message_queue_Buffer          Buffer;
}   Message_queue_MP_Packet;

/*
 *  _Message_queue_MP_Send_process_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  process operation can be performed on another node.
 */

void _Message_queue_MP_Send_process_packet (
  Message_queue_MP_Remote_operations operation,
  Objects_Id                         message_queue_id,
  rtems_name                         name,
  Objects_Id                         proxy_id
);

/*
 *  _Message_queue_MP_Send_request_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  directive operation can be initiated on another node.
 */

rtems_status_code _Message_queue_MP_Send_request_packet (
  Message_queue_MP_Remote_operations  operation,
  Objects_Id                          message_queue_id,
  void                               *buffer,
  unsigned32                         *size_p,
  rtems_option                        option_set,
  Watchdog_Interval                   timeout
);

/*
 *  _Message_queue_MP_Send_response_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  directive can be performed on another node.
 */

void _Message_queue_MP_Send_response_packet (
  Message_queue_MP_Remote_operations  operation,
  Objects_Id                          message_queue_id,
  Thread_Control                     *the_thread
);

/*
 *
 *  _Message_queue_MP_Process_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs the actions specific to this package for
 *  the request from another node.
 */

void _Message_queue_MP_Process_packet (
  rtems_packet_prefix *the_packet_prefix
);

/*
 *  _Message_queue_MP_Send_object_was_deleted
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked indirectly by the thread queue
 *  when a proxy has been removed from the thread queue and
 *  the remote node must be informed of this.
 */

void _Message_queue_MP_Send_object_was_deleted (
  Thread_Control *the_proxy
);

/*
 *  _Message_queue_MP_Send_extract_proxy
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked when a task is deleted and it
 *  has a proxy which must be removed from a thread queue and
 *  the remote node must be informed of this.
 */

void _Message_queue_MP_Send_extract_proxy (
  void           *argument
);

/*
 *  _Message_queue_MP_Get_packet
 *
 *  DESCRIPTION:
 *
 *  This function is used to obtain a message queue mp packet.
 */

Message_queue_MP_Packet *_Message_queue_MP_Get_packet ( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of file */
