/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup CANFDStack
 *
 * @brief This file is part of CAN/CAN FD bus common support
 *        and implements internal functions for CAN queues handling
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

#ifndef __DEV_CAN_CAN_IMPL_H
#define __DEV_CAN_CAN_IMPL_H

#include <stdatomic.h>
#include <inttypes.h>
#include <sys/queue.h>
#include <rtems.h>
#include <rtems/timespec.h>
#include <rtems/status-checks.h>
#include <rtems/thread.h>

#include <dev/can/can-frame.h>
#include <dev/can/can-filter.h>
#include <dev/can/can-queue.h>

/**
 * @brief   This function atomically tests FIFO flag to input flag.
 *
 * @param   fifo  Pointer to CAN FIFO Queue.
 * @param   flag  Required flag to be tested.
 *
 * @return  True if FIFO has the flag.
 * @return  False otherwise.
 *
 */
static inline int rtems_can_queue_fifo_test_flag(
  struct rtems_can_queue_fifo *fifo,
  unsigned int flag
)
{
  return ( atomic_load( &fifo->fifo_flags ) & flag ) == flag;
}

/**
 * @brief   This function atomically sets FIFO flag.
 *
 * @param   fifo  Pointer to CAN FIFO Queue.
 * @param   flag  Flag to be set.
 *
 * @return  None.
 *
 */
static inline void rtems_can_queue_fifo_set_flag(
  struct rtems_can_queue_fifo *fifo,
  int flag
)
{
  atomic_fetch_or( &fifo->fifo_flags, flag );
}

/**
 * @brief   This function atomically sets FIFO flag.
 *
 * @param   fifo  Pointer to CAN FIFO Queue.
 * @param   flag  Flag to be set.
 *
 * @return  None.
 *
 */
static inline void rtems_can_queue_fifo_clear_flag(
  struct rtems_can_queue_fifo *fifo,
  int flag
)
{
  atomic_fetch_and( &fifo->fifo_flags, ~flag );
}

/**
 * @brief   This function atomically tests and sets FIFO flag
 *
 * @param   fifo  Pointer to CAN FIFO Queue.
 * @param   flag  Flag to be tested and set.
 *
 * @return  True if FIFO has the flag.
 * @return  False otherwise.
 *
 */
static inline int rtems_can_queue_fifo_test_and_set_flag(
  struct rtems_can_queue_fifo *fifo,
  int flag
)
{
  return ( atomic_fetch_or( &fifo->fifo_flags, flag ) & flag ) ? 1 : 0;
}

/**
 * @brief   This function atomically tests and clears FIFO flag
 *
 * @param   fifo  Pointer to CAN FIFO Queue.
 * @param   flag  Flag to be tested and cleared.
 *
 * @return  True if FIFO has the flag.
 * @return  False otherwise.
 *
 */
static inline int rtems_can_queue_fifo_test_and_clear_fl(
  struct rtems_can_queue_fifo *fifo,
  int flag
)
{
  return ( atomic_fetch_and( &fifo->fifo_flags, ~flag ) & flag ) ? 1 : 0;
}

/**
 * @brief   This fumction tests if there is ready slot to be taken by
 *  @ref rtems_can_queue_fifo_test_outslot .
 *
 * @param   fifo  Pointer to CAN FIFO Queue.
 *
 * @return  True if FIFO has slot which .
 * @return  False otherwise.
 *
 */
static inline int rtems_can_queue_fifo_out_is_ready_unprotected(
  struct rtems_can_queue_fifo *fifo
)
{
  return ( ( fifo->head ) != NULL ) ? 1 : 0;
}

/**
 * @brief   This function checks whether CAN Frame identifier and flags match
 *          with queue filter.
 *
 *  It is used to filter frame to edges based on CAN identifier and flag
 *  match with queue filter.
 *
 * @param   filter Pointer to @ref rtems_can_filter structure.
 * @param   id     CAN frame identifier.
 * @param   flags  CAN frame flags.
 *
 * @return  True if match, false otherwise.
 *
 */
static inline bool rtems_can_queue_filter_match(
  struct rtems_can_filter *filter,
  uint32_t id,
  uint32_t flags )
{
  return ( ( filter->id^id ) & filter->id_mask ) ||
         ( ( filter->flags^flags ) & filter->flags_mask ) ? false : true;
}

