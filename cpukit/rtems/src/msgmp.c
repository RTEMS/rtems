/*
 *  Multiprocessing Support for the Message Queue Manager
 *
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/message.h>
#include <rtems/score/mpci.h>
#include <rtems/rtems/msgmp.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>
#include <rtems/rtems/support.h>

RTEMS_STATIC_ASSERT(
  MESSAGE_QUEUE_MP_PACKET_SIZE <= MP_PACKET_MINIMUM_PACKET_SIZE,
  Message_queue_MP_Packet
);

/*
 *  _Message_queue_MP_Send_process_packet
 *
 */

void _Message_queue_MP_Send_process_packet (
  Message_queue_MP_Remote_operations  operation,
  Objects_Id                          message_queue_id,
  rtems_name                          name,
  Objects_Id                          proxy_id
)
{
  Message_queue_MP_Packet *the_packet;
  uint32_t                 node;

  switch ( operation ) {

    case MESSAGE_QUEUE_MP_ANNOUNCE_CREATE:
    case MESSAGE_QUEUE_MP_ANNOUNCE_DELETE:
    case MESSAGE_QUEUE_MP_EXTRACT_PROXY:

      the_packet                    = _Message_queue_MP_Get_packet();
      the_packet->Prefix.the_class  = MP_PACKET_MESSAGE_QUEUE;
      the_packet->Prefix.length     = sizeof ( Message_queue_MP_Packet );
      the_packet->Prefix.to_convert = sizeof ( Message_queue_MP_Packet );
      the_packet->operation         = operation;
      the_packet->Prefix.id         = message_queue_id;
      the_packet->name              = name;
      the_packet->proxy_id          = proxy_id;

      if ( operation == MESSAGE_QUEUE_MP_EXTRACT_PROXY )
         node = _Objects_Get_node( message_queue_id );
      else
         node = MPCI_ALL_NODES;

      _MPCI_Send_process_packet( node, &the_packet->Prefix );
      break;

    case MESSAGE_QUEUE_MP_RECEIVE_REQUEST:
    case MESSAGE_QUEUE_MP_RECEIVE_RESPONSE:
    case MESSAGE_QUEUE_MP_SEND_REQUEST:
    case MESSAGE_QUEUE_MP_SEND_RESPONSE:
    case MESSAGE_QUEUE_MP_URGENT_REQUEST:
    case MESSAGE_QUEUE_MP_URGENT_RESPONSE:
    case MESSAGE_QUEUE_MP_BROADCAST_REQUEST:
    case MESSAGE_QUEUE_MP_BROADCAST_RESPONSE:
    case MESSAGE_QUEUE_MP_FLUSH_REQUEST:
    case MESSAGE_QUEUE_MP_FLUSH_RESPONSE:
    case MESSAGE_QUEUE_MP_GET_NUMBER_PENDING_REQUEST:
    case MESSAGE_QUEUE_MP_GET_NUMBER_PENDING_RESPONSE:
      break;

  }
}

/*
 *  _Message_queue_MP_Send_request_packet
 *
 */

