/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup CANFDStack
 *
 * @brief This file is part of CAN/CAN FD bus common support
 *        and implements CAN FIFOs and generic hubs/ends
 *        for chip and caracter driver interface sides.
 *
 * Implementation is based on original LinCAN - Linux CAN bus driver
 * Part of OrtCAN project https://ortcan.sourceforge.net/
 */

/*
 * Copyright (C) 2023-2024 Michal Lenc <michallenc@seznam.cz>
 * Copyright (C) 2002-2009 DCE FEE CTU Prague
 * Copyright (C) 2002-2024 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __DEV_CAN_CAN_QUEUE_H
#define __DEV_CAN_CAN_QUEUE_H

#include <stdatomic.h>
#include <inttypes.h>
#include <sys/queue.h>
#include <rtems.h>
#include <rtems/timespec.h>
#include <rtems/status-checks.h>
#include <rtems/thread.h>

#include <dev/can/can-frame.h>
#include <dev/can/can-filter.h>

/**
 * @brief This structure represents one CAN message slot in the CAN FIFO queue.
 */
struct rtems_can_queue_slot {
  /**
   * @brief Pointer to next/younger slot.
   */
  struct rtems_can_queue_slot *next;
  /**
   * @brief Space for flags and optional command describing action
   *  associated with slot data
   */
  atomic_uint slot_flags;
  /**
   * @brief This member holds @ref can_frame structure representing one
   *  CAN frame/message.
   */
  struct can_frame frame;
};

/**
 * @brief Macro for command associated with allocated slot.
 */
#define RTEMS_CAN_SLOTF_CMD  ( 0x00ff )

/**
 * @brief This structure represents CAN FIFO queue. It is implemented as
 * a single linked list of slots prepared for processing. The empty slots
 * are stored in single linked list.
 *
 */
struct rtems_can_queue_fifo {
  /**
   * @brief This field holds global flags describing state of the FIFO.
   */
  atomic_uint fifo_flags;
  /**
   * @brief This member holds the number of elements in the fifo.
   *
   * This is only the number of slots taken on the out side which
   * processing has not been finished by chip driver (or copy to user
   * in another direction). These has to be accounted because FIFI is
   * not empty even that there are no slots on the linked list. Only when
   * all slots taken by out side are processed then it can be signaled
   * as empty.
   */
  unsigned int out_taken;
  /**
   * @brief This member holds futher description of error condition.
   */
  unsigned long error_code;
  /**
   * @brief This member holds the pointer to the FIFO head, oldest slot.
   */
  struct rtems_can_queue_slot *head;
  /**
   * @brief This member holds the pointer to the location, where pointer to
   *   newly inserted slot should be added.
   */
  struct rtems_can_queue_slot **tail;
  /**
   * @brief This member holds the pointer to list of the free slots
   *   associated with queue.
   */
  struct rtems_can_queue_slot *free_list;
  /**
   * @brief This member holds the pointer to the memory allocated for
   *   the list slots.
   */
  struct rtems_can_queue_slot *entry;
  /**
   * @brief This member holds maximum data length of one CAN frame.
   */
  int max_data_length;
  /**
   * @brief This member holds the number of allocated slots.
   */
  int allocated_slot_count;
  /**
   * @brief This member holds the lock to ensure atomicity of slot
   * manipulation operations.
   */
  rtems_mutex fifo_lock;
};

/**
 * @brief This is set when FIFO is scheduled for destruction.
 */
#define RTEMS_CAN_FIFOF_DESTROY      ( 1 << 15 )
/**
 * @brief This is set when some error condition occurs.
 */
#define RTEMS_CAN_FIFOF_ERROR        ( 1 << 14 )
/**
 * @brief This defines that error should lead to the FIFO block state.
 */
#define RTEMS_CAN_FIFOF_ERR2BLOCK    ( 1 << 13 )
/**
 * @brief This state blocks insertion of the next messages.
 */
#define RTEMS_CAN_FIFOF_BLOCK        ( 1 << 12 )
/**
 * @brief This indicates attempt to acquire new slot, when FIFO is full.
 */
#define RTEMS_CAN_FIFOF_OVERRUN      ( 1 << 11 )
/**
 * @brief This indicates FIFO full state.
 */
#define RTEMS_CAN_FIFOF_FULL         ( 1 << 10 )
/**
 * @brief This indicates no allocated slot in the FIFO.
 */
