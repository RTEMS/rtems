/*
 *  Multiprocessing Support for the Partition Manager
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
#include <rtems/score/mpci.h>
#include <rtems/score/mppkt.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/part.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/support.h>

RTEMS_STATIC_ASSERT(
  sizeof(Partition_MP_Packet) <= MP_PACKET_MINIMUM_PACKET_SIZE,
  Partition_MP_Packet
);

/*
 *  _Partition_MP_Send_process_packet
 *
 */

void _Partition_MP_Send_process_packet (
  Partition_MP_Remote_operations  operation,
  Objects_Id                      partition_id,
  rtems_name                      name,
  Objects_Id                      proxy_id
)
{
  Partition_MP_Packet *the_packet;
  uint32_t             node;

  switch ( operation ) {

    case PARTITION_MP_ANNOUNCE_CREATE:
    case PARTITION_MP_ANNOUNCE_DELETE:
    case PARTITION_MP_EXTRACT_PROXY:

      the_packet                    = _Partition_MP_Get_packet();
      the_packet->Prefix.the_class  = MP_PACKET_PARTITION;
      the_packet->Prefix.length     = sizeof ( Partition_MP_Packet );
      the_packet->Prefix.to_convert = sizeof ( Partition_MP_Packet );
      the_packet->operation         = operation;
      the_packet->Prefix.id         = partition_id;
      the_packet->name              = name;
      the_packet->proxy_id          = proxy_id;

      if ( operation == PARTITION_MP_EXTRACT_PROXY )
         node = _Objects_Get_node( partition_id );
      else
         node = MPCI_ALL_NODES;

      _MPCI_Send_process_packet( node, &the_packet->Prefix );
      break;

    case PARTITION_MP_GET_BUFFER_REQUEST:
    case PARTITION_MP_GET_BUFFER_RESPONSE:
    case PARTITION_MP_RETURN_BUFFER_REQUEST:
    case PARTITION_MP_RETURN_BUFFER_RESPONSE:
      break;
  }
}

/*
 *  _Partition_MP_Send_request_packet
 *
 */

rtems_status_code _Partition_MP_Send_request_packet (
  Partition_MP_Remote_operations  operation,
  Objects_Id                      partition_id,
  void                           *buffer
)
{
  Partition_MP_Packet *the_packet;

  switch ( operation ) {

    case PARTITION_MP_GET_BUFFER_REQUEST:
    case PARTITION_MP_RETURN_BUFFER_REQUEST:

      the_packet                    = _Partition_MP_Get_packet();
      the_packet->Prefix.the_class  = MP_PACKET_PARTITION;
      the_packet->Prefix.length     = sizeof ( Partition_MP_Packet );
      the_packet->Prefix.to_convert = sizeof ( Partition_MP_Packet );
      the_packet->operation         = operation;
      the_packet->Prefix.id         = partition_id;
      the_packet->buffer            = buffer;

      return
        _MPCI_Send_request_packet(
          _Objects_Get_node( partition_id ),
          &the_packet->Prefix,
          STATES_READY      /* Not used */
        );

      break;

    case PARTITION_MP_ANNOUNCE_CREATE:
    case PARTITION_MP_ANNOUNCE_DELETE:
    case PARTITION_MP_EXTRACT_PROXY:
    case PARTITION_MP_GET_BUFFER_RESPONSE:
    case PARTITION_MP_RETURN_BUFFER_RESPONSE:
      break;

  }
  /*
   *  The following line is included to satisfy compilers which
   *  produce warnings when a function does not end with a return.
   */
  return RTEMS_SUCCESSFUL;
}

/*
 *  _Partition_MP_Send_response_packet
 *
 */