rtems_status_code _Message_queue_MP_Send_request_packet (
  Message_queue_MP_Remote_operations  operation,
  Objects_Id                          message_queue_id,
  const void                         *buffer,
  size_t                             *size_p,
  rtems_option                        option_set,
  rtems_interval                      timeout
)
{
  Message_queue_MP_Packet *the_packet;

  switch ( operation ) {

    case MESSAGE_QUEUE_MP_SEND_REQUEST:
    case MESSAGE_QUEUE_MP_URGENT_REQUEST:
    case MESSAGE_QUEUE_MP_BROADCAST_REQUEST:
    case MESSAGE_QUEUE_MP_FLUSH_REQUEST:
    case MESSAGE_QUEUE_MP_GET_NUMBER_PENDING_REQUEST:

      the_packet                    = _Message_queue_MP_Get_packet();
      the_packet->Prefix.the_class  = MP_PACKET_MESSAGE_QUEUE;
      the_packet->Prefix.length     = MESSAGE_QUEUE_MP_PACKET_SIZE;
      if ( size_p )
        the_packet->Prefix.length     += *size_p;
      the_packet->Prefix.to_convert = MESSAGE_QUEUE_MP_PACKET_SIZE;

      /*
       * make sure message is not too big for our MPCI driver
       * We have to check it here instead of waiting for MPCI because
       * we are about to slam in the payload
       */

      if (the_packet->Prefix.length > _MPCI_table->maximum_packet_size) {
          return RTEMS_INVALID_SIZE;
      }

      if (! _Options_Is_no_wait(option_set))
          the_packet->Prefix.timeout = timeout;

      the_packet->operation  = operation;
      the_packet->Prefix.id  = message_queue_id;
      the_packet->option_set = option_set;

      /*
       * Copy the data into place if needed
       */

      if (buffer) {
          the_packet->Buffer.size = *size_p;
          _CORE_message_queue_Copy_buffer(
            buffer,
            the_packet->Buffer.buffer,
            *size_p
          );
      }

      return (rtems_status_code) _MPCI_Send_request_packet(
        _Objects_Get_node(message_queue_id),
        &the_packet->Prefix,
        STATES_WAITING_FOR_MESSAGE
      );
      break;

    case MESSAGE_QUEUE_MP_RECEIVE_REQUEST:

      the_packet                    = _Message_queue_MP_Get_packet();
      the_packet->Prefix.the_class  = MP_PACKET_MESSAGE_QUEUE;
      the_packet->Prefix.length     = MESSAGE_QUEUE_MP_PACKET_SIZE;
      the_packet->Prefix.to_convert = MESSAGE_QUEUE_MP_PACKET_SIZE;

      if (! _Options_Is_no_wait(option_set))
          the_packet->Prefix.timeout = timeout;

      the_packet->operation  = MESSAGE_QUEUE_MP_RECEIVE_REQUEST;
      the_packet->Prefix.id  = message_queue_id;
      the_packet->option_set = option_set;
      the_packet->size       = 0;        /* just in case of an error */

      _Thread_Executing->Wait.return_argument_second.immutable_object = buffer;
      _Thread_Executing->Wait.return_argument = size_p;

      return (rtems_status_code) _MPCI_Send_request_packet(
        _Objects_Get_node(message_queue_id),
        &the_packet->Prefix,
        STATES_WAITING_FOR_MESSAGE
      );
      break;

    case MESSAGE_QUEUE_MP_ANNOUNCE_CREATE:
    case MESSAGE_QUEUE_MP_ANNOUNCE_DELETE:
    case MESSAGE_QUEUE_MP_EXTRACT_PROXY:
    case MESSAGE_QUEUE_MP_RECEIVE_RESPONSE:
    case MESSAGE_QUEUE_MP_SEND_RESPONSE:
    case MESSAGE_QUEUE_MP_URGENT_RESPONSE:
    case MESSAGE_QUEUE_MP_BROADCAST_RESPONSE:
    case MESSAGE_QUEUE_MP_FLUSH_RESPONSE:
    case MESSAGE_QUEUE_MP_GET_NUMBER_PENDING_RESPONSE:
      break;
  }

  return RTEMS_SUCCESSFUL;
}

/*
 *  _Message_queue_MP_Send_response_packet
 *
 */