#define RTEMS_CAN_FIFOF_EMPTY        ( 1 << 9 )
/**
 * @brief This is used when FIFO is beeing destroyed.
 */
#define RTEMS_CAN_FIFOF_DEAD         ( 1 << 8 )
/**
 * @brief This indicates FIFO is inactive.
 */
#define RTEMS_CAN_FIFOF_INACTIVE     ( 1 << 7 )
/**
 * @brief This indicates FIFO was freed when being empty.
 */
#define RTEMS_CAN_FIFOF_FREEONEMPTY  ( 1 << 6 )
/**
 * @brief This indicates FIFO is ready.
 */
#define RTEMS_CAN_FIFOF_READY        ( 1 << 5 )
/**
 * @brief This indicates pending notification.
 */
#define RTEMS_CAN_FIFOF_NOTIFYPEND   ( 1 << 4 )

/**
 * @brief This function frees all ready slots from the FIFO.
 *
 * The caller should be prepared to handle situations, when some
 * slots are held by input or output side slots processing.
 * These slots cannot be flushed or their processing interrupted.
 *
 * @param fifo     Pointer to the FIFO structure.
 *
 * @return The nonzero value indicates that queue has not been
 * empty before the function call.
 *
 */
int rtems_can_queue_fifo_flush_slots( struct rtems_can_queue_fifo *fifo );

/**
 * @brief This function initializes slot chain of one CAN FIFO.
 *
 * The caller should be prepared to handle situations, when some
 * slots are held by input or output side slots processing.
 * These slots cannot be flushed or their processing interrupted.
 *
 * @param fifo     Pointer to the FIFO structure.
 *
 * @return The negative value indicates that there is no memory
 * to allocate space for the requested number of the slots.
 *
 */
int rtems_can_queue_fifo_init_slots( struct rtems_can_queue_fifo *fifo );

struct rtems_can_queue_edge;
TAILQ_HEAD( rtems_can_queue_edges_list, rtems_can_queue_edge );

/**
 * @brief This structure represents one direction connection from messages
 * source ( @ref input_ends) to message consumer ( @ref output_ends) fifo
 * ends hub. The edge contains @ref rtems_can_queue_fifo for message fifo
 * implementation-
 *
 */
struct rtems_can_queue_edge {
  /**
   * @brief This member holds place where primitive @ref rtems_can_queue_fifo
   * FIFO is located.
   */
  struct rtems_can_queue_fifo fifo;
  /**
   * @brief This member holds place where primitive @ref rtems_can_filter is
   * located.
   */
  struct rtems_can_filter filter;
  /**
   * @brief This member holds the lists of all peers FIFOs connected by
   * their input side ( @ref input_ends ) to the same terminal
   * ( @ref rtems_can_queue_ends ).
   */
  TAILQ_ENTRY( rtems_can_queue_edge ) input_peers;
  /**
   * @brief This member holds the lists of all peers FIFOs connected by their
   * output side ( @ref output_ends ) to the same terminal
   * ( @ref rtems_can_queue_ends )
   */
  TAILQ_ENTRY( rtems_can_queue_edge ) output_peers;
  /**
   * @brief This member holds the lists of peers FIFOs connected by their
   * output side ( @ref output_ends ) to the same terminal
   * ( @ref rtems_can_queue_ends ) with same priority and active state.
   */
  TAILQ_ENTRY( rtems_can_queue_edge ) activepeers;
  /**
   * @brief This member holds the pointer to the activepeers head.
   */
  struct rtems_can_queue_edges_list *peershead;
  /**
   * @brief This member holds the pointer to the FIFO input side terminal
   * (  @ref rtems_can_queue_ends ).
   */
  struct rtems_can_queue_ends *input_ends;
  /**
   * @brief This member holds the pointer to the FIFO output side terminal
   * (  @ref rtems_can_queue_ends ).
   */
  struct rtems_can_queue_ends *output_ends;
  /**
   * @brief This member holds the atomic usage counter, mainly used for
   * safe destruction of the edge.
   */
  atomic_uint edge_used;
  /**
   * @brief This member holds the the assigned queue priority from the
   * range 0 to @ref RTEMS_CAN_QUEUE_PRIO_NR - 1
   */
  int edge_prio;
  /**
   * @brief This member holds the edge sequential number intended for
   * debugging purposes only.
   */
  int edge_num;
};

