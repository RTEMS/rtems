/*
 *  Multiprocessing Communications Interface (MPCI) Handler
 *
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

#include <rtems/system.h>
#include <rtems/score/cpu.h>
#include <rtems/score/interr.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#include <rtems/score/mppkt.h>
#endif
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/sysstate.h>

#include <rtems/score/coresem.h>

/*PAGE
 *
 *  _MPCI_Handler_initialization
 *
 *  This subprogram performs the initialization necessary for this handler.
 */

void _MPCI_Handler_initialization( 
  MPCI_Control            *users_mpci_table,
  unsigned32               timeout_status
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

  if ( !_System_state_Is_multiprocessing )
    return;

  /*
   *  Register the MP Process Packet routine.
   */
 
  _MPCI_Register_packet_processor(
    MP_PACKET_MPCI_INTERNAL,
    _MPCI_Internal_packets_Process_packet
  );

  /*
   *  Create the counting semaphore used by the MPCI Receive Server.
   */

  attributes.discipline = CORE_SEMAPHORE_DISCIPLINES_FIFO;

  _CORE_semaphore_Initialize(
    &_MPCI_Semaphore,
    &attributes,              /* the_semaphore_attributes */
    0                         /* initial_value */
  );

  _Thread_queue_Initialize(
    &_MPCI_Remote_blocked_threads,
    THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_RPC_REPLY,
    timeout_status
  );
}

/*PAGE
 *
 *  _MPCI_Create_server
 *
 *  This subprogram creates the MPCI receive server.
 */

char *_MPCI_Internal_name = "MPCI";

void _MPCI_Create_server( void )
{

  if ( !_System_state_Is_multiprocessing )
    return;

  /*
   *  Initialize the MPCI Receive Server
   */

  _MPCI_Receive_server_tcb = _Thread_Internal_allocate();
 
  _Thread_Initialize(
    &_Thread_Internal_information,
    _MPCI_Receive_server_tcb,
    NULL,        /* allocate the stack */
    MPCI_RECEIVE_SERVER_STACK_SIZE,
    CPU_ALL_TASKS_ARE_FP,
    PRIORITY_MINIMUM,
    FALSE,       /* no preempt */
    THREAD_CPU_BUDGET_ALGORITHM_NONE,
    NULL,        /* no budget algorithm callout */
    0,           /* all interrupts enabled */
    _MPCI_Internal_name
  );
 
  _Thread_Start(
    _MPCI_Receive_server_tcb,
    THREAD_START_NUMERIC,
    (void *) _MPCI_Receive_server,
    NULL,
    0
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
  MP_packet_Classes      the_class,
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

    _Thread_queue_Enter_critical_section( &_MPCI_Remote_blocked_threads );

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

Thread _MPCI_Receive_server(
  unsigned32 ignored
)
{
 
  MP_packet_Prefix         *the_packet;
  MPCI_Packet_processor     the_function;
  Thread_Control           *executing;
 
  executing = _Thread_Executing;

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

  return 0;   /* unreached - only to remove warnings */
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

/*PAGE
 *
 *  _MPCI_Internal_packets_Send_process_packet
 *
 */
 
void _MPCI_Internal_packets_Send_process_packet (
   MPCI_Internal_Remote_operations operation
)
{
  MPCI_Internal_packet *the_packet;
 
  switch ( operation ) {
 
    case MPCI_PACKETS_SYSTEM_VERIFY:
 
      the_packet                    = _MPCI_Internal_packets_Get_packet();
      the_packet->Prefix.the_class  = MP_PACKET_MPCI_INTERNAL;
      the_packet->Prefix.length     = sizeof ( MPCI_Internal_packet );
      the_packet->Prefix.to_convert = sizeof ( MPCI_Internal_packet );
      the_packet->operation         = operation;
 
      the_packet->maximum_nodes = _Objects_Maximum_nodes;
 
      the_packet->maximum_global_objects = _Objects_MP_Maximum_global_objects;
 
      _MPCI_Send_process_packet( MPCI_ALL_NODES, &the_packet->Prefix );
      break;
  }
}
 
/*PAGE
 *
 *  _MPCI_Internal_packets_Send_request_packet
 *
 *  This subprogram is not needed since there are no request
 *  packets to be sent by this manager.
 *
 */
 
/*PAGE
 *
 *  _MPCI_Internal_packets_Send_response_packet
 *
 *  This subprogram is not needed since there are no response
 *  packets to be sent by this manager.
 *
 */
 
/*PAGE
 *
 *
 *  _MPCI_Internal_packets_Process_packet
 *
 */
 
void _MPCI_Internal_packets_Process_packet (
  MP_packet_Prefix  *the_packet_prefix
)
{
  MPCI_Internal_packet *the_packet;
  unsigned32                  maximum_nodes;
  unsigned32                  maximum_global_objects;
 
  the_packet = (MPCI_Internal_packet *) the_packet_prefix;
 
  switch ( the_packet->operation ) {
 
    case MPCI_PACKETS_SYSTEM_VERIFY:
 
      maximum_nodes          = the_packet->maximum_nodes;
      maximum_global_objects = the_packet->maximum_global_objects;
      if ( maximum_nodes != _Objects_Maximum_nodes ||
           maximum_global_objects != _Objects_MP_Maximum_global_objects ) {
 
        _MPCI_Return_packet( the_packet_prefix );
 
        _Internal_error_Occurred(
          INTERNAL_ERROR_CORE,
          TRUE,
          INTERNAL_ERROR_INCONSISTENT_MP_INFORMATION
        );
      }
 
      _MPCI_Return_packet( the_packet_prefix );
 
      break;
  }
}
 
/*PAGE
 *
 *  _MPCI_Internal_packets_Send_object_was_deleted
 *
 *  This subprogram is not needed since there are no objects
 *  deleted by this manager.
 *
 */
 
/*PAGE
 *
 *  _MPCI_Internal_packets_Send_extract_proxy
 *
 *  This subprogram is not needed since there are no objects
 *  deleted by this manager.
 *
 */
 
/*PAGE
 *
 *  _MPCI_Internal_packets_Get_packet
 *
 */
 
MPCI_Internal_packet *_MPCI_Internal_packets_Get_packet ( void )
{
  return ( (MPCI_Internal_packet *) _MPCI_Get_packet() );
}

/* end of file */
