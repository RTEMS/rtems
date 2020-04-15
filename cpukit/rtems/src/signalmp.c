/**
 *  @file
 *
 *  @brief Signal MP Support
 *  @ingroup ClassicSignalMP
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/signalimpl.h>
#include <rtems/rtems/optionsimpl.h>
#include <rtems/rtems/statusimpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>

/**
 *  The following enumerated type defines the list of
 *  remote signal operations.
 */
typedef enum {
  SIGNAL_MP_SEND_REQUEST  = 0,
  SIGNAL_MP_SEND_RESPONSE = 1
}   Signal_MP_Remote_operations;

/**
 *  The following data structure defines the packet used to perform
 *  remote signal operations.
 */
typedef struct {
  rtems_packet_prefix          Prefix;
  Signal_MP_Remote_operations  operation;
  rtems_signal_set             signal_set;
}   Signal_MP_Packet;

RTEMS_STATIC_ASSERT(
  sizeof(Signal_MP_Packet) <= MP_PACKET_MINIMUM_PACKET_SIZE,
  Signal_MP_Packet
);

static Signal_MP_Packet *_Signal_MP_Get_packet( Objects_Id id )
{
  if ( !_Thread_MP_Is_remote( id ) ) {
    return NULL;
  }

  return (Signal_MP_Packet *) _MPCI_Get_packet();
}

/*
 *  _Signal_MP_Send_process_packet
 *
 *  This subprogram is not needed since there are no process
 *  packets to be sent by this manager.
 *
 */

rtems_status_code _Signal_MP_Send(
  rtems_id         id,
  rtems_signal_set signal_set
)
{
  Signal_MP_Packet *the_packet;
  Status_Control    status;

  the_packet = _Signal_MP_Get_packet( id );
  if ( the_packet == NULL ) {
    return RTEMS_INVALID_ID;
  }

  the_packet->Prefix.the_class  = MP_PACKET_SIGNAL;
  the_packet->Prefix.length     = sizeof( *the_packet );
  the_packet->Prefix.to_convert = sizeof( *the_packet );
  the_packet->operation         = SIGNAL_MP_SEND_REQUEST;
  the_packet->Prefix.id         = id;
  the_packet->signal_set        = signal_set;

  status = _MPCI_Send_request_packet(
    _Objects_Get_node( id ),
    &the_packet->Prefix,
    STATES_READY
  );
  return _Status_Get( status );
}

static void _Signal_MP_Send_response_packet (
  Signal_MP_Remote_operations  operation,
  Thread_Control              *the_thread
)
{
  Signal_MP_Packet *the_packet;

  switch ( operation ) {

    case SIGNAL_MP_SEND_RESPONSE:

      the_packet = ( Signal_MP_Packet *) the_thread->receive_packet;

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

    case SIGNAL_MP_SEND_REQUEST:
      break;

  }
}

void _Signal_MP_Process_packet (
  rtems_packet_prefix  *the_packet_prefix
)
{
  Signal_MP_Packet *the_packet;

  the_packet = (Signal_MP_Packet *) the_packet_prefix;

  switch ( the_packet->operation ) {

    case SIGNAL_MP_SEND_REQUEST:

      the_packet->Prefix.return_code = rtems_signal_send(
        the_packet->Prefix.id,
        the_packet->signal_set
      );

      _Signal_MP_Send_response_packet(
        SIGNAL_MP_SEND_RESPONSE,
        _Thread_Executing
      );
      break;

    case SIGNAL_MP_SEND_RESPONSE:

      _MPCI_Process_response( the_packet_prefix );
      _MPCI_Return_packet( the_packet_prefix );
      break;

  }
}

/*
 *  _Signal_MP_Send_object_was_deleted
 *
 *  This subprogram is not needed since there are no objects
 *  deleted by this manager.
 *
 */

/*
 *  _Signal_MP_Send_extract_proxy
 *
 *  This subprogram is not needed since there are no objects
 *  deleted by this manager.
 *
 */

/* end of file */
