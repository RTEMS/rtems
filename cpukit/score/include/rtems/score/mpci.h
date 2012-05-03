/**
 *  @file  rtems/score/mpci.h
 *
 *  This include file contains all the constants and structures associated
 *  with the MPCI layer.  It provides mechanisms to utilize packets.
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

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/mppkt.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>
#include <rtems/score/watchdog.h>

/**
 *  The following defines the node number used when a broadcast is desired.
 */
#define MPCI_ALL_NODES 0

/**
 *  For packets associated with requests that don't already have a timeout,
 *  use the one specified by this MPCI driver.  The value specified by
 *   the MPCI driver sets an upper limit on how long a remote request
 *   should take to complete.
 */
#define MPCI_DEFAULT_TIMEOUT    0xFFFFFFFF

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

/**
 *  The following defines the type for packet processing routines
 *  invoked by the MPCI Receive server.
 */
typedef void (*MPCI_Packet_processor)( MP_packet_Prefix * );

/**
 *  The following enumerated type defines the list of
 *  internal MP operations.
 */
typedef enum {
  MPCI_PACKETS_SYSTEM_VERIFY  =  0
}   MPCI_Internal_Remote_operations;

/**
 *  The following data structure defines the packet used to perform
 *  remote event operations.
 */
typedef struct {
  /** This field is the general header for all packets. */
  MP_packet_Prefix                 Prefix;
  /** This value specifies the operation. */
  MPCI_Internal_Remote_operations  operation;
  /** This is the maximum number of nodes in the system. It must agree
   *  on all nodes.
   */
  uint32_t                         maximum_nodes;
  /** This field is the maximum number of concurrently existent
   *  globally offered objects.
   */
  uint32_t                         maximum_global_objects;
}    MPCI_Internal_packet;

/**
 *  The following thread queue is used to maintain a list of tasks
 *  which currently have outstanding remote requests.
 */
SCORE_EXTERN Thread_queue_Control _MPCI_Remote_blocked_threads;

/**
 *  The following define the internal pointers to the user's
 *  configuration information.
 */
SCORE_EXTERN MPCI_Control *_MPCI_table;

/**
 *  @brief Pointer to MP Thread Control Block
 *
 *  The following is used to determine when the multiprocessing receive
 *  thread is executing so that a proxy can be allocated instead of
 *  blocking the multiprocessing receive thread.
 */
SCORE_EXTERN Thread_Control *_MPCI_Receive_server_tcb;

/**
 *  The following table contains the process packet routines provided
 *  by each object that supports MP operations.
 */
SCORE_EXTERN MPCI_Packet_processor
               _MPCI_Packet_processors[MP_PACKET_CLASSES_LAST+1];

/**
 *  This routine performs the initialization necessary for this handler.
 *
 *  @param[in] timeout_status is the value which should be returned to
 *             blocking threads when they timeout on a remote operation.
 */
void _MPCI_Handler_initialization(
  uint32_t   timeout_status
);

/**
 *  This routine creates the packet receive server used in MP systems.
 */
void _MPCI_Create_server( void );

/**
 *  This routine initializes the MPCI driver by
 *  invoking the user provided MPCI initialization callout.
 */
void _MPCI_Initialization ( void );

/**
 *  This routine registers the MPCI packet processor for the
 *  designated object class.
 *
 *  @param[in] the_class is the class indicator for packets which will
 *             be processed by @a the_packet_processor method.
 *  @param[in] the_packet_processor is a pointer to a method which is
 *             invoked when packets with @a the_class are received.
 */
void _MPCI_Register_packet_processor(
  MP_packet_Classes      the_class,
  MPCI_Packet_processor  the_packet_processor

);

/**
 *  This function obtains a packet by invoking the user provided
 *  MPCI get packet callout.
 *
 *  @return This method returns a pointer to a MPCI packet which can be
 *          filled in by the caller and used to perform a subsequent
 *          remote operation.
 */
MP_packet_Prefix *_MPCI_Get_packet ( void );

/**
 *  This routine deallocates a packet by invoking the user provided
 *  MPCI return packet callout.
 *
 *  @param[in] the_packet is the MP packet to deallocate.
 */
