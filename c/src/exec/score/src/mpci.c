/*
 *  Multiprocessing Communications Interface (MPCI) Handler
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/cpu.h>
#include <rtems/fatal.h>
#include <rtems/mpci.h>
#include <rtems/mppkt.h>
#include <rtems/states.h>
#include <rtems/thread.h>
#include <rtems/threadq.h>
#include <rtems/tqdata.h>
#include <rtems/watchdog.h>

/*PAGE
 *
 *  _MPCI_Handler_initialization
 *
 *  This subprogram performs the initialization necessary for this handler.
 */

void _MPCI_Handler_initialization ( void )
{
  _Thread_queue_Initialize(
    &_MPCI_Remote_blocked_threads,
    OBJECTS_NO_CLASS,
    THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_RPC_REPLY,
    NULL
  );
}

/*PAGE
 *
 *  _MPCI_Initialization
 *
 *  This subprogram initializes the MPCI driver by
 *  invoking the user provided MPCI initialization callout.
 */

void _MPCI_Initialization ( void )
{
  (*_Configuration_MPCI_table->initialization)(
    _Configuration_Table,
    &_CPU_Table,
    _Configuration_MP_table
  );
}

/*PAGE
 *
 *  _MPCI_Get_packet
 *
 *  This subprogram obtains a packet by invoking the user provided
 *  MPCI get packet callout.
 */

rtems_packet_prefix *_MPCI_Get_packet ( void )
{
  rtems_packet_prefix  *the_packet;

  (*_Configuration_MPCI_table->get_packet)( &the_packet );

  if ( the_packet == NULL )
    rtems_fatal_error_occurred( RTEMS_UNSATISFIED );

  /*
   *  Put in a default timeout that will be used for
   *  all packets that do not otherwise have a timeout.
   */

  the_packet->timeout = MPCI_DEFAULT_TIMEOUT;

  return the_packet;
}

/*PAGE
 *
 *  _MPCI_Return_packet
 *
 *  This subprogram returns a packet by invoking the user provided
 *  MPCI return packet callout.
 */

void _MPCI_Return_packet (
  rtems_packet_prefix   *the_packet
)
{
  (*_Configuration_MPCI_table->return_packet)( the_packet );
}

/*PAGE
 *
 *  _MPCI_Send_process_packet
 *
 *  This subprogram sends a process packet by invoking the user provided
 *  MPCI send callout.
 */

void _MPCI_Send_process_packet (
  unsigned32          destination,
  rtems_packet_prefix   *the_packet
)
{
  the_packet->source_tid = _Thread_Executing->Object.id;
  the_packet->to_convert =
     ( the_packet->to_convert - sizeof(rtems_packet_prefix) ) /
       sizeof(unsigned32);

  (*_Configuration_MPCI_table->send_packet)( destination, the_packet );
}

/*PAGE
 *
 *  _MPCI_Send_request_packet
 *
 *  This subprogram sends a request packet by invoking the user provided
 *  MPCI send callout.
 */

rtems_status_code _MPCI_Send_request_packet (
  unsigned32          destination,
  rtems_packet_prefix   *the_packet,
  States_Control      extra_state
)
{
  the_packet->source_tid      = _Thread_Executing->Object.id;
  the_packet->source_priority = _Thread_Executing->current_priority;
  the_packet->to_convert =
     ( the_packet->to_convert - sizeof(rtems_packet_prefix) ) /
       sizeof(unsigned32);

  _Thread_Executing->Wait.id = the_packet->id;

  _Thread_Executing->Wait.queue = &_MPCI_Remote_blocked_threads;

  _Thread_Disable_dispatch();

    (*_Configuration_MPCI_table->send_packet)( destination, the_packet );

    _MPCI_Remote_blocked_threads.sync = TRUE;

    /*
     *  See if we need a default timeout
     */

    if (the_packet->timeout == MPCI_DEFAULT_TIMEOUT)
        the_packet->timeout = _Configuration_MPCI_table->default_timeout;

    _Thread_queue_Enqueue( &_MPCI_Remote_blocked_threads, the_packet->timeout );

    _Thread_Executing->current_state =
      _States_Set( extra_state, _Thread_Executing->current_state );

  _Thread_Enable_dispatch();

  return _Thread_Executing->Wait.return_code;
}

/*PAGE
 *
 *  _MPCI_Send_response_packet
 *
 *  This subprogram sends a response packet by invoking the user provided
 *  MPCI send callout.
 */

void _MPCI_Send_response_packet (
  unsigned32          destination,
  rtems_packet_prefix   *the_packet
)
{
  the_packet->source_tid = _Thread_Executing->Object.id;

  (*_Configuration_MPCI_table->send_packet)( destination, the_packet );
}

/*PAGE
 *
 *  _MPCI_Receive_packet
 *
 *  This subprogram receives a packet by invoking the user provided
 *  MPCI receive callout.
 */

rtems_packet_prefix  *_MPCI_Receive_packet ( void )
{
  rtems_packet_prefix  *the_packet;

  (*_Configuration_MPCI_table->receive_packet)( &the_packet );

  return the_packet;
}

/*PAGE
 *
 *  _MPCI_Process_response
 *
 *  This subprogram obtains a packet by invoking the user provided
 *  MPCI get packet callout.
 */

Thread_Control *_MPCI_Process_response (
  rtems_packet_prefix  *the_packet
)
{
  Thread_Control    *the_thread;
  Objects_Locations  location;

  the_thread = _Thread_Get( the_packet->id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:
      the_thread = NULL;          /* IMPOSSIBLE */
      break;
    case OBJECTS_LOCAL:
      _Thread_queue_Extract( &_MPCI_Remote_blocked_threads, the_thread );
      the_thread->Wait.return_code = the_packet->return_code;
      _Thread_Unnest_dispatch();
    break;
  }

  return the_thread;
}

/* end of file */