struct rtems_can_queue_ends;
TAILQ_HEAD( rtems_can_queue_ends_list, rtems_can_queue_ends );

/**
 * @brief This structure represents place to connect edges to for CAN
 * communication entity. The zero, one or more incoming and outgoing edges
 * can be connected to this structure.
 */
struct rtems_can_queue_ends {
  /**
   * @brief This member holds the array of the lists of active edges
   * directed to the ends structure.
   */
  struct rtems_can_queue_edges_list active[RTEMS_CAN_QUEUE_PRIO_NR];
  /**
   * @brief This member holds the list of the edges directed to the ends
   * structure with empty FIFOs.
   */
  struct rtems_can_queue_edges_list idle;
  /**
   * @brief This member holds the list of outgoing edges input sides.
   */
  struct rtems_can_queue_edges_list inlist;
  /**
   * @brief This member holds the list of all incoming edges output sides.
   * Each of there edges is listed on one of @ref active or @ref idle lists.
   */
  struct rtems_can_queue_edges_list outlist;
  /**
   * @brief This member holds the lock synchronizing operations between
   * threads accessing the ends.
   */
  rtems_mutex ends_lock;
  /**
   * @brief pointer to notify procedure. The next state changes are notified.
   *
   *  @ref RTEMS_CAN_QUEUE_NOTIFY_EMPTY (out->in call) - all slots are
   *  processed by FIFO out side.
   *
   *  @ref RTEMS_CAN_QUEUE_NOTIFY_SPACE (out->in call) - full state negated
   *  => there is space for new message.
   *
   *  @ref RTEMS_CAN_QUEUE_NOTIFY_PROC  (in->out call) - empty state negated
   *  => out side is requested to process slots.
   *
   *  @ref RTEMS_CAN_QUEUE_NOTIFY_NOUSR (both) - notify, that the last user
   *  has released the edge usage called with some lock to prevent edge
   *  disappear.
   *
   *  @ref RTEMS_CAN_QUEUE_NOTIFY_DEAD  (both) - edge is in progress of
   *  deletion.
   *
   *  @ref RTEMS_CAN_QUEUE_NOTIFY_ATTACH (both) - new edge has been attached
   *  to end.
   *
   * @param qends The pointer to @ref rtems_can_queue_ends structure.
   * @param qedge The pointer to @ref rtems_can_queue_edge structure.
   * @param what  Integer specifying notification changes.
   */
  void (*notify)(
    struct rtems_can_queue_ends *qends,
    struct rtems_can_queue_edge *qedge,
    int what
  );
  /**
   * @brief This member is used to chain ends wanting for postponed
   * destruction.
   */
  TAILQ_ENTRY( rtems_can_queue_ends ) dead_peers;
};

/**
 * @brief out-> in all slots are processed by FIFO out side.
 */
#define RTEMS_CAN_QUEUE_NOTIFY_EMPTY  ( 1 )
/**
 * @brief out -> in - full state negated => there is space for new message.
 */
#define RTEMS_CAN_QUEUE_NOTIFY_SPACE  ( 2 )
/**
 * @brief out -> in - full state negated => there is space for new message.
 */
#define RTEMS_CAN_QUEUE_NOTIFY_PROC   ( 3 )
/**
 * @brief Notify, that the last user has released the edge usage called
 *  with some lock to prevent edge disappear.
 */
#define RTEMS_CAN_QUEUE_NOTIFY_NOUSR  ( 4 )
/**
 * @brief Edge is in progress of deletion.
 */
#define RTEMS_CAN_QUEUE_NOTIFY_DEAD   ( 5 )
/**
 * @brief Edge should be deleted.
 */
#define RTEMS_CAN_QUEUE_NOTIFY_DEAD_WANTED ( 6 )
/**
 * @brief Edge is in progress of deletion.
 */
#define RTEMS_CAN_QUEUE_NOTIFY_ATTACH ( 7 )

/**
 * @brief This function finds one outgoing edge and allocates slot from it.
 *
 * Function looks for the first non-blocked outgoing edge in @qends structure
 * and tries to allocate slot from it.
 *
 * @param qends  Ends structure belonging to calling communication object.
 * @param qedgep Place to store pointer to found edge.
 * @param slotp  Place to store pointer to  allocated slot.
 * @param cmd    Command type for slot.
 *
 * @return If there is no usable edge or there is no free slot in edge
 * negative value is returned.
 *
 */
