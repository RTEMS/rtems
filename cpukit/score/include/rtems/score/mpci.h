/*  mpci.h
 *
 *  This include file contains all the constants and structures associated
 *  with the MPCI layer.  It provides mechanisms to utilize packets.
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

#ifndef __MPCI_h
#define __MPCI_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/mppkt.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/coresem.h>

/*
 *  The following constants define the stack size requirements for
 *  the system threads.
 */
 
#define MPCI_RECEIVE_SERVER_STACK_SIZE \
  ( STACK_MINIMUM_SIZE + \
    CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK + \
    _CPU_Table.extra_mpci_receive_server_stack \
  )

/*
 *  The following defines the node number used when a broadcast is desired.
 */

#define MPCI_ALL_NODES 0

/*
 *  For packets associated with requests that don't already have a timeout,
 *  use the one specified by this MPCI driver.  The value specified by
 *   the MPCI driver sets an upper limit on how long a remote request
 *   should take to complete.
 */

#define MPCI_DEFAULT_TIMEOUT    0xFFFFFFFF

/*
 *  The following records define the Multiprocessor Communications
 *  Interface (MPCI) Table.  This table defines the user-provided
 *  MPCI which is a required part of a multiprocessor system.
 *
 *  For non-blocking local operations that become remote operations,
 *  we need a timeout.  This is a per-driver timeout: default_timeout
 */

typedef void MPCI_Entry;

typedef MPCI_Entry ( *MPCI_initialization_entry )( void );

typedef MPCI_Entry ( *MPCI_get_packet_entry )(
                 MP_packet_Prefix **
             );

typedef MPCI_Entry ( *MPCI_return_packet_entry )(
                 MP_packet_Prefix *
             );

typedef MPCI_Entry ( *MPCI_send_entry )(
                 unsigned32,
                 MP_packet_Prefix *
             );

typedef MPCI_Entry ( *MPCI_receive_entry )(
                 MP_packet_Prefix **
             );

typedef struct {
  unsigned32                 default_timeout;        /* in ticks */
  unsigned32                 maximum_packet_size;
  MPCI_initialization_entry  initialization;
  MPCI_get_packet_entry      get_packet;
  MPCI_return_packet_entry   return_packet;
  MPCI_send_entry            send_packet;
  MPCI_receive_entry         receive_packet;
} MPCI_Control;

/*
 *  The following defines the type for packet processing routines
 *  invoked by the MPCI Receive server.
 */

typedef void (*MPCI_Packet_processor)( MP_packet_Prefix * );
 
/*
 *  The following enumerated type defines the list of
 *  internal MP operations.
 */
 
typedef enum {
  MPCI_PACKETS_SYSTEM_VERIFY  =  0
}   MPCI_Internal_Remote_operations;
 
/*
 *  The following data structure defines the packet used to perform
 *  remote event operations.
 */
 
typedef struct {
  MP_packet_Prefix                 Prefix;
  MPCI_Internal_Remote_operations  operation;
  unsigned32                       maximum_nodes;
  unsigned32                       maximum_global_objects;
}    MPCI_Internal_packet;

/*
 *  This is the core semaphore which the MPCI Receive Server blocks on.
 */

SCORE_EXTERN CORE_semaphore_Control _MPCI_Semaphore;
/*
 *  The following thread queue is used to maintain a list of tasks
 *  which currently have outstanding remote requests.
 */

SCORE_EXTERN Thread_queue_Control _MPCI_Remote_blocked_threads;

/*
 *  The following define the internal pointers to the user's
 *  configuration information.
 */
 
SCORE_EXTERN MPCI_Control *_MPCI_table;

/*
 *  The following points to the MPCI Receive Server.
 */
 
SCORE_EXTERN Thread_Control *_MPCI_Receive_server_tcb;

/*
 *  The following table contains the process packet routines provided
 *  by each object that supports MP operations.
 */

SCORE_EXTERN MPCI_Packet_processor 
               _MPCI_Packet_processors[MP_PACKET_CLASSES_LAST+1];

/*
 *  _MPCI_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this handler.
 */

void _MPCI_Handler_initialization( 
  MPCI_Control            *users_mpci_table,
  unsigned32               timeout_status
);

/*
 *  _MPCI_Create_server
 *
 *  DESCRIPTION:
 *  
 *  This routine creates the packet receive server used in MP systems.
 */

void _MPCI_Create_server( void );

/*
 *  _MPCI_Initialization
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the MPCI driver by
 *  invoking the user provided MPCI initialization callout.
 */

