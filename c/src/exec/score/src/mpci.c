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
#include <rtems/core/cpu.h>
#include <rtems/core/interr.h>
#include <rtems/core/mpci.h>
#include <rtems/core/mppkt.h>
#include <rtems/core/states.h>
#include <rtems/core/thread.h>
#include <rtems/core/threadq.h>
#include <rtems/core/tqdata.h>
#include <rtems/core/watchdog.h>
#include <rtems/sysstate.h>

#include <rtems/core/coresem.h>

#include <rtems/rtems/status.h> /* XXX for TIMEOUT */

/*PAGE
 *
 *  _MPCI_Handler_initialization
 *
 *  This subprogram performs the initialization necessary for this handler.
 */

void _MPCI_Handler_initialization( 
  MPCI_Control            *users_mpci_table
)
{
  CORE_semaphore_Attributes    attributes;

  if ( _System_state_Is_multiprocessing && !users_mpci_table )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      INTERNAL_ERROR_NO_MPCI
    );

  _MPCI_table = users_mpci_table;

  attributes.discipline = CORE_SEMAPHORE_DISCIPLINES_FIFO;

  _CORE_semaphore_Initialize(
    &_MPCI_Semaphore,
    OBJECTS_NO_CLASS,         /* free floating semaphore */
    &attributes,              /* the_semaphore_attributes */
    0,                        /* initial_value */
    NULL                      /* proxy_extract_callout */
  );

  _Thread_queue_Initialize(
    &_MPCI_Remote_blocked_threads,
    OBJECTS_NO_CLASS,
    THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_RPC_REPLY,
    NULL,
    RTEMS_TIMEOUT
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
  (*_MPCI_table->initialization)();
}

/*PAGE
 *
 *  _MPCI_Register_packet_processor
 *
 *  This routine registers the MPCI packet processor for the
 *  designated object class.
 */
 
void _MPCI_Register_packet_processor(
  Objects_Classes        the_class,
  MPCI_Packet_processor  the_packet_processor
 
)
{
  _MPCI_Packet_processors[ the_class ] = the_packet_processor;
}

/*PAGE
 *
 *  _MPCI_Get_packet
 *
 *  This subprogram obtains a packet by invoking the user provided
 *  MPCI get packet callout.
 */

MP_packet_Prefix *_MPCI_Get_packet ( void )
{
  MP_packet_Prefix  *the_packet;

  (*_MPCI_table->get_packet)( &the_packet );

  if ( the_packet == NULL )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      INTERNAL_ERROR_OUT_OF_PACKETS
    );

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
  MP_packet_Prefix   *the_packet
)
{
  (*_MPCI_table->return_packet)( the_packet );
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
  MP_packet_Prefix   *the_packet
)
{
  the_packet->source_tid = _Thread_Executing->Object.id;
  the_packet->to_convert =
     ( the_packet->to_convert - sizeof(MP_packet_Prefix) ) /
       sizeof(unsigned32);

  (*_MPCI_table->send_packet)( destination, the_packet );
}

/*PAGE
 *
 *  _MPCI_Send_request_packet
 *
 *  This subprogram sends a request packet by invoking the user provided
 *  MPCI send callout.
 */

unsigned32 _MPCI_Send_request_packet (
  unsigned32          destination,
  MP_packet_Prefix   *the_packet,
  States_Control      extra_state
)
{
  the_packet->source_tid      = _Thread_Executing->Object.id;
  the_packet->source_priority = _Thread_Executing->current_priority;
  the_packet->to_convert =
     ( the_packet->to_convert - sizeof(MP_packet_Prefix) ) /
       sizeof(unsigned32);

  _Thread_Executing->Wait.id = the_packet->id;

  _Thread_Executing->Wait.queue = &_MPCI_Remote_blocked_threads;

  _Thread_Disable_dispatch();

    (*_MPCI_table->send_packet)( destination, the_packet );

    _MPCI_Remote_blocked_threads.sync = TRUE;

    /*
     *  See if we need a default timeout
     */

    if (the_packet->timeout == MPCI_DEFAULT_TIMEOUT)
        the_packet->timeout = _MPCI_table->default_timeout;

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
  MP_packet_Prefix   *the_packet
)
{
  the_packet->source_tid = _Thread_Executing->Object.id;

  (*_MPCI_table->send_packet)( destination, the_packet );
}

/*PAGE
 *
 *  _MPCI_Receive_packet
 *
 *  This subprogram receives a packet by invoking the user provided
 *  MPCI receive callout.
 */

MP_packet_Prefix  *_MPCI_Receive_packet ( void )
{
  MP_packet_Prefix  *the_packet;

  (*_MPCI_table->receive_packet)( &the_packet );

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
  MP_packet_Prefix  *the_packet
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

/*PAGE
 *
 *  _MPCI_Receive_server
 *
 */

void _MPCI_Receive_server( void )
{
 
  MP_packet_Prefix         *the_packet;
  MPCI_Packet_processor     the_function;
  Thread_Control           *executing;
 
  executing = _Thread_Executing;
  _MPCI_Receive_server_tcb = executing;

  for ( ; ; ) {
 
    executing->receive_packet = NULL;

    _Thread_Disable_dispatch();
    _CORE_semaphore_Seize( &_MPCI_Semaphore, 0, TRUE, WATCHDOG_NO_TIMEOUT );
    _Thread_Enable_dispatch();
 
    for ( ; ; ) {
      the_packet = _MPCI_Receive_packet();
 
      if ( !the_packet )
        break;
 
      executing->receive_packet = the_packet;
 
      if ( !_Mp_packet_Is_valid_packet_class ( the_packet->the_class ) )
        break;
 
      the_function = _MPCI_Packet_processors[ the_packet->the_class ];
 
      if ( !the_function )
        _Internal_error_Occurred(
          INTERNAL_ERROR_CORE,
          TRUE,
          INTERNAL_ERROR_BAD_PACKET
        );
 
        (*the_function)( the_packet );
    }
  }
}

/*PAGE
 *
 *  _MPCI_Announce
 *
 */
 
void _MPCI_Announce ( void )
{
  _Thread_Disable_dispatch();
  (void) _CORE_semaphore_Surrender( &_MPCI_Semaphore, 0, 0 );
  _Thread_Enable_dispatch();
}

/* end of file */
