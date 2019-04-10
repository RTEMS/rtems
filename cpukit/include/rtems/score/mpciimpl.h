/**
 * @file
 *
 * @ingroup RTEMSScoreMPCI
 *
 * @brief MPCI Layer Implementation
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_MPCIIMPL_H
#define _RTEMS_SCORE_MPCIIMPL_H

#include <rtems/score/mpci.h>
#include <rtems/score/status.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreMPCI
 *
 * @{
 */

/**
 *  For packets associated with requests that don't already have a timeout,
 *  use the one specified by this MPCI driver.  The value specified by
 *   the MPCI driver sets an upper limit on how long a remote request
 *   should take to complete.
 */
#define MPCI_DEFAULT_TIMEOUT    0xFFFFFFFF

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
extern Thread_queue_Control _MPCI_Remote_blocked_threads;

/**
 *  The following define the internal pointers to the user's
 *  configuration information.
 */
extern MPCI_Control *_MPCI_table;

/**
 *  @brief Pointer to MP thread control block.
 *
 *  The following is used to determine when the multiprocessing receive
 *  thread is executing so that a proxy can be allocated instead of
 *  blocking the multiprocessing receive thread.
 */
extern Thread_Control *_MPCI_Receive_server_tcb;

/**
 *  The following table contains the process packet routines provided
 *  by each object that supports MP operations.
 */
extern MPCI_Packet_processor
_MPCI_Packet_processors[ MP_PACKET_CLASSES_LAST + 1 ];

/**
 * @brief Registers the MPCI packet processor for the designated object class.
 *
 * @param the_class The class indicator for packets which will
 *            be processed by @a the_packet_processor method.
 * @param the_packet_processor Pointer to a method which is
 *            invoked when packets with @a the_class are received.
 */
void _MPCI_Register_packet_processor(
  MP_packet_Classes      the_class,
  MPCI_Packet_processor  the_packet_processor

);

/**
 * @brief Obtains a packet by invoking the user provided
 *          MPCI get packet callout.
 *
 * @return Returns a pointer to a MPCI packet which can be
 *          filled in by the caller and used to perform a subsequent
 *          remote operation.
 */
MP_packet_Prefix *_MPCI_Get_packet ( void );

/**
 * @brief Deallocates a packet.
 *
 * This routine deallocates a packet by invoking the user provided
 * MPCI return packet callout.
 *
 * @param[out] the_packet The MP packet to deallocate.
 */
void _MPCI_Return_packet (
  MP_packet_Prefix *the_packet
);

/**
 * @brief Sends a process packet.
 *
 * This routine sends a process packet by invoking the user provided
 * MPCI send callout.
 *
 * @param destination The node which should receive this packet.
 * @param the_packet The packet to be sent.
 */
void _MPCI_Send_process_packet (
  uint32_t          destination,
  MP_packet_Prefix *the_packet
);

/**
 * @brief Sends a request packet.
 *
 * This routine sends a request packet by invoking the user provided
 * MPCI send callout.
 *
 * @param destination The node which should receive this packet.
 * @param the_packet The packet to be sent.
 * @param extra_state The extra thread state bits which should be
 *            set in addition to the remote operation pending state.  It
 *            may indicate the caller is blocking on a message queue
 *            operation.
 *
 * @ret This method returns the operation status from the remote node.
 */
Status_Control _MPCI_Send_request_packet(
  uint32_t          destination,
  MP_packet_Prefix *the_packet,
  States_Control    extra_state
);

/**
 * @brief Sends a response packet.
 *
 * This routine sends a response packet by invoking the user provided
 * MPCI send callout.
 *
 * @param destination The node which should receive this packet.
 * @param the_packet The packet to be sent.
 */
void _MPCI_Send_response_packet (
  uint32_t          destination,
  MP_packet_Prefix *the_packet
);

/**
 * @brief Receives a packet.
 *
 * This routine receives a packet by invoking the user provided
 * MPCI receive callout.
 *
 * @return This method returns the packet received.
 */
MP_packet_Prefix  *_MPCI_Receive_packet ( void );

/**
 * @brief Passes a packet to the thread.
 *
 * This routine is responsible for passing @a the_packet to the thread
 * waiting on the remote operation to complete.  The unblocked thread is
 * responsible for eventually freeing @a the_packet.
 *
 * @param the_packet is the response packet to be processed.
 *
 * @retval pointer This method returns a pointer to the thread which was unblocked
 * @retval NULL The waiting thread no longer exists.
 */
Thread_Control *_MPCI_Process_response (
  MP_packet_Prefix *the_packet
);

/**
 * @brief Receives and processes all packets.
 *
 * This is the server thread which receives and processes all MCPI packets.
 *
 * @param ignored The thread argument.  It is not used.
 */
void _MPCI_Receive_server(
  Thread_Entry_numeric_type ignored
);

/**
 * @brief Announces the availability of a packet.
 *
 * This routine informs RTEMS of the availability of an MPCI packet.
 */
void _MPCI_Announce ( void );

/**
 * @brief Performs a process on another node.
 *
 * This routine performs a remote procedure call so that a
 * process operation can be performed on another node.
 *
 * @param operation The remote operation to perform.
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
 * @brief Performs requested action from another node.
 *
 * This routine performs the actions specific to this package for
 * the request from another node.
 *
 * @param the_packet_prefix The packet prefix for this method.
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
 * @brief Obtains an internal thread.
 *
 * This routine is used to obtain an internal threads MP packet.
 *
 * @retval pointer The pointer to the obtained packet.
 * @retval NULL Something went wrong.
 */
MPCI_Internal_packet *_MPCI_Internal_packets_Get_packet ( void );

/**
 * @brief Checks if the packet class is valid.
 *
 * @param the_packet_class The packet class to perform the validation on.
 *
 * @retval true @a the_packet_class is valid.
 * @retval false @a the_packet_class is not valid.
 *
 * @note Check for lower bounds (MP_PACKET_CLASSES_FIRST ) is unnecessary
 *       because this enum starts at lower bound of zero.
 */

RTEMS_INLINE_ROUTINE bool _Mp_packet_Is_valid_packet_class (
  MP_packet_Classes the_packet_class
)
{
  return ( the_packet_class <= MP_PACKET_CLASSES_LAST );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
