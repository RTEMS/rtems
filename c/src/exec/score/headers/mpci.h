/*  mpci.h
 *
 *  This include file contains all the constants and structures associated
 *  with the MPCI layer.  It provides mechanisms to utilize packets.
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

#ifndef __MPCI_h
#define __MPCI_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/core/mppkt.h>
#include <rtems/core/states.h>
#include <rtems/core/thread.h>
#include <rtems/core/threadq.h>
#include <rtems/core/tqdata.h>
#include <rtems/core/watchdog.h>
#include <rtems/core/coresem.h>

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
 *  This is the core semaphore which the MPCI Receive Server blocks on.
 */

EXTERN CORE_semaphore_Control _MPCI_Semaphore;
/*
 *  The following thread queue is used to maintain a list of tasks
 *  which currently have outstanding remote requests.
 */

EXTERN Thread_queue_Control _MPCI_Remote_blocked_threads;

/*
 *  The following define the internal pointers to the user's
 *  configuration information.
 */
 
EXTERN MPCI_Control *_MPCI_table;

/*
 *  The following points to the MPCI Receive Server.
 */
 
EXTERN Thread_Control *_MPCI_Receive_server_tcb;

/*
 *  The following table contains the process packet routines provided
 *  by each object that supports MP operations.
 */

EXTERN MPCI_Packet_processor _MPCI_Packet_processors[MP_PACKET_CLASSES_LAST+1];

/*
 *  _MPCI_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this handler.
 */

void _MPCI_Handler_initialization( 
  MPCI_Control            *users_mpci_table
);

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
  MP_packet_Classes      the_object,
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
 
void _MPCI_Receive_server( void );

/*PAGE
 *
 *  _MPCI_Announce
 *
 *  DESCRIPTION:
 *
 *  XXX
 */
 
void _MPCI_Announce ( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
