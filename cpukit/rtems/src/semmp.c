/**
 *  @file
 *
 *  @brief Semaphore MP Support
 *  @ingroup ClassicSEM
 */

/*
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

#include <rtems/rtems/semimpl.h>
#include <rtems/rtems/optionsimpl.h>

RTEMS_STATIC_ASSERT(
  sizeof(Semaphore_MP_Packet) <= MP_PACKET_MINIMUM_PACKET_SIZE,
  Semaphore_MP_Packet
);

void _Semaphore_MP_Send_process_packet (
  Semaphore_MP_Remote_operations  operation,
  Objects_Id                      semaphore_id,
  rtems_name                      name,
  Objects_Id                      proxy_id
)
{
  Semaphore_MP_Packet *the_packet;
  uint32_t             node;

  switch ( operation ) {

    case SEMAPHORE_MP_ANNOUNCE_CREATE:
    case SEMAPHORE_MP_ANNOUNCE_DELETE:
    case SEMAPHORE_MP_EXTRACT_PROXY:

      the_packet                    = _Semaphore_MP_Get_packet();
      the_packet->Prefix.the_class  = MP_PACKET_SEMAPHORE;
      the_packet->Prefix.length     = sizeof ( Semaphore_MP_Packet );
      the_packet->Prefix.to_convert = sizeof ( Semaphore_MP_Packet );
      the_packet->operation         = operation;
      the_packet->Prefix.id         = semaphore_id;
      the_packet->name              = name;
      the_packet->proxy_id          = proxy_id;

      if ( operation == SEMAPHORE_MP_EXTRACT_PROXY )
         node = _Objects_Get_node( semaphore_id );
      else
         node = MPCI_ALL_NODES;

      _MPCI_Send_process_packet( node, &the_packet->Prefix );
      break;

    case SEMAPHORE_MP_OBTAIN_REQUEST:
    case SEMAPHORE_MP_OBTAIN_RESPONSE:
    case SEMAPHORE_MP_RELEASE_REQUEST:
    case SEMAPHORE_MP_RELEASE_RESPONSE:
      break;
  }
}

rtems_status_code _Semaphore_MP_Send_request_packet (
  Semaphore_MP_Remote_operations operation,
  Objects_Id                     semaphore_id,
  rtems_option                   option_set,
  rtems_interval                 timeout
)
{
  Semaphore_MP_Packet *the_packet;

  switch ( operation ) {

    case SEMAPHORE_MP_OBTAIN_REQUEST:
    case SEMAPHORE_MP_RELEASE_REQUEST:

      the_packet                    = _Semaphore_MP_Get_packet();
      the_packet->Prefix.the_class  = MP_PACKET_SEMAPHORE;
      the_packet->Prefix.length     = sizeof ( Semaphore_MP_Packet );
      the_packet->Prefix.to_convert = sizeof ( Semaphore_MP_Packet );
      if ( ! _Options_Is_no_wait(option_set))
          the_packet->Prefix.timeout = timeout;

      the_packet->operation         = operation;
      the_packet->Prefix.id         = semaphore_id;
      the_packet->option_set        = option_set;

      return _MPCI_Send_request_packet(
          _Objects_Get_node( semaphore_id ),
          &the_packet->Prefix,
          STATES_WAITING_FOR_SEMAPHORE
        );
      break;

    case SEMAPHORE_MP_ANNOUNCE_CREATE:
    case SEMAPHORE_MP_ANNOUNCE_DELETE:
    case SEMAPHORE_MP_EXTRACT_PROXY:
    case SEMAPHORE_MP_OBTAIN_RESPONSE:
    case SEMAPHORE_MP_RELEASE_RESPONSE:
      break;

  }
  /*
   *  The following line is included to satisfy compilers which
   *  produce warnings when a function does not end with a return.
   */
  return RTEMS_SUCCESSFUL;
}

void _Semaphore_MP_Send_response_packet (
  Semaphore_MP_Remote_operations  operation,
  Objects_Id                      semaphore_id,
  Thread_Control                 *the_thread
)
{
  Semaphore_MP_Packet *the_packet;

  switch ( operation ) {

    case SEMAPHORE_MP_OBTAIN_RESPONSE:
    case SEMAPHORE_MP_RELEASE_RESPONSE:

      the_packet = ( Semaphore_MP_Packet *) the_thread->receive_packet;

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

    case SEMAPHORE_MP_ANNOUNCE_CREATE:
    case SEMAPHORE_MP_ANNOUNCE_DELETE:
    case SEMAPHORE_MP_EXTRACT_PROXY:
    case SEMAPHORE_MP_OBTAIN_REQUEST:
    case SEMAPHORE_MP_RELEASE_REQUEST:
      break;

  }
}