/**
 * @brief   This function allocates slot for the input of one CAN message
 *
 * @param   fifo    Pointer to the FIFO structure
 * @param   slotp   Pointer to location to store pointer to the allocated slot.
 * @param   cmd     Optional command associated with allocated slot.
 *
 * @return  The function returns negative value if there is no
 *  free slot in the FIFO queue. Zero if free slot was found.
 */
static inline int rtems_can_queue_fifo_get_inslot(
  struct rtems_can_queue_fifo *fifo,
  struct rtems_can_queue_slot **slotp,
  int cmd
)
{
  struct rtems_can_queue_slot *slot;
  rtems_mutex_lock( &fifo->fifo_lock );

  /* Get the first free slot slot from free_list */
  if ( ( slot = fifo->free_list ) == NULL ) {
    rtems_can_queue_fifo_set_flag( fifo, RTEMS_CAN_FIFOF_OVERRUN );
    rtems_can_queue_fifo_set_flag( fifo, RTEMS_CAN_FIFOF_FULL );
    rtems_mutex_unlock( &fifo->fifo_lock );
    *slotp = NULL;
    return -1;
  }

  /* Adjust free slot list */
  if ( ( fifo->free_list = slot->next ) == NULL ) {
    rtems_can_queue_fifo_set_flag( fifo, RTEMS_CAN_FIFOF_FULL );
  }

  rtems_mutex_unlock( &fifo->fifo_lock );
  *slotp = slot;
  slot->slot_flags = cmd & RTEMS_CAN_SLOTF_CMD;

  return 0;
}

/**
 * @brief   This function releases slot for the further processing.
 *
 * @param   fifo  Pointer to the FIFO structure
 * @param   slot  Pointer to the slot previously acquired by
 *  @ref rtems_can_queue_fifo_get_inslot .
 *
 * @return  The nonzero return value indicates, that the queue was empty
 *  before call to the function. The caller should wake-up output side
 *  of the queue.
 *
 */
static inline int rtems_can_queue_fifo_put_inslot(
  struct rtems_can_queue_fifo *fifo,
  struct rtems_can_queue_slot *slot
)
{
  int ret = 0;

  slot->next = NULL;
  rtems_mutex_lock( &fifo->fifo_lock );
  *fifo->tail = slot;
  fifo->tail = &slot->next;

  if ( rtems_can_queue_fifo_test_and_clear_fl( fifo, RTEMS_CAN_FIFOF_EMPTY ) ) {
    /* Fifo has been empty before put */
    ret = RTEMS_CAN_FIFOF_EMPTY;
  }

  if ( rtems_can_queue_fifo_test_and_clear_fl( fifo, RTEMS_CAN_FIFOF_INACTIVE ) ) {
    /* Fifo has been empty before put */
    ret |= RTEMS_CAN_FIFOF_INACTIVE;
  }

  if ( rtems_can_queue_fifo_test_and_clear_fl( fifo, RTEMS_CAN_FIFOF_OVERRUN ) ) {
    /* RX overflow occured, report it with frame flag */
    slot->frame.header.flags |= CAN_FRAME_FIFO_OVERFLOW;
  }

  rtems_mutex_unlock( &fifo->fifo_lock );

  return ret;
}

/**
 * @brief   This function releases and aborts slot.
 *
 * @param   fifo   Pointer to the FIFO structure.
 * @param   slot  Pointer to the slot previously acquired by
 *  @ref rtems_can_queue_fifo_get_inslot .
 *
 * @return  The nonzero value indicates that fifo was full.
 *
 */
static inline int rtems_can_queue_fifo_abort_inslot(
  struct rtems_can_queue_fifo *fifo,
  struct rtems_can_queue_slot *slot
)
{
  int ret = 0;

  rtems_mutex_lock( &fifo->fifo_lock );
  slot->next = fifo->free_list;
  fifo->free_list = slot;
  if ( rtems_can_queue_fifo_test_and_clear_fl( fifo, RTEMS_CAN_FIFOF_FULL ) ) {
    ret = RTEMS_CAN_FIFOF_FULL;
  }

  rtems_mutex_unlock( &fifo->fifo_lock );

  return ret;
}

/**
 * @brief   This function tests and gets ready slot from the FIFO.
 *
 * @param   fifo   Pointer to the FIFO structure.
 * @param   slotp Pointer to location to store pointer to the oldest slot
 * from the FIFO.
 *
 * @return  The negative value indicates, that queue is empty.
 *  The positive or zero value represents command stored into slot by
 *  the call to the function @ref rtems_can_queue_fifo_get_inslot .
 *  The successfully acquired FIFO output slot has to be released by
 *  the call @ref rtems_can_queue_fifo_free_outslot or
 *  @ref rtems_can_queue_fifo_again_outslot
 *
 */