void _Partition_MP_Send_response_packet (
  Partition_MP_Remote_operations  operation,
  Objects_Id                      partition_id,
  Thread_Control                 *the_thread
)
{
  Partition_MP_Packet *the_packet;

  switch ( operation ) {

    case PARTITION_MP_GET_BUFFER_RESPONSE:
    case PARTITION_MP_RETURN_BUFFER_RESPONSE:

      the_packet = ( Partition_MP_Packet *) the_thread->receive_packet;

/*
 *  The packet being returned already contains the class, length, and
 *  to_convert fields, therefore they are not set in this routine.
 */
      the_packet->operation = operation;
      the_packet->Prefix.id = the_packet->Prefix.source_tid;

      _MPCI_Send_response_packet(
        _Objects_Get_node( the_packet->Prefix.source_tid ),
        &the_packet->Prefix
      );
      break;

    case PARTITION_MP_ANNOUNCE_CREATE:
    case PARTITION_MP_ANNOUNCE_DELETE:
    case PARTITION_MP_EXTRACT_PROXY:
    case PARTITION_MP_GET_BUFFER_REQUEST:
    case PARTITION_MP_RETURN_BUFFER_REQUEST:
      break;

  }
}

/*
 *
 *  _Partition_MP_Process_packet
 *
 */

void _Partition_MP_Process_packet (
  rtems_packet_prefix  *the_packet_prefix
)
{
  Partition_MP_Packet *the_packet;
  Thread_Control      *the_thread;
  bool                 ignored;

  the_packet = (Partition_MP_Packet *) the_packet_prefix;

  switch ( the_packet->operation ) {

    case PARTITION_MP_ANNOUNCE_CREATE:

      ignored = _Objects_MP_Allocate_and_open(
                  &_Partition_Information,
                  the_packet->name,
                  the_packet->Prefix.id,
                  true
                );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case PARTITION_MP_ANNOUNCE_DELETE:

      _Objects_MP_Close( &_Partition_Information, the_packet->Prefix.id );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case PARTITION_MP_EXTRACT_PROXY:

      the_thread = _Thread_MP_Find_proxy( the_packet->proxy_id );

      if ( ! _Thread_Is_null( the_thread ) )
         _Thread_queue_Extract( the_thread->Wait.queue, the_thread );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case PARTITION_MP_GET_BUFFER_REQUEST:

      the_packet->Prefix.return_code = rtems_partition_get_buffer(
        the_packet->Prefix.id,
        &the_packet->buffer
      );

      _Partition_MP_Send_response_packet(
        PARTITION_MP_GET_BUFFER_RESPONSE,
        the_packet->Prefix.id,
        _Thread_Executing
      );
      break;

    case PARTITION_MP_GET_BUFFER_RESPONSE:

      the_thread = _MPCI_Process_response( the_packet_prefix );

      *(void **)the_thread->Wait.return_argument = the_packet->buffer;

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case PARTITION_MP_RETURN_BUFFER_REQUEST:

      the_packet->Prefix.return_code = rtems_partition_return_buffer(
        the_packet->Prefix.id,
        the_packet->buffer
      );

      _Partition_MP_Send_response_packet(
        PARTITION_MP_RETURN_BUFFER_RESPONSE,
        the_packet->Prefix.id,
        _Thread_Executing
      );
      break;

    case PARTITION_MP_RETURN_BUFFER_RESPONSE:

      the_thread = _MPCI_Process_response( the_packet_prefix );

      _MPCI_Return_packet( the_packet_prefix );
      break;

  }
}

/*
 *  _Partition_MP_Send_object_was_deleted
 *
 *  This routine is not needed by the Partition since a partition
 *  cannot be deleted when buffers are in use.
 *
 */

/*
 *  _Partition_MP_Send_extract_proxy
 *
 */

void _Partition_MP_Send_extract_proxy (
  void           *argument
)
{
  Thread_Control *the_thread = (Thread_Control *)argument;

  _Partition_MP_Send_process_packet(
    PARTITION_MP_EXTRACT_PROXY,
    the_thread->Wait.id,
    (rtems_name) 0,
    the_thread->Object.id
  );

}

/*
 *  _Partition_MP_Get_packet
 *
 */

Partition_MP_Packet *_Partition_MP_Get_packet ( void )
{
  return ( (Partition_MP_Packet *) _MPCI_Get_packet() );
}

/* end of file */