int rtems_can_queue_get_inslot(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge **qedgep,
  struct rtems_can_queue_slot **slotp,
  int cmd
);

/**
 * @brief This function finds best outgoing edge and slot for given priority.
 *
 * Function looks for the non-blocked outgoing edge accepting messages
 * with given ID. If edge is found, slot is allocated from that edge.
 * The edges with non-zero mask are preferred over edges open to all messages.
 * If more edges with mask accepts given message ID, the edge with
 * highest priority below or equal to required priority is selected.
 *
 * @param qends            Ends structure belonging to calling communication
 * object.
 * @param qedgep           Place to store pointer to found edge.
 * @param slotp            Place to store pointer to  allocated slot.
 * @param cmd              Command type for slot.
 * @param can_frame_header Pointer to CAN frame header.
 * @param prio             Optional priority of message,
 *
 * @return If there is no usable edge or there is no free slot in edge
 * negative value is returned.
 *
 */
int rtems_can_queue_get_inslot_for_prio(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge **qedgep,
  struct rtems_can_queue_slot **slotp,
  const struct can_frame_header *header,
  int cmd,
  int prio
);

/**
 * @brief This function tests whether there is a free space in any outgoing
 * edge.
 *
 * Function looks for the first non-blocked outgoing edge in @qends structure
 * with free space for slot.
 *
 * @param qends  Ends structure belonging to calling communication object.
 *
 * @return 0 if there is usable edge with free space available, -1
 * otherwise.
 *
 */
int rtems_can_queue_test_inslot( struct rtems_can_queue_ends *qends );

/**
 * @brief This function schedules filled slot for processing
 *
 * Puts slot previously acquired by @ref rtems_can_queue_get_inslot or
 * @ref rtems_can_queue_get_inslot_for_prio function call into FIFO queue and
 * activates edge processing if needed.
 *
 * @param qends  Ends structure belonging to calling communication object.
 * @param qedge  Edge the slot belongs to.
 * @param slot   Pointer to the preprared slot.
 *
 * @return Positive value informs, that activation of output end
 * has been necessary
 *
 */
int rtems_can_queue_put_inslot(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge *qedge,
  struct rtems_can_queue_slot *slot
);

/**
 * @brief This function aborts preparation of the message in the slot
 *
 * Frees slot previously acquired by @ref rtems_can_queue_get_inslot or
 * @ref rtems_can_queue_get_inslot_for_prio function call. Used when message
 * copying into slot fails.
 *
 * @param qends  Ends structure belonging to calling communication object.
 * @param qedge  Edge the slot belongs to.
 * @param slot   Pointer to the preprared slot.
 *
 * @return Positive value informs, that queue full state has been negated.
 *
 */
int rtems_can_queue_abort_inslot(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge *qedge,
  struct rtems_can_queue_slot *slot
);

/**
 * @brief This function sends message into all edges which accept its ID.
 *
 * Sends message to all outgoing edges connected to the given ends, which
 * accepts message communication ID.
 *
 * @param qends      Ends structure belonging to calling communication object
 * @param src_edge   Optional source edge for echo detection
 * @param frame      Pointer to CAN frame.
 * @param flags2add  Optional additional CAN Frame flags.
 *
 * @return Returns number of edges message has been send to.
 *
 */
int rtems_can_queue_filter_frame_to_edges(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge *src_edge,
  struct can_frame *frame,
  unsigned int flags2add
);

/**
 * @brief This function tests and retrieves ready slot for given ends.
 *
 * Function takes highest priority active incoming edge and retrieves
 * oldest ready slot from it.
 *
 * @param qends  Ends structure belonging to calling communication object.
 * @param qedgep Place to store pointer to found edge.
 * @param slotp  Place to store pointer to received slot.
 *
 * @return Negative value informs, that there is no ready output
 * slot for given ends. Positive value is equal to the command
 * slot has been allocated by the input side.
 *
 */
int rtems_can_queue_test_outslot(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge **qedgep,
  struct rtems_can_queue_slot **slotp
);