static inline int rtems_can_queue_fifo_test_outslot(
  struct rtems_can_queue_fifo *fifo,
  struct rtems_can_queue_slot **slotp
)
{
  int cmd;
  struct rtems_can_queue_slot *slot;

  rtems_mutex_lock( &fifo->fifo_lock );
  if ( ( slot = fifo->head ) == NULL ) {
    rtems_mutex_unlock( &fifo->fifo_lock );
    *slotp = NULL;
    return -1;
  }

  if ( ( fifo->head=slot->next ) == NULL ) {
    fifo->tail = &fifo->head;
  }

  fifo->out_taken += 1;

  rtems_mutex_unlock( &fifo->fifo_lock );

  *slotp = slot;
  cmd = slot->slot_flags;

  return ( cmd & RTEMS_CAN_SLOTF_CMD );
}

/**
 * @brief   This function frees FIFO slot after processing.
 *
 * @param   fifo   Pointer to the FIFO structure.
 * @param   slot  Pointer to the slot previously acquired by
 * rtems_can_queue_fifo_test_outslot().
 *
 * @return The returned value informs about FIFO state change.
 *  The mask %RTEMS_CAN_FIFOF_FULL indicates, that the FIFO was full before
 *  the function call. The mask %RTEMS_CAN_FIFOF_EMPTY informs, that last
 *  ready slot has been processed.
 *
 */
static inline int rtems_can_queue_fifo_free_outslot(
  struct rtems_can_queue_fifo *fifo,
  struct rtems_can_queue_slot *slot
)
{
  int ret = 0;

  rtems_mutex_lock( &fifo->fifo_lock );
  slot->next = fifo->free_list;
  fifo->free_list = slot;
  if ( rtems_can_queue_fifo_test_and_clear_fl( fifo, RTEMS_CAN_FIFOF_FULL ) ) {
    ret = RTEMS_CAN_FIFOF_FULL;
  }

  fifo->out_taken -= 1;

  if ( ( fifo->head ) == NULL ) {
    rtems_can_queue_fifo_set_flag( fifo, RTEMS_CAN_FIFOF_INACTIVE );
    ret |= RTEMS_CAN_FIFOF_INACTIVE;
    if ( fifo->out_taken == 0 ) {
      rtems_can_queue_fifo_set_flag( fifo, RTEMS_CAN_FIFOF_EMPTY );
      ret |= RTEMS_CAN_FIFOF_EMPTY;
    }
  }

  rtems_mutex_unlock( &fifo->fifo_lock );

  return ret;
}

/**
 * @brief   This function returns back FIFO slot to postpone its processing.
 *
 * @param   fifo   Pointer to the FIFO structure.
 * @param   slot  Pointer to the slot previously acquired by
 * rtems_can_queue_fifo_test_outslot().
 *
 * @return  This function returns %RTEMS_CAN_FIFOF_INACTIV if there
  *         has not been ready slot before the call.
 *
 */
static inline int rtems_can_queue_fifo_again_outslot(
  struct rtems_can_queue_fifo *fifo,
  struct rtems_can_queue_slot *slot
)
{
  int ret = 0;

  rtems_mutex_lock( &fifo->fifo_lock );
  if ( ( slot->next = fifo->head ) == NULL ) {
    fifo->tail = &slot->next;
    ret = RTEMS_CAN_FIFOF_INACTIVE;
  }

  fifo->out_taken -= 1;

  fifo->head = slot;
  rtems_mutex_unlock( &fifo->fifo_lock );

  return ret;
}

/**
 * @brief   This function calculates size of FIFO slot
 *
 * @param   max_data_length Maximum CAN frame data length.
 *
 * @return  Returns size of one FIFO slot
 *
 */
static inline int rtems_can_queue_fifo_slot_size( int max_data_length )
{
  return RTEMS_ALIGN_UP(
    offsetof( struct rtems_can_queue_slot, frame.data ) + max_data_length,
    RTEMS_ALIGNOF(struct rtems_can_queue_slot)
  );
}

/**
 * @brief   This function provides request to send notification to the
 * input ends.
 *
 * @param   qedge Pointer to the edge structure.
 * @param   what  Notification type.
 *
 * @return None
 *
 */
static inline void rtems_can_queue_notify_input_ends(
  struct rtems_can_queue_edge *qedge,
  int what
)
{
  if ( qedge->input_ends ) {
    if ( qedge->input_ends->notify ) {
      qedge->input_ends->notify( qedge->input_ends, qedge,what );
    }
  }
}