void _Message_queue_MP_Send_response_packet (
  Message_queue_MP_Remote_operations  operation,
  Objects_Id                          message_queue_id,
  Thread_Control                     *the_thread
)
{
  Message_queue_MP_Packet *the_packet;

  switch ( operation ) {

    case MESSAGE_QUEUE_MP_RECEIVE_RESPONSE:
    case MESSAGE_QUEUE_MP_SEND_RESPONSE:
    case MESSAGE_QUEUE_MP_URGENT_RESPONSE:
    case MESSAGE_QUEUE_MP_BROADCAST_RESPONSE:
    case MESSAGE_QUEUE_MP_FLUSH_RESPONSE:
    case MESSAGE_QUEUE_MP_GET_NUMBER_PENDING_RESPONSE:

      the_packet = ( Message_queue_MP_Packet *) the_thread->receive_packet;

/*
 *  The packet being returned already contains the class, length, and
 *  to_convert fields, therefore they are not set in this routine.
 *
 *  Exception: MESSAGE_QUEUE_MP_RECEIVE_RESPONSE needs payload length
 *             added to 'length'
 */
      the_packet->operation = operation;
      the_packet->Prefix.id = the_packet->Prefix.source_tid;

      if (operation == MESSAGE_QUEUE_MP_RECEIVE_RESPONSE)
          the_packet->Prefix.length += the_packet->size;

      _MPCI_Send_response_packet(
        _Objects_Get_node( the_packet->Prefix.source_tid ),
        &the_packet->Prefix
      );
      break;

    case MESSAGE_QUEUE_MP_ANNOUNCE_CREATE:
    case MESSAGE_QUEUE_MP_ANNOUNCE_DELETE:
    case MESSAGE_QUEUE_MP_EXTRACT_PROXY:
    case MESSAGE_QUEUE_MP_RECEIVE_REQUEST:
    case MESSAGE_QUEUE_MP_SEND_REQUEST:
    case MESSAGE_QUEUE_MP_URGENT_REQUEST:
    case MESSAGE_QUEUE_MP_BROADCAST_REQUEST:
    case MESSAGE_QUEUE_MP_FLUSH_REQUEST:
    case MESSAGE_QUEUE_MP_GET_NUMBER_PENDING_REQUEST:
      break;

  }
}

/*
 *
 *  _Message_queue_MP_Process_packet
 *
 */

void _Message_queue_MP_Process_packet (
  rtems_packet_prefix   *the_packet_prefix
)
{
  Message_queue_MP_Packet *the_packet;
  Thread_Control          *the_thread;
  bool                     ignored;

  the_packet = (Message_queue_MP_Packet *) the_packet_prefix;

  switch ( the_packet->operation ) {

    case MESSAGE_QUEUE_MP_ANNOUNCE_CREATE:

      ignored = _Objects_MP_Allocate_and_open(
                  &_Message_queue_Information,
                  the_packet->name,
                  the_packet->Prefix.id,
                  true
                );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case MESSAGE_QUEUE_MP_ANNOUNCE_DELETE:

      _Objects_MP_Close( &_Message_queue_Information, the_packet->Prefix.id );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case MESSAGE_QUEUE_MP_EXTRACT_PROXY:

      the_thread = _Thread_MP_Find_proxy( the_packet->proxy_id );

      if (! _Thread_Is_null( the_thread ) )
         _Thread_queue_Extract( the_thread->Wait.queue, the_thread );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case MESSAGE_QUEUE_MP_RECEIVE_REQUEST:

      the_packet->Prefix.return_code = rtems_message_queue_receive(
        the_packet->Prefix.id,
        the_packet->Buffer.buffer,
        &the_packet->size,
        the_packet->option_set,
        the_packet->Prefix.timeout
      );

      if ( the_packet->Prefix.return_code != RTEMS_PROXY_BLOCKING )
        _Message_queue_MP_Send_response_packet(
          MESSAGE_QUEUE_MP_RECEIVE_RESPONSE,
          the_packet->Prefix.id,
          _Thread_Executing
        );
      break;

    case MESSAGE_QUEUE_MP_RECEIVE_RESPONSE:

      the_thread = _MPCI_Process_response( the_packet_prefix );

      if (the_packet->Prefix.return_code == RTEMS_SUCCESSFUL) {
        *(size_t *) the_thread->Wait.return_argument =
           the_packet->size;

        _CORE_message_queue_Copy_buffer(
          the_packet->Buffer.buffer,
          the_thread->Wait.return_argument_second.mutable_object,
          the_packet->size
        );
      }

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case MESSAGE_QUEUE_MP_SEND_REQUEST:

      the_packet->Prefix.return_code = rtems_message_queue_send(
        the_packet->Prefix.id,
        the_packet->Buffer.buffer,
        the_packet->Buffer.size
      );

      _Message_queue_MP_Send_response_packet(
        MESSAGE_QUEUE_MP_SEND_RESPONSE,
        the_packet->Prefix.id,
        _Thread_Executing
      );
      break;

    case MESSAGE_QUEUE_MP_SEND_RESPONSE:
    case MESSAGE_QUEUE_MP_URGENT_RESPONSE:

      the_thread = _MPCI_Process_response( the_packet_prefix );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case MESSAGE_QUEUE_MP_URGENT_REQUEST:

      the_packet->Prefix.return_code = rtems_message_queue_urgent(
        the_packet->Prefix.id,
        the_packet->Buffer.buffer,
        the_packet->Buffer.size
      );

      _Message_queue_MP_Send_response_packet(
        MESSAGE_QUEUE_MP_URGENT_RESPONSE,
        the_packet->Prefix.id,
        _Thread_Executing
      );
      break;

    case MESSAGE_QUEUE_MP_BROADCAST_REQUEST:

      the_packet->Prefix.return_code = rtems_message_queue_broadcast(
        the_packet->Prefix.id,
        the_packet->Buffer.buffer,
        the_packet->Buffer.size,
        &the_packet->count
      );

      _Message_queue_MP_Send_response_packet(
        MESSAGE_QUEUE_MP_BROADCAST_RESPONSE,
        the_packet->Prefix.id,
        _Thread_Executing
      );
      break;

    case MESSAGE_QUEUE_MP_BROADCAST_RESPONSE:
    case MESSAGE_QUEUE_MP_FLUSH_RESPONSE:
    case MESSAGE_QUEUE_MP_GET_NUMBER_PENDING_RESPONSE:

      the_thread = _MPCI_Process_response( the_packet_prefix );

      *(uint32_t *) the_thread->Wait.return_argument = the_packet->count;

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case MESSAGE_QUEUE_MP_FLUSH_REQUEST:

      the_packet->Prefix.return_code = rtems_message_queue_flush(
        the_packet->Prefix.id,
        &the_packet->count
      );

      _Message_queue_MP_Send_response_packet(
        MESSAGE_QUEUE_MP_FLUSH_RESPONSE,
        the_packet->Prefix.id,
        _Thread_Executing
      );
      break;

    case MESSAGE_QUEUE_MP_GET_NUMBER_PENDING_REQUEST:

      the_packet->Prefix.return_code = rtems_message_queue_get_number_pending(
        the_packet->Prefix.id,
        &the_packet->count
      );

      _Message_queue_MP_Send_response_packet(
        MESSAGE_QUEUE_MP_GET_NUMBER_PENDING_RESPONSE,
        the_packet->Prefix.id,
        _Thread_Executing
      );
      break;

  }
}