/**
 * @brief This function tests ready outslot with minimum priority
 *
 * Function searches for ready slot in active incoming edge. The difference
 * from @ref rtems_can_queue_test_outslot function is that this function does
 * not retreive the slot from FIFO, it just checks its existence. This can be
 * used to determined whether there is a slot with higher priority class
 * in the infrastruce.
 *
 * @param qends    Ends structure belonging to calling communication object.
 * @param prio_min Minimum slot priority to be considered
 *
 * @return Negative value informs, that there is no ready output
 * slot for given ends and minimum priority. Positive value informs about
 * the available slot priority.
 *
 */
int rtems_can_queue_pending_outslot_prio(
  struct rtems_can_queue_ends *qends,
  int prio_min
);

/**
 * @brief This function frees processed output slot.
 *
 * Function releases processed slot previously acquired by
 * @ref rtems_can_queue_test_outslot function call.
 *
 * @param qends  Ends structure belonging to calling communication object.
 * @param qedge  Edge the slot belongs to.
 * @param slot   Pointer to the processed slot.
 *
 * @return Informs if input side has been notified
 * to know about change of edge state
 *
 */
int rtems_can_queue_free_outslot(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge *qedge,
  struct rtems_can_queue_slot *slot
);

/**
 * @brief This function reschedules output slot to process it again later.
 *
 * Function reschedules slot previously acquired by
 * @ref rtems_can_queue_test_outslot function call for second time processing.
 *
 * @param qends  Ends structure belonging to calling communication object.
 * @param qedge  Edge the slot belongs to.
 * @param slot   Pointer to the processed slot.
 *
 * @return Function cannot fail.
 *
 */
int rtems_can_queue_push_back_outslot(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge *qedge,
  struct rtems_can_queue_slot *slot
);

/**
 * @brief This function flushes all ready slots in the edge.
 *
 * Tries to flush all allocated slots from the edge, but there could
 * exist some slots associated to edge which are processed by input
 * or output side and cannot be flushed at this moment
 *
 * @param qedge  Pointer to the edge.
 *
 * @return The nonzero value indicates, that queue has not been
 * empty before the function call
 *
 */
int rtems_can_queue_flush( struct rtems_can_queue_edge *qedge );

/**
 * @brief This function disconnects edge from communication entities.
 *
 * @param qedge Pointer to edge.
 *
 * @return Negative value means, that edge is used by somebody
 * other and cannot be disconnected. Operation has to be delayed.
 *
 */
int rtems_can_queue_disconnect_edge( struct rtems_can_queue_edge *qedge );

/**
 * @brief This function connects edge between two communication entities.
 *
 * @param qedge   Pointer to edge.
 * @param input_ends  Pointer to ends the input of the edge should be
 * connected to.
 * @param output_ends Pointer to ends the output of the edge should be
 * connected to.
 *
 * @return Negative value informs about failed operation.
 *
 */
int rtems_can_queue_connect_edge(
  struct rtems_can_queue_edge *qedge,
  struct rtems_can_queue_ends *input_ends,
  struct rtems_can_queue_ends *output_ends
);

/**
 * @brief This function implements subsystem independent routine to initialize
 * ends state.
 *
 * @param qends  Pointer to the end.
 *
 * @return This fucntion cannot fail.
 *
 */
int rtems_can_queue_ends_init( struct rtems_can_queue_ends *qends );

/**
 * @brief This function blocks slot allocation of all outgoing edges of
 * specified ends
 *
 * @param qends Pointer to ends structure.
 *
 * @return None.
 *
 */
void rtems_can_queue_block_inlist( struct rtems_can_queue_ends *qends );

/**
 * @brief This function blocks slot allocation of all incoming edges of
 * specified ends
 *
 * @param qends Pointer to ends structure.
 *
 * @return None.
 *
 */
void rtems_can_queue_block_outlist( struct rtems_can_queue_ends *qends );

/**
 * @brief This function sends request to die to all outgoing edges
 *
 * @param qends     Pointer to ends structure.
 * @param send_rest Select, whether already allocated slots should be
 * processed by FIFO output side.
 *
 * @return Non-zero value means, that not all edges could be immediately
 * disconnected and that ends structure memory release has to be delayed.
 *
 */
int rtems_can_queue_ends_kill_inlist(
  struct rtems_can_queue_ends *qends,
  int send_rest
);