/**
 * @brief   This function provides request to send notification to the
 * output ends.
 *
 * @param   qedge Pointer to the edge structure.
 * @param   what  Notification type.
 *
 * @return None
 *
 */
static inline void rtems_can_queue_notify_output_ends(
  struct rtems_can_queue_edge *qedge,
  int what
)
{
  if ( qedge->output_ends ) {
    if ( qedge->output_ends->notify ) {
      qedge->output_ends->notify( qedge->output_ends, qedge, what );
    }
  }
}

/**
 * @brief   This function provides request to send notification to both ends.
 *
 * @param   qedge Pointer to the edge structure.
 * @param   what  Notification type.
 *
 * @return None
 *
 */
static inline void rtems_can_queue_notify_both_ends(
  struct rtems_can_queue_edge *qedge,
  int what
)
{
  rtems_can_queue_notify_input_ends( qedge, what );
  rtems_can_queue_notify_output_ends( qedge, what );
}

/**
 * @brief   This function marks output end of the edge as active.
 *
 * Function call moves output side of the edge from idle onto active edges
 * list. This function has to be called with edge reference count held.
 * that is same as for most of other edge functions.
 *
 * @param   qedge  Pointer to the edge structure.
 * @param   input_ends Input side of the edge
 *
 * @return None
 *
 */
static inline void rtems_can_queue_activate_edge(
  struct rtems_can_queue_ends *input_ends,
  struct rtems_can_queue_edge *qedge
)
{
  (void) input_ends;

  struct rtems_can_queue_ends *output_ends = qedge->output_ends;

  if ( qedge->edge_prio >= RTEMS_CAN_QUEUE_PRIO_NR ) {
    qedge->edge_prio = RTEMS_CAN_QUEUE_PRIO_NR - 1;
  }

  if ( output_ends != NULL ) {
    rtems_mutex_lock( &output_ends->ends_lock );
    rtems_mutex_lock( &qedge->fifo.fifo_lock );
    if ( qedge->peershead != &output_ends->active[qedge->edge_prio] ) {
      if ( qedge->peershead != NULL ) {
        TAILQ_REMOVE( qedge->peershead, qedge, activepeers );
      }

      TAILQ_INSERT_HEAD(
        &output_ends->active[qedge->edge_prio],
        qedge, activepeers
      );
      qedge->peershead = &output_ends->active[qedge->edge_prio];
    }
    rtems_mutex_unlock( &qedge->fifo.fifo_lock );
    rtems_mutex_unlock( &output_ends->ends_lock );
  }
}

static inline void rtems_can_queue_edge_to_idle_unprotected(
  struct rtems_can_queue_ends *output_ends,
  struct rtems_can_queue_edge *qedge
)
{
  if ( qedge->peershead != NULL ) {
    TAILQ_REMOVE( qedge->peershead, qedge, activepeers );
  }
  TAILQ_INSERT_TAIL( &output_ends->idle, qedge, activepeers );
  qedge->peershead = &output_ends->idle;
}

/**
 * @brief  This function increments edge reference count
 *
 * @param  edge  Pointer to the edge structure
 *
 * @return None
 *
 */
static inline void rtems_can_queue_edge_incref(
  struct rtems_can_queue_edge *edge
)
{
  atomic_fetch_add( &edge->edge_used, 1 );
}

static inline void rtems_can_queue_do_edge_decref_body(
  struct rtems_can_queue_edge *edge
)
{
  int dead_fl = 0;
  struct rtems_can_queue_ends *input_ends=edge->input_ends;
  struct rtems_can_queue_ends *output_ends=edge->output_ends;

  if ( input_ends < output_ends ) {
    rtems_mutex_lock( &input_ends->ends_lock );
    rtems_mutex_lock( &output_ends->ends_lock );
    if ( atomic_fetch_sub( &edge->edge_used, 1 ) == 1 ) {
      dead_fl = !rtems_can_queue_fifo_test_and_set_flag(
        &edge->fifo,
        RTEMS_CAN_FIFOF_DEAD
      );
    }
    rtems_mutex_unlock( &output_ends->ends_lock );
    rtems_mutex_unlock( &input_ends->ends_lock );
  } else {
    rtems_mutex_lock( &output_ends->ends_lock );
    if ( output_ends != input_ends ) {
      rtems_mutex_lock( &input_ends->ends_lock );
    }
    if ( atomic_fetch_sub( &edge->edge_used, 1 ) == 1 ) {
      dead_fl = !rtems_can_queue_fifo_test_and_set_flag(
        &edge->fifo,
        RTEMS_CAN_FIFOF_DEAD
      );
    }
    if ( output_ends != input_ends ) {
      rtems_mutex_unlock( &input_ends->ends_lock );
    }
    rtems_mutex_unlock( &output_ends->ends_lock );
  }

  if ( dead_fl ) {
    rtems_can_queue_edge_do_dead( edge );
  }
}