/*
 *  _Message_queue_MP_Send_object_was_deleted
 *
 */

void _Message_queue_MP_Send_object_was_deleted (
  Thread_Control  *the_proxy
)
{
  the_proxy->receive_packet->return_code = RTEMS_OBJECT_WAS_DELETED;

  _Message_queue_MP_Send_response_packet(
    MESSAGE_QUEUE_MP_RECEIVE_RESPONSE,
    the_proxy->Wait.id,
    the_proxy
  );
}

/*
 *  _Message_queue_MP_Send_extract_proxy
 *
 */

void _Message_queue_MP_Send_extract_proxy (
  void           *argument
)
{
  Thread_Control *the_thread = (Thread_Control *)argument;

  _Message_queue_MP_Send_process_packet(
    MESSAGE_QUEUE_MP_EXTRACT_PROXY,
    the_thread->Wait.id,
    (rtems_name) 0,
    the_thread->Object.id
  );
}

/*
 *  _Message_queue_MP_Get_packet
 *
 */

Message_queue_MP_Packet *_Message_queue_MP_Get_packet ( void )
{
  return ( (Message_queue_MP_Packet *) _MPCI_Get_packet() );
}


/*
 *  _Message_queue_Core_message_queue_mp_support
 *
 *  Input parameters:
 *    the_thread - the remote thread the message was submitted to
 *    id         - id of the message queue
 *
 *  Output parameters: NONE
 */

void  _Message_queue_Core_message_queue_mp_support (
  Thread_Control *the_thread,
  Objects_Id      id
)
{
  the_thread->receive_packet->return_code = RTEMS_SUCCESSFUL;

  _Message_queue_MP_Send_response_packet(
    MESSAGE_QUEUE_MP_RECEIVE_RESPONSE,
    id,
    the_thread
  );
}

/* end of file */