void _MPCI_Return_packet (
  MP_packet_Prefix *the_packet
);

/**
 *  This routine sends a process packet by invoking the user provided
 *  MPCI send callout.
 *
 *  @param[in] destination is the node which should receive this packet.
 *  @param[in] the_packet is the packet to be sent.
 */
void _MPCI_Send_process_packet (
  uint32_t          destination,
  MP_packet_Prefix *the_packet
);

/**
 *  This routine sends a request packet by invoking the user provided
 *  MPCI send callout.
 *
 *  @param[in] destination is the node which should receive this packet.
 *  @param[in] the_packet is the packet to be sent.
 *  @param[in] extra_state is the extra thread state bits which should be
 *             set in addition to the remote operation pending state.  It
 *             may indicate the caller is blocking on a message queue
 *             operation.
 *
 *  @return This method returns the operation status from the remote node.
 */
uint32_t _MPCI_Send_request_packet (
  uint32_t           destination,
  MP_packet_Prefix  *the_packet,
  States_Control     extra_state
);

/**
 *  This routine sends a response packet by invoking the user provided
 *  MPCI send callout.
 *
 *  @param[in] destination is the node which should receive this packet.
 *  @param[in] the_packet is the packet to be sent.
 */
void _MPCI_Send_response_packet (
  uint32_t          destination,
  MP_packet_Prefix *the_packet
);

/**
 *  This routine receives a packet by invoking the user provided
 *  MPCI receive callout.
 *
 *  @return This method returns the packet received.
 */
MP_packet_Prefix  *_MPCI_Receive_packet ( void );

/**
 *  This routine is responsible for passing @a the_packet to the thread
 *  waiting on the remote operation to complete.  The unblocked thread is
 *  responsible for eventually freeing @a the_packet.
 *
 *  @param[in] the_packet is the response packet to be processed.
 *
 *  @return This method returns a pointer to the thread which was if unblocked
 *          or NULL if the waiting thread no longer exists.
 */
Thread_Control *_MPCI_Process_response (
  MP_packet_Prefix *the_packet
);

/**
 *  This is the server thread which receives and processes all MCPI packets.
 *
 *  @param[in] ignored is the thread argument.  It is not used.
 */
Thread _MPCI_Receive_server(
  uint32_t   ignored
);

/**
 *  This routine informs RTEMS of the availability of an MPCI packet.
 */
void _MPCI_Announce ( void );

/**
 *  This routine performs a remote procedure call so that a
 *  process operation can be performed on another node.
 *
 *  @param[in] operation is the remote operation to perform.
 */
void _MPCI_Internal_packets_Send_process_packet (
   MPCI_Internal_Remote_operations operation
);

/**
 *  _MPCI_Internal_packets_Send_request_packet
 *
 *  This routine performs a remote procedure call so that a
 *  directive operation can be initiated on another node.
 *
 *  This routine is not needed since there are no request
 *  packets to be sent by this manager.
 */

/**
 *  _MPCI_Internal_packets_Send_response_packet
 *
 *  This routine performs a remote procedure call so that a
 *  directive can be performed on another node.
 *
 *  This routine is not needed since there are no response
 *  packets to be sent by this manager.
 */

/**
 *  This routine performs the actions specific to this package for
 *  the request from another node.
 */
void _MPCI_Internal_packets_Process_packet (
  MP_packet_Prefix *the_packet_prefix
);

/**
 *  _MPCI_Internal_packets_Send_object_was_deleted
 *
 *  This routine is invoked indirectly by the thread queue
 *  when a proxy has been removed from the thread queue and
 *  the remote node must be informed of this.
 *
 *  This routine is not needed since there are no objects
 *  deleted by this manager.
 */

/**
 *  _MPCI_Internal_packets_Send_extract_proxy
 *
 *  This routine is invoked when a task is deleted and it
 *  has a proxy which must be removed from a thread queue and
 *  the remote node must be informed of this.
 *
 *  This routine is not needed since there are no objects
 *  deleted by this manager.
 */

/**
 *  This routine is used to obtain a internal threads mp packet.
 */
MPCI_Internal_packet *_MPCI_Internal_packets_Get_packet ( void );

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