/**
 * @brief This function sends request to die to all incomming edges
 *
 * @param qends Pointer to ends structure.
 *
 * @return Non-zero value means, that not all edges could be immediately
 * disconnected and that ends structure memory release has to be delayed.
 *
 */
int rtems_can_queue_ends_kill_outlist( struct rtems_can_queue_ends *qends );

/**
 * @brief This function flushes all messages in incoming edges
 *
 * @param qends: pointer to ends structure
 *
 * Return Value: Negative value informs about unsuccessful result
 */
int rtems_can_queue_ends_flush_inlist( struct rtems_can_queue_ends *qends );

/**
 * @brief This function flushes all messages in outgoing edges
 *
 * @param qends Pointer to ends structure.
 *
 * @return Negative value informs about unsuccessful result.
 *
 */
int rtems_can_queue_ends_flush_outlist( struct rtems_can_queue_ends *qends );

/**
 * @brief This function decrements the edge user counter
 *
 * @param qedge Pointer to the edge to be disconnected.
 *
 * @return None
 *
 */
void rtems_can_queue_do_edge_decref( struct rtems_can_queue_edge *edge );

/**
 * @brief This function disconnects the edge
 *
 * @param qedge Pointer to the edge to be disconnected.
 *
 * @return None
 *
 */
void rtems_can_queue_edge_do_dead( struct rtems_can_queue_edge *qedge );

/**
 * @brief This function initializes one CAN FIFO.
 *
 * @param fifo                  Pointer to the FIFO structure.
 * @param allocated_slot_count  Number of requested slots.
 * @param max_data_length       Maximum size of data in one slot/frame.
 *
 * @return The negative value indicates, that there is no memory
 * to allocate space for the requested number of the slots.
 *
 */
int rtems_can_queue_fifo_init_kern(
  struct rtems_can_queue_fifo *fifo,
  int allocated_slot_count,
  int max_data_length
);

/**
 * @brief This function frees slots allocated for CAN FIFO.
 *
 * @param fifo     Pointer to the FIFO structure.
 *
 * @return This function should not fail.
 *
 */
int rtems_can_queue_fifo_done_kern( struct rtems_can_queue_fifo *fifo );

/**
 * @brief This function allocates new edge structure.
 *
 * @param allocated_slot_count Required number of slots in the newly allocated
 * edge structure.
 * @param max_data_length Maximul data length of one CAN frame.
 *
 * @return Pointer to rtems_can_queue_edge structure on success, NULL on error.
 *
 */
struct rtems_can_queue_edge *rtems_can_queue_new_edge_kern(
  int allocated_slot_countm,
  int max_data_length
);

/**
 * @brief This function waits for all ends to TX their messages.
 *
 * @param qends     Pointer to ends structure.
 * @param ts        Absolute time againts CLOCK_MONOTONIC that informs
 *                  rtems_can_queue_sync_wait_kern how long to wait.
 *
 * @return 0 on success, -ETIME in case of timeout.
 *
 */
int rtems_can_queue_ends_sync_all_kern(
  struct rtems_can_queue_ends *qends,
  struct timespec *ts
);

/**
 * @brief This function waits for all slots processing
 *
 * @param qends   Ends structure belonging to calling communication object.
 * @param qedge   Pointer to edge.
 * @param nowait  True if semaphore should not wait
 * @param timeout Number of clock ticks to wait for semaphore. Passing zero
 * indicates an infinite timeout.
 *
 * @return Positive value indicates, that edge empty state has been reached.
 * Negative or zero value informs the semaphore timeouted.
 *
 */
int rtems_can_queue_sync_wait_kern(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge *qedge,
  bool nowait,
  rtems_interval timeout
);

/**
 * @brief This function provides finalizing of the ends structure for clients.
 *
 * @param qends     Pointer to ends structure.
 * @param nonblock  Flag indicating that user does not want to wait for
 *    processing of all remaining messages.
 *
 * @return Function should be designed such way to not fail.
 *
 */
int rtems_can_queue_ends_dispose_kern(
  struct rtems_can_queue_ends *qends,
  bool nonblock
);

/**
 * @brief This function provides initialization of kernel queue side.
 *
 * @return Zero on success.
 *
 */
int rtems_can_queue_kern_initialize( void );

#endif /* __DEV_CAN_CAN_QUEUE_H */
