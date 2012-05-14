/*
 *  Multiprocessing Support for the Event Manager
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
#include <rtems/rtems/event.h>
#include <rtems/score/mpci.h>
#include <rtems/score/mppkt.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/support.h>

RTEMS_STATIC_ASSERT(
  sizeof(Event_MP_Packet) <= MP_PACKET_MINIMUM_PACKET_SIZE,
  Event_MP_Packet
);

/*
 *  _Event_MP_Send_process_packet
 *
 *  This subprogram is not needed since there are no process
 *  packets to be sent by this manager.
 *
 */

/*
 *  _Event_MP_Send_request_packet
 *
 */

rtems_status_code _Event_MP_Send_request_packet (
  Event_MP_Remote_operations operation,
  Objects_Id                 event_id,
  rtems_event_set            event_in
)
{
  Event_MP_Packet *the_packet;

  switch ( operation ) {

    case EVENT_MP_SEND_REQUEST:

      the_packet                    = _Event_MP_Get_packet();
      the_packet->Prefix.the_class  = MP_PACKET_EVENT;
      the_packet->Prefix.length     = sizeof ( Event_MP_Packet );
      the_packet->Prefix.to_convert = sizeof ( Event_MP_Packet );
      the_packet->operation         = operation;
      the_packet->Prefix.id         = event_id;
      the_packet->event_in          = event_in;

      return (rtems_status_code)
        _MPCI_Send_request_packet(
          _Objects_Get_node( event_id ),
          &the_packet->Prefix,
          STATES_READY
        );

      break;

    case EVENT_MP_SEND_RESPONSE:
      break;

  }
  /*
   *  The following line is included to satisfy compilers which
   *  produce warnings when a function does not end with a return.
   */
  return RTEMS_SUCCESSFUL;
}

/*
 *  _Event_MP_Send_response_packet
 *
 */

void _Event_MP_Send_response_packet (
  Event_MP_Remote_operations  operation,
  Thread_Control             *the_thread
)
{
  Event_MP_Packet *the_packet;

  switch ( operation ) {

    case EVENT_MP_SEND_RESPONSE:

      the_packet = ( Event_MP_Packet *) the_thread->receive_packet;

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

    case EVENT_MP_SEND_REQUEST:
      break;

  }
}

/*
 *
 *  _Event_MP_Process_packet
 *
 */

void _Event_MP_Process_packet (
  rtems_packet_prefix  *the_packet_prefix
)
{
  Event_MP_Packet *the_packet;
  Thread_Control  *the_thread;

  the_packet = (Event_MP_Packet *) the_packet_prefix;

  switch ( the_packet->operation ) {

    case EVENT_MP_SEND_REQUEST:

      the_packet->Prefix.return_code = rtems_event_send(
        the_packet->Prefix.id,
        the_packet->event_in
      );

      _Event_MP_Send_response_packet(
        EVENT_MP_SEND_RESPONSE,
        _Thread_Executing
      );
      break;

    case EVENT_MP_SEND_RESPONSE:

      the_thread = _MPCI_Process_response( the_packet_prefix );

      _MPCI_Return_packet( the_packet_prefix );

      break;

  }
}

/*
 *  _Event_MP_Send_object_was_deleted
 *
 *  This subprogram is not needed since there are no objects
 *  deleted by this manager.
 *
 */

/*
 *  _Event_MP_Send_extract_proxy
 *
 *  This subprogram is not needed since there are no objects
 *  deleted by this manager.
 *
 */

/*
 *  _Event_MP_Get_packet
 *
 */

Event_MP_Packet *_Event_MP_Get_packet ( void )
{
  return ( (Event_MP_Packet *) _MPCI_Get_packet() );
}

/* end of file */