/**
 * @brief   This function decrements edge reference count.
 *
 * This function has to be called without lock held for both ends of edge.
 * If reference count drops to 0, function rtems_can_queue_edge_do_dead()
 * is called.
 *
 * @param   edge  Pointer to the edge structure
 *
 * @return None
 *
 */
static inline void rtems_can_queue_edge_decref(
  struct rtems_can_queue_edge *edge
)
{
  unsigned int x;

  x = atomic_load_explicit( &edge->edge_used, memory_order_relaxed );

  do {
    if ( x <= 1 ) {
      return rtems_can_queue_do_edge_decref( edge );
    }
  } while( !atomic_compare_exchange_strong(&edge->edge_used, &x, x-1 ) );
}

static inline struct rtems_can_queue_edge *rtems_can_queue_first_inedge(
  struct rtems_can_queue_ends *qends
)
{
  struct rtems_can_queue_edge *edge;

  rtems_mutex_lock( &qends->ends_lock );
  edge = TAILQ_FIRST( &qends->inlist );

  while ( edge && rtems_can_queue_fifo_test_flag( &edge->fifo, RTEMS_CAN_FIFOF_DEAD ) ) {
    edge = TAILQ_NEXT( edge, input_peers );
  }
  if ( edge )
    rtems_can_queue_edge_incref( edge );

  rtems_mutex_unlock( &qends->ends_lock );
  return edge;
}


static inline struct rtems_can_queue_edge *rtems_can_queue_next_inedge(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge *edge
)
{
  struct rtems_can_queue_edge *next;

  rtems_mutex_lock( &qends->ends_lock );
  next = TAILQ_NEXT( edge, input_peers );

  while ( next && rtems_can_queue_fifo_test_flag( &next->fifo, RTEMS_CAN_FIFOF_DEAD ) ) {
    next = TAILQ_NEXT( next, input_peers );
  }
  if ( next )
    rtems_can_queue_edge_incref( next );

  rtems_mutex_unlock( &qends->ends_lock );
  rtems_can_queue_edge_decref( edge );
  return next;
}

#define rtems_can_queue_for_each_inedge( qends, edge ) \
  for ( \
    edge = rtems_can_queue_first_inedge( qends ); \
    edge; \
    edge = rtems_can_queue_next_inedge( qends, edge ) )

static inline
struct rtems_can_queue_edge *can_queue_first_outedge(
  struct rtems_can_queue_ends *qends
)
{
  struct rtems_can_queue_edge *edge;

  rtems_mutex_lock( &qends->ends_lock );
  edge = TAILQ_FIRST( &qends->outlist );

  while ( edge && rtems_can_queue_fifo_test_flag( &edge->fifo, RTEMS_CAN_FIFOF_DEAD ) ) {
    edge = TAILQ_NEXT( edge, output_peers );
  }
  if ( edge )
    rtems_can_queue_edge_incref( edge );

  rtems_mutex_unlock( &qends->ends_lock );
  return edge;
}

static inline
struct rtems_can_queue_edge *rtems_can_queue_next_outedge(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge *edge
)
{
  struct rtems_can_queue_edge *next;

  rtems_mutex_lock( &qends->ends_lock );
  next = TAILQ_NEXT( edge, output_peers );

  while ( next && rtems_can_queue_fifo_test_flag( &next->fifo, RTEMS_CAN_FIFOF_DEAD ) ) {
    next = TAILQ_NEXT( next, output_peers );
  }
  if ( next )
    rtems_can_queue_edge_incref( next );

  rtems_mutex_unlock( &qends->ends_lock );
  rtems_can_queue_edge_decref( edge );
  return next;
}

#define rtems_can_queue_for_each_outedge( qends, edge ) \
  for ( \
    edge = can_queue_first_outedge( qends ); \
    edge; \
    edge = rtems_can_queue_next_outedge( qends, edge ) )


#endif /* __DEV_CAN_CAN_IMPL_H */
