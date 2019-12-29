/**
 * @file
 *
 * @brief Message Manager MP Support
 *
 * This include file contains all the constants and structures associated
 * with the Multiprocessing Support in the Message Manager.
 */

/* COPYRIGHT (c) 1989-2013.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_MSGMP_H
#define _RTEMS_RTEMS_MSGMP_H

#ifndef _RTEMS_RTEMS_MESSAGEIMPL_H
# error "Never use <rtems/rtems/msgmp.h> directly; include <rtems/rtems/messageimpl.h> instead."
#endif

#include <rtems/score/mpciimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicMsgMP Message Queue MP Support
 *
 *  @ingroup ClassicMP
 *
 *  This encapsulates functionality related to the transparent multiprocessing
 *  support within the Classic API Message Queue Manager.
 */
/*{*/

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

RTEMS_INLINE_ROUTINE bool _Message_queue_MP_Is_remote( Objects_Id id )
{
  return _Objects_MP_Is_remote( id, &_Message_queue_Information );
}

/**
 *  @brief Message_queue_Core_message_queue_mp_support
 *
 *  Input parameters:
 *    the_thread - the remote thread the message was submitted to
 *    id         - id of the message queue
 *
 *  Output parameters: NONE
 */
void  _Message_queue_Core_message_queue_mp_support (
  Thread_Control *the_thread,
  rtems_id        id
);

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
 * @brief Issues a remote rtems_message_queue_broadcast() request.
 */
rtems_status_code _Message_queue_MP_Broadcast(
  rtems_id    id,
  const void *buffer,
  size_t      size,
  uint32_t   *count
);

/**
 * @brief Issues a remote rtems_message_queue_flush() request.
 */
rtems_status_code _Message_queue_MP_Flush(
  rtems_id  id,
  uint32_t *count
);

/**
 * @brief Issues a remote rtems_message_queue_get_number_pending() request.
 */
rtems_status_code _Message_queue_MP_Get_number_pending(
  rtems_id  id,
  uint32_t *count
);

/**
 * @brief Issues a remote rtems_message_queue_receive() request.
 */
rtems_status_code _Message_queue_MP_Receive(
  rtems_id        id,
  void           *buffer,
  size_t         *size,
  rtems_option    option_set,
  rtems_interval  timeout
);

/**
 * @brief Issues a remote rtems_message_queue_send() request.
 */
rtems_status_code _Message_queue_MP_Send(
  rtems_id    id,
  const void *buffer,
  size_t      size
);

/**
 * @brief Issues a remote rtems_message_queue_urgent() request.
 */
rtems_status_code _Message_queue_MP_Urgent(
  rtems_id    id,
  const void *buffer,
  size_t      size
);

/**
 *  @brief _Message_queue_MP_Send_object_was_deleted
 *
 *  This routine is invoked indirectly by the thread queue
 *  when a proxy has been removed from the thread queue and
 *  the remote node must be informed of this.
 */
void _Message_queue_MP_Send_object_was_deleted (
  Thread_Control *the_proxy,
  Objects_Id      mp_id
);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of file */