void _Semaphore_MP_Process_packet (
  rtems_packet_prefix  *the_packet_prefix
)
{
  Semaphore_MP_Packet *the_packet;
  Thread_Control      *the_thread;
  bool                 ignored;

  the_packet = (Semaphore_MP_Packet *) the_packet_prefix;

  switch ( the_packet->operation ) {

    case SEMAPHORE_MP_ANNOUNCE_CREATE:

      ignored = _Objects_MP_Allocate_and_open(
                  &_Semaphore_Information,
                  the_packet->name,
                  the_packet->Prefix.id,
                  true
                );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case SEMAPHORE_MP_ANNOUNCE_DELETE:

      _Objects_MP_Close( &_Semaphore_Information, the_packet->Prefix.id );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case SEMAPHORE_MP_EXTRACT_PROXY:

      the_thread = _Thread_MP_Find_proxy( the_packet->proxy_id );

      if ( ! _Thread_Is_null( the_thread ) )
        _Thread_queue_Extract( the_thread->Wait.queue, the_thread );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case SEMAPHORE_MP_OBTAIN_REQUEST:

      the_packet->Prefix.return_code = rtems_semaphore_obtain(
        the_packet->Prefix.id,
        the_packet->option_set,
        the_packet->Prefix.timeout
      );

      if ( the_packet->Prefix.return_code != RTEMS_PROXY_BLOCKING )
        _Semaphore_MP_Send_response_packet(
           SEMAPHORE_MP_OBTAIN_RESPONSE,
           the_packet->Prefix.id,
           _Thread_Executing
        );
      break;

    case SEMAPHORE_MP_OBTAIN_RESPONSE:
    case SEMAPHORE_MP_RELEASE_RESPONSE:

      the_thread = _MPCI_Process_response( the_packet_prefix );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case SEMAPHORE_MP_RELEASE_REQUEST:

      the_packet->Prefix.return_code = rtems_semaphore_release(
        the_packet->Prefix.id
      );

      _Semaphore_MP_Send_response_packet(
        SEMAPHORE_MP_RELEASE_RESPONSE,
        the_packet->Prefix.id,
        _Thread_Executing
      );
      break;
  }
}

void _Semaphore_MP_Send_object_was_deleted (
  Thread_Control *the_proxy
)
{
  the_proxy->receive_packet->return_code = RTEMS_OBJECT_WAS_DELETED;

  _Semaphore_MP_Send_response_packet(
    SEMAPHORE_MP_OBTAIN_RESPONSE,
    the_proxy->Wait.id,
    the_proxy
  );

}

void _Semaphore_MP_Send_extract_proxy (
  void           *argument
)
{
  Thread_Control *the_thread = (Thread_Control *)argument;

  _Semaphore_MP_Send_process_packet(
    SEMAPHORE_MP_EXTRACT_PROXY,
    the_thread->Wait.id,
    (rtems_name) 0,
    the_thread->Object.id
  );

}

Semaphore_MP_Packet *_Semaphore_MP_Get_packet ( void )
{
  return ( (Semaphore_MP_Packet *) _MPCI_Get_packet() );
}

#if defined(RTEMS_MULTIPROCESSING)
void  _Semaphore_Core_mutex_mp_support (
  Thread_Control *the_thread,
  Objects_Id      id
)
{
  the_thread->receive_packet->return_code = RTEMS_SUCCESSFUL;

  _Semaphore_MP_Send_response_packet(
     SEMAPHORE_MP_OBTAIN_RESPONSE,
     id,
     the_thread
   );
}
#endif

#if defined(RTEMS_MULTIPROCESSING)
void  _Semaphore_Core_semaphore_mp_support (
  Thread_Control *the_thread,
  Objects_Id      id
)
{
  the_thread->receive_packet->return_code = RTEMS_SUCCESSFUL;

  _Semaphore_MP_Send_response_packet(
     SEMAPHORE_MP_OBTAIN_RESPONSE,
     id,
     the_thread
   );
}
#endif
/* end of file */
