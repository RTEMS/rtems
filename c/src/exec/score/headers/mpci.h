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

#ifndef __RTEMS_MPCI_h
#define __RTEMS_MPCI_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/mppkt.h>
#include <rtems/states.h>
#include <rtems/status.h>
#include <rtems/thread.h>
#include <rtems/threadq.h>
#include <rtems/tqdata.h>
#include <rtems/watchdog.h>

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
 *  _MPCI_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this handler.
 */

void _MPCI_Handler_initialization ( void );

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
 *  _MPCI_Get_packet
 *
 *  DESCRIPTION:
 *
 *  This function obtains a packet by invoking the user provided
 *  MPCI get packet callout.
 */

rtems_packet_prefix *_MPCI_Get_packet ( void );

/*
 *  _MPCI_Return_packet
 *
 *  DESCRIPTION:
 *
 *  This routine returns a packet by invoking the user provided
 *  MPCI return packet callout.
 */

void _MPCI_Return_packet (
  rtems_packet_prefix *the_packet
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
  rtems_packet_prefix *the_packet
);

/*
 *  _MPCI_Send_request_packet
 *
 *  DESCRIPTION:
 *
 *  This routine sends a request packet by invoking the user provided
 *  MPCI send callout.
 */

rtems_status_code _MPCI_Send_request_packet (
  unsigned32         destination,
  rtems_packet_prefix  *the_packet,
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
  rtems_packet_prefix *the_packet
);

/*
 *  _MPCI_Receive_packet
 *
 *  DESCRIPTION:
 *
 *  This routine receives a packet by invoking the user provided
 *  MPCI receive callout.
 */

rtems_packet_prefix  *_MPCI_Receive_packet ( void );

/*
 *  _MPCI_Process_response
 *
 *  DESCRIPTION:
 *
 *  This routine obtains a packet by invoking the user provided
 *  MPCI get packet callout.
 */

Thread_Control *_MPCI_Process_response (
  rtems_packet_prefix *the_packet
);

/*
 *  The following thread queue is used to maintain a list of tasks
 *  which currently have outstanding remote requests.
 */

EXTERN Thread_queue_Control _MPCI_Remote_blocked_threads;

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
