/**
 * @file rtems/rtems/msgmp.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Multiprocessing Support in the Message Manager.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_MSGMP_H
#define _RTEMS_RTEMS_MSGMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/message.h>
#include <rtems/score/mppkt.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>

/**
 *  @defgroup ClassicMsgMP Message Queue MP Support
 *
 *  @ingroup ClassicMP
 *
 *  This encapsulates functionality which XXX
 */
/**{*/

/**
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

/**
 *  The following data structure defines the packet used to perform
 *  remote message queue operations.
 */
typedef struct {
  rtems_packet_prefix                Prefix;
  Message_queue_MP_Remote_operations operation;
  rtems_name                         name;
  rtems_option                       option_set;
  Objects_Id                         proxy_id;
  uint32_t                           count;
  size_t                             size;
  uint32_t                           pad0;
  CORE_message_queue_Buffer          Buffer;
}   Message_queue_MP_Packet;

#define MESSAGE_QUEUE_MP_PACKET_SIZE \
  offsetof(Message_queue_MP_Packet, Buffer.buffer)

/**
 *  @brief _Message_queue_MP_Send_process_packet
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

/**
 *  @brief _Message_queue_MP_Send_request_packet
 *
 *  This routine performs a remote procedure call so that a
 *  directive operation can be initiated on another node.
 */
rtems_status_code _Message_queue_MP_Send_request_packet (
  Message_queue_MP_Remote_operations  operation,
  Objects_Id                          message_queue_id,
  const void                         *buffer,
  size_t                             *size_p,
  rtems_option                        option_set,
  rtems_interval                      timeout
);

/**
 *  @brief _Message_queue_MP_Send_response_packet
 *
 *  This routine performs a remote procedure call so that a
 *  directive can be performed on another node.
 */
void _Message_queue_MP_Send_response_packet (
  Message_queue_MP_Remote_operations  operation,
  Objects_Id                          message_queue_id,
  Thread_Control                     *the_thread
);

/**
 *
 @brief *  _Message_queue_MP_Process_packet
 *
 *  This routine performs the actions specific to this package for
 *  the request from another node.
 */
void _Message_queue_MP_Process_packet (
  rtems_packet_prefix *the_packet_prefix
);

/**
 *  @brief _Message_queue_MP_Send_object_was_deleted
 *
 *  This routine is invoked indirectly by the thread queue
 *  when a proxy has been removed from the thread queue and
 *  the remote node must be informed of this.
 */
void _Message_queue_MP_Send_object_was_deleted (
  Thread_Control *the_proxy
);

/**
 *  @brief _Message_queue_MP_Send_extract_proxy
 *
 *  This routine is invoked when a task is deleted and it
 *  has a proxy which must be removed from a thread queue and
 *  the remote node must be informed of this.
 */
void _Message_queue_MP_Send_extract_proxy (
  void           *argument
);

/**
 *  @brief _Message_queue_MP_Get_packet
 *
 *  This function is used to obtain a message queue mp packet.
 */
Message_queue_MP_Packet *_Message_queue_MP_Get_packet ( void );

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of file */