void _MPCI_Initialization ( void );

/*
 *  _MPCI_Register_packet_processor
 *
 *  DESCRIPTION:
 *
 *  This routine registers the MPCI packet processor for the
 *  designated object class.
 */
 
void _MPCI_Register_packet_processor( 
  MP_packet_Classes      the_class,
  MPCI_Packet_processor  the_packet_processor
  
);
 
/*
 *  _MPCI_Get_packet
 *
 *  DESCRIPTION:
 *
 *  This function obtains a packet by invoking the user provided
 *  MPCI get packet callout.
 */

MP_packet_Prefix *_MPCI_Get_packet ( void );

/*
 *  _MPCI_Return_packet
 *
 *  DESCRIPTION:
 *
 *  This routine returns a packet by invoking the user provided
 *  MPCI return packet callout.
 */

void _MPCI_Return_packet (
  MP_packet_Prefix *the_packet
);

/*
 *  _MPCI_Send_process_packet
 *
 *  DESCRIPTION:
 *
 *  This routine sends a process packet by invoking the user provided
 *  MPCI send callout.
 */

void _MPCI_Send_process_packet (
  unsigned32        destination,
  MP_packet_Prefix *the_packet
);

/*
 *  _MPCI_Send_request_packet
 *
 *  DESCRIPTION:
 *
 *  This routine sends a request packet by invoking the user provided
 *  MPCI send callout.
 */

unsigned32 _MPCI_Send_request_packet (
  unsigned32         destination,
  MP_packet_Prefix  *the_packet,
  States_Control     extra_state
);

/*
 *  _MPCI_Send_response_packet
 *
 *  DESCRIPTION:
 *
 *  This routine sends a response packet by invoking the user provided
 *  MPCI send callout.
 */

void _MPCI_Send_response_packet (
  unsigned32        destination,
  MP_packet_Prefix *the_packet
);

/*
 *  _MPCI_Receive_packet
 *
 *  DESCRIPTION:
 *
 *  This routine receives a packet by invoking the user provided
 *  MPCI receive callout.
 */

MP_packet_Prefix  *_MPCI_Receive_packet ( void );

/*
 *  _MPCI_Process_response
 *
 *  DESCRIPTION:
 *
 *  This routine obtains a packet by invoking the user provided
 *  MPCI get packet callout.
 */

Thread_Control *_MPCI_Process_response (
  MP_packet_Prefix *the_packet
);

/*PAGE
 *
 *  _MPCI_Receive_server
 *
 */
 
Thread _MPCI_Receive_server(
  unsigned32 ignored
);

/*PAGE
 *
 *  _MPCI_Announce
 *
 *  DESCRIPTION:
 *
 *  XXX
 */
 
void _MPCI_Announce ( void );

/*
 *  _MPCI_Internal_packets_Send_process_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  process operation can be performed on another node.
 */
 
void _MPCI_Internal_packets_Send_process_packet (
   MPCI_Internal_Remote_operations operation
);
 
/*
 *  _MPCI_Internal_packets_Send_request_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  directive operation can be initiated on another node.
 *
 *  This routine is not needed since there are no request
 *  packets to be sent by this manager.
 */
 
/*
 *  _MPCI_Internal_packets_Send_response_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  directive can be performed on another node.
 *
 *  This routine is not needed since there are no response
 *  packets to be sent by this manager.
 */
 
/*
 *
 *  _MPCI_Internal_packets_Process_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs the actions specific to this package for
 *  the request from another node.
 */
 
void _MPCI_Internal_packets_Process_packet (
  MP_packet_Prefix *the_packet_prefix
);
 
/*
 *  _MPCI_Internal_packets_Send_object_was_deleted
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked indirectly by the thread queue
 *  when a proxy has been removed from the thread queue and
 *  the remote node must be informed of this.
 *
 *  This routine is not needed since there are no objects
 *  deleted by this manager.
 */
 
/*
 *  _MPCI_Internal_packets_Send_extract_proxy
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked when a task is deleted and it
 *  has a proxy which must be removed from a thread queue and
 *  the remote node must be informed of this.
 *
 *  This routine is not needed since there are no objects
 *  deleted by this manager.
 */
 
/*
 *  _MPCI_Internal_packets_Get_packet
 *
 *  DESCRIPTION:
 *
 *  This routine is used to obtain a internal threads mp packet.
 */
 
 MPCI_Internal_packet *_MPCI_Internal_packets_Get_packet ( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
