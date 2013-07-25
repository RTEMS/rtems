/**
 * @file
 *
 * @ingroup ScoreMPCI
 *
 * @brief MPCI Layer API
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_MPCI_H
#define _RTEMS_SCORE_MPCI_H

#include <rtems/score/mppkt.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/watchdog.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreMPCI MPCI Handler
 *
 *  @ingroup Score
 *
 *  The MPCI Handler encapsulates functionality which is related to the
 *  generation, receipt, and processing of remote operations in a
 *  multiprocessor system.  This handler contains the message passing
 *  support for making remote service calls as well as the server thread
 *  which processes requests from remote nodes.
*/
/**@{*/

/**
 *  The following defines the node number used when a broadcast is desired.
 */
#define MPCI_ALL_NODES 0

/**
 *  This type is returned by all user provided MPCI routines.
 */
typedef void MPCI_Entry;

/**
 *  This type defines the prototype for the initization entry point
 *  in an Multiprocessor Communications Interface.
 */
typedef MPCI_Entry ( *MPCI_initialization_entry )( void );

/**
 *  This type defines the prototype for the get packet entry point
 *  in an Multiprocessor Communications Interface.  The single
 *  parameter will point to the packet allocated.
 */
typedef MPCI_Entry ( *MPCI_get_packet_entry )(
                     MP_packet_Prefix **
                   );

/**
 *  This type defines the prototype for the return packet entry point
 *  in an Multiprocessor Communications Interface.  The single
 *  parameter will point to a packet previously allocated by the
 *  get packet MPCI entry.
 */
typedef MPCI_Entry ( *MPCI_return_packet_entry )(
                     MP_packet_Prefix *
                   );

/**
 *  This type defines the prototype for send get packet entry point
 *  in an Multiprocessor Communications Interface.  The single
 *  parameter will point to a packet previously allocated by the
 *  get packet entry point that has been filled in by the caller.
 */
typedef MPCI_Entry ( *MPCI_send_entry )(
                     uint32_t,
                     MP_packet_Prefix *
                   );

/**
 *  This type defines the prototype for the receive packet entry point
 *  in an Multiprocessor Communications Interface.  The single
 *  parameter will point to a packet allocated and filled in by the
 *  receive packet handler.  The caller will block until a packet is
 *  received.
 */
typedef MPCI_Entry ( *MPCI_receive_entry )(
                     MP_packet_Prefix **
                   );

/**
 *  This type defines the Multiprocessor Communications
 *  Interface (MPCI) Table.  This table defines the user-provided
 *  MPCI which is a required part of a multiprocessor system.
 *
 *  For non-blocking local operations that become remote operations,
 *  we need a timeout.  This is a per-driver timeout: default_timeout
 */
typedef struct {
  /** This fields contains the timeout for MPCI operations in ticks. */
  uint32_t                   default_timeout;
  /** This field contains the maximum size of a packet supported by this
   *  MPCI layer.  This size places a limit on the size of a message
   *  which can be transmitted over this interface.
   **/
  size_t                     maximum_packet_size;
  /** This field points to the MPCI initialization entry point. */
  MPCI_initialization_entry  initialization;
  /** This field points to the MPCI get packet entry point. */
  MPCI_get_packet_entry      get_packet;
  /** This field points to the MPCI return packet entry point. */
  MPCI_return_packet_entry   return_packet;
  /** This field points to the MPCI send packet entry point. */
  MPCI_send_entry            send_packet;
  /** This field points to the MPCI receive packet entry point. */
  MPCI_receive_entry         receive_packet;
} MPCI_Control;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
