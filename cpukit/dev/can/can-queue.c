/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup CANFDStack
 *
 * @brief This file is part of CAN/CAN FD bus common support
 *        and implements CAN FIFOs and generic hubs/ends
 *        for chip and character driver interface sides.
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

#include <string.h>
#include <dev/can/can-impl.h>

#define RTEMS_CAN_QUEUE_ROUND_ROBIN 1

int rtems_can_queue_fifo_flush_slots( struct rtems_can_queue_fifo *fifo )
{
  struct rtems_can_queue_slot *slot;
  int ret = 0;

  rtems_mutex_lock( &fifo->fifo_lock );
  slot = fifo->head;
  if ( slot ) {
    *fifo->tail = fifo->free_list;
    fifo->free_list = slot;
    fifo->head = NULL;
    fifo->tail = &fifo->head;
    ret |= RTEMS_CAN_FIFOF_INACTIVE;
  }
  rtems_can_queue_fifo_clear_flag( fifo, RTEMS_CAN_FIFOF_FULL );
  rtems_can_queue_fifo_set_flag( fifo, RTEMS_CAN_FIFOF_INACTIVE );
  if ( fifo->out_taken == 0 ) {
    if ( rtems_can_queue_fifo_test_and_set_flag( fifo, RTEMS_CAN_FIFOF_EMPTY ) ) {
      ret |= RTEMS_CAN_FIFOF_EMPTY;
    }
  }
  rtems_mutex_unlock( &fifo->fifo_lock );

  return ret;
}

int rtems_can_queue_fifo_init_slots( struct rtems_can_queue_fifo *fifo )
{
  struct rtems_can_queue_slot *slot;
  int allocated_slot_count = fifo->allocated_slot_count;

  if ( ( fifo->entry == NULL ) || ( allocated_slot_count == 0 ) ) {
    return -1;
  }

  slot = fifo->entry;
  fifo->free_list = slot;
  while ( --allocated_slot_count ) {
    slot = slot->next = (void *)slot + rtems_can_queue_fifo_slot_size(
      fifo->max_data_length
    );
  }
  slot->next = NULL;
  fifo->head = NULL;
  fifo->tail = &fifo->head;
  fifo->out_taken = 0;
  rtems_can_queue_fifo_set_flag( fifo, RTEMS_CAN_FIFOF_EMPTY );

  return 1;
}

void rtems_can_queue_do_edge_decref( struct rtems_can_queue_edge *qedge )
{
  rtems_can_queue_do_edge_decref_body( qedge );
}

int rtems_can_queue_get_inslot(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge **qedgep,
  struct rtems_can_queue_slot **slotp,
  int cmd
)
{
  struct rtems_can_queue_edge *edge;
  int ret = -2;

  edge = rtems_can_queue_first_inedge( qends );
  if ( edge ) {
    if ( rtems_can_queue_fifo_test_flag( &edge->fifo, RTEMS_CAN_FIFOF_BLOCK ) == 0 ) {
      ret = rtems_can_queue_fifo_get_inslot( &edge->fifo, slotp, cmd );
      if ( ret == 0 ) {
        *qedgep = edge;
        RTEMS_DEBUG_PRINT( "cmd = %d found edge %d\n", cmd, edge->edge_num );
        return ret;
      }
    }
    rtems_can_queue_edge_decref( edge );
  }
  *qedgep = NULL;
  RTEMS_DEBUG_PRINT( "cmd = %d failed\n", cmd );

  return ret;
}

int rtems_can_queue_get_inslot_for_prio(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge **qedgep,
  struct rtems_can_queue_slot **slotp,
  const struct can_frame_header *header,
  int cmd,
  int prio
)
{
  struct rtems_can_queue_edge *edge = NULL;
  struct rtems_can_queue_edge *bestedge = NULL;
  int ret = -2;

  rtems_can_queue_for_each_inedge( qends, edge ) {
    if ( rtems_can_queue_fifo_test_flag( &edge->fifo, RTEMS_CAN_FIFOF_BLOCK ) ) {
      continue;
    }

    if ( header != NULL ) {
      if ( !rtems_can_queue_filter_match( &edge->filter, header->can_id, header->flags ) ) {
        continue;
      }
    }

    if ( bestedge != NULL ) {
      if ( bestedge->edge_prio < edge->edge_prio ) {
        if ( edge->edge_prio > prio ) {
          continue;
        }
      } else {
        if ( bestedge->edge_prio <= prio ) {
          continue;
        }
      }

      rtems_can_queue_edge_decref( bestedge );
    }
    bestedge = edge;
    rtems_can_queue_edge_incref( bestedge );
  }
  if ( ( edge = bestedge ) != NULL ) {
    ret = rtems_can_queue_fifo_get_inslot( &edge->fifo, slotp, cmd );
    if ( ret == 0 ) {
      *qedgep = edge;
      RTEMS_DEBUG_PRINT( "cmd = %d prio = %d found edge %d\n", cmd, prio,
                         edge->edge_num );
      return ret;
    }
    rtems_can_queue_edge_decref( bestedge );
  }
  *qedgep = NULL;
  RTEMS_DEBUG_PRINT( "cmd=%d prio=%d failed\n", cmd, prio );

  return ret;
}

int rtems_can_queue_test_inslot( struct rtems_can_queue_ends *qends )
{
  struct rtems_can_queue_edge *edge;
  int ret = -1;

  edge = rtems_can_queue_first_inedge( qends );
  if ( edge ) {
    if ( rtems_can_queue_fifo_test_flag( &edge->fifo, RTEMS_CAN_FIFOF_BLOCK ) == 0 ) {
      if ( rtems_can_queue_fifo_test_flag( &edge->fifo, RTEMS_CAN_FIFOF_FULL ) == 0 ) {
        /* There is empty space */
        rtems_can_queue_edge_decref( edge );
        return 0;
      }
    }
  }

  return ret;
}

int rtems_can_queue_put_inslot(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge *qedge,
  struct rtems_can_queue_slot *slot
)
{
  int ret;

  ret = rtems_can_queue_fifo_put_inslot( &qedge->fifo, slot );
  if ( ret ) {
    rtems_can_queue_activate_edge( qends, qedge );
    rtems_can_queue_notify_output_ends( qedge, RTEMS_CAN_QUEUE_NOTIFY_PROC );
  }

  rtems_can_queue_edge_decref( qedge );
  RTEMS_DEBUG_PRINT( "For edge %d returned %d\n", qedge->edge_num, ret );

  return ret;
}

int rtems_can_queue_abort_inslot(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge *qedge,
  struct rtems_can_queue_slot *slot
)
{
  (void) qends;

  int ret;

  ret = rtems_can_queue_fifo_abort_inslot( &qedge->fifo, slot );
  if ( ret ) {
    rtems_can_queue_notify_output_ends( qedge, RTEMS_CAN_QUEUE_NOTIFY_SPACE );
  }

  rtems_can_queue_edge_decref( qedge );
  RTEMS_DEBUG_PRINT( "For edge %d returned %d\n", qedge->edge_num, ret );

  return ret;
}

int rtems_can_queue_filter_frame_to_edges(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge *src_edge,
  struct can_frame *frame,
  unsigned int flags2add
)
{
  struct rtems_can_queue_edge *edge;
  struct rtems_can_queue_slot *slot;
  int destnr = 0;
  int is_txerr = 0;
  int ret;

  RTEMS_DEBUG_PRINT( "For frame ID 0x%08lx and flags 0x%02x\n",
                     frame->header.can_id, frame->header.flags );

  is_txerr = ( ( flags2add & CAN_FRAME_TXERR ) == 0 ) ? 0 : 1;

  rtems_can_queue_for_each_inedge( qends, edge ) {
    if ( rtems_can_queue_fifo_test_flag( &edge->fifo, RTEMS_CAN_FIFOF_BLOCK ) ) {
      continue;
    }

    if (
      src_edge != NULL &&
      is_txerr == 0 &&
      edge->output_ends == src_edge->input_ends
    ) {
      flags2add |= CAN_FRAME_ECHO;
    } else {
      flags2add &= ~CAN_FRAME_ECHO;
    }

    if ( !rtems_can_queue_filter_match( &edge->filter, frame->header.can_id,
                               frame->header.flags | flags2add ) ) {
      continue;
    }

    ret = rtems_can_queue_fifo_get_inslot( &edge->fifo, &slot, 0 );
    if ( ret == 0 ) {
      if ( frame->header.dlen > edge->fifo.max_data_length ) {
        rtems_can_queue_fifo_abort_inslot( &edge->fifo, slot );
        continue;
      }

      slot->frame = *frame;
      slot->frame.header.flags |= flags2add;
      if ( slot->frame.header.flags & CAN_FRAME_ERR ) {
        /* Invalidate CAN identifier if this is an error frame. This way
         * the user will know this is not a regular frame even without
         * flag check.
         */
        slot->frame.header.can_id |= CAN_ERR_ID_TAG;
      }

      destnr++;
      ret = rtems_can_queue_fifo_put_inslot( &edge->fifo, slot );
      if ( ret ) {
        rtems_can_queue_activate_edge( qends, edge );
        rtems_can_queue_notify_output_ends(
          edge,
          RTEMS_CAN_QUEUE_NOTIFY_PROC
        );
      }

      if ( is_txerr == 1 ) {
        /* Send TX error frame to just one edge and skip all others. */
        rtems_can_queue_edge_decref( edge );
        break;
      }
    }
  }
  RTEMS_DEBUG_PRINT( "Sent frame ID %d to %d edges\n", frame->header.can_id,
                     destnr );

  return destnr;
}

int rtems_can_queue_test_outslot(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge **qedgep,
  struct rtems_can_queue_slot **slotp
)
{
  struct rtems_can_queue_edge *edge;
  int prio = RTEMS_CAN_QUEUE_PRIO_NR - 1;
  int ret;

  do {
    rtems_mutex_lock( &qends->ends_lock );
    while ( !TAILQ_EMPTY( &qends->active[prio] ) ) {
      edge = TAILQ_FIRST( &qends->active[prio] );
      if ( !rtems_can_queue_fifo_test_flag( &edge->fifo, RTEMS_CAN_FIFOF_DEAD ) ) {
        int out_ready;
        rtems_can_queue_edge_incref( edge );
        rtems_mutex_unlock( &qends->ends_lock );
        *qedgep = edge;
        RTEMS_DEBUG_PRINT( "Found edge %d\n", edge->edge_num );
        ret = rtems_can_queue_fifo_test_outslot( &edge->fifo, slotp );
        if ( ( ret >= 0 ) && !RTEMS_CAN_QUEUE_ROUND_ROBIN ) {
          return ret;
        }
        rtems_mutex_lock( &qends->ends_lock );
        rtems_mutex_lock( &edge->fifo.fifo_lock );
        out_ready = rtems_can_queue_fifo_out_is_ready_unprotected(
          &edge->fifo
        );
        if ( !out_ready || RTEMS_CAN_QUEUE_ROUND_ROBIN ) {
          if ( edge->peershead != NULL ) {
            TAILQ_REMOVE( edge->peershead, edge, activepeers );
          }
          if ( out_ready ) {
            TAILQ_INSERT_HEAD(
              &qends->active[edge->edge_prio],
              edge,
              activepeers
            );
            edge->peershead = &qends->active[edge->edge_prio];
          } else {
            rtems_can_queue_fifo_set_flag(
              &edge->fifo,
              RTEMS_CAN_FIFOF_INACTIVE
            );
            TAILQ_INSERT_TAIL( &qends->idle, edge, activepeers );
            edge->peershead = &qends->idle;
          }
        }
        rtems_mutex_unlock( &edge->fifo.fifo_lock );
        rtems_mutex_unlock( &qends->ends_lock );
        if ( ret >= 0 ) {
          return ret;
        }
        rtems_can_queue_edge_decref( edge );
        rtems_mutex_lock( &qends->ends_lock );
      } else {
        rtems_mutex_lock( &edge->fifo.fifo_lock );
        rtems_can_queue_fifo_set_flag( &edge->fifo, RTEMS_CAN_FIFOF_INACTIVE );
        rtems_can_queue_edge_to_idle_unprotected( qends, edge );
        rtems_mutex_unlock( &edge->fifo.fifo_lock );
      }
    }
    rtems_mutex_unlock( &qends->ends_lock );
  } while ( --prio >= 0 );

  *qedgep = NULL;
  return -1;
}

int rtems_can_queue_pending_outslot_prio(
  struct rtems_can_queue_ends *qends,
  int prio_min
) {
  struct rtems_can_queue_edge *edge;
  int prio;

  for ( prio = RTEMS_CAN_QUEUE_PRIO_NR; --prio >= prio_min; ) {
    rtems_mutex_lock( &qends->ends_lock );
    while ( !TAILQ_EMPTY( &qends->active[prio] ) ) {
      edge = TAILQ_FIRST( &qends->active[prio] );
      if ( !rtems_can_queue_fifo_test_flag( &edge->fifo, RTEMS_CAN_FIFOF_DEAD ) ) {
        rtems_can_queue_edge_incref( edge );
        rtems_mutex_unlock( &qends->ends_lock );
        if ( rtems_can_queue_fifo_out_is_ready_unprotected( &edge->fifo ) ) {
          return prio;
        }
        rtems_mutex_lock( &qends->ends_lock );
        rtems_mutex_lock( &edge->fifo.fifo_lock );
        if ( !rtems_can_queue_fifo_out_is_ready_unprotected( &edge->fifo ) ) {
          rtems_can_queue_fifo_set_flag(
            &edge->fifo,
            RTEMS_CAN_FIFOF_INACTIVE
          );
          rtems_can_queue_edge_to_idle_unprotected( qends, edge );
        }
        rtems_mutex_unlock( &edge->fifo.fifo_lock );
        rtems_mutex_unlock( &qends->ends_lock );
        rtems_can_queue_edge_decref( edge );
        rtems_mutex_lock( &qends->ends_lock );
      } else {
        rtems_mutex_lock( &edge->fifo.fifo_lock );
        rtems_can_queue_fifo_set_flag( &edge->fifo, RTEMS_CAN_FIFOF_INACTIVE );
        rtems_can_queue_edge_to_idle_unprotected( qends, edge );
        rtems_mutex_unlock( &edge->fifo.fifo_lock );
      }
    }
    rtems_mutex_unlock( &qends->ends_lock );
  }
  return -1;
}

int rtems_can_queue_free_outslot(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge *qedge,
  struct rtems_can_queue_slot *slot
)
{
  int ret;

  ret = rtems_can_queue_fifo_free_outslot( &qedge->fifo, slot );
  if ( ret & RTEMS_CAN_FIFOF_EMPTY ) {
    rtems_can_queue_notify_input_ends( qedge, RTEMS_CAN_QUEUE_NOTIFY_EMPTY );
  }

  if ( ret & RTEMS_CAN_FIFOF_FULL ) {
    rtems_can_queue_notify_input_ends( qedge, RTEMS_CAN_QUEUE_NOTIFY_SPACE );
  }
  if ( ret & RTEMS_CAN_FIFOF_INACTIVE ) {
    rtems_mutex_lock( &qends->ends_lock );
    rtems_mutex_lock( &qedge->fifo.fifo_lock );
    if ( !rtems_can_queue_fifo_out_is_ready_unprotected( &qedge->fifo ) ) {
      rtems_can_queue_fifo_set_flag( &qedge->fifo, RTEMS_CAN_FIFOF_INACTIVE );
      rtems_can_queue_edge_to_idle_unprotected( qends, qedge );
    }
    rtems_mutex_unlock( &qedge->fifo.fifo_lock );
    rtems_mutex_unlock( &qends->ends_lock );
  }
  rtems_can_queue_edge_decref( qedge );
  RTEMS_DEBUG_PRINT( "For edge %d returned %d\n", qedge->edge_num, ret );

  return ret;
}

int rtems_can_queue_push_back_outslot(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge *qedge,
  struct rtems_can_queue_slot *slot
)
{
  int ret;

  ret = rtems_can_queue_fifo_again_outslot( &qedge->fifo, slot );

  rtems_mutex_lock( &qends->ends_lock );
  rtems_mutex_lock( &qedge->fifo.fifo_lock );
  if ( rtems_can_queue_fifo_out_is_ready_unprotected( &qedge->fifo ) ) {
    if ( qedge->peershead != NULL ) {
      TAILQ_REMOVE( qedge->peershead, qedge, activepeers );
    }
    TAILQ_INSERT_TAIL( &qends->active[qedge->edge_prio], qedge, activepeers );
    qedge->peershead = &qends->active[qedge->edge_prio];
    rtems_can_queue_fifo_clear_flag( &qedge->fifo, RTEMS_CAN_FIFOF_INACTIVE );
  }
  rtems_mutex_unlock( &qedge->fifo.fifo_lock );
  rtems_mutex_unlock( &qends->ends_lock );

  rtems_can_queue_edge_decref( qedge );

  return ret;
}

int rtems_can_queue_flush( struct rtems_can_queue_edge *qedge )
{
  int ret;

  ret = rtems_can_queue_fifo_flush_slots( &qedge->fifo );
  if ( ret != 0 ) {
    rtems_can_queue_notify_input_ends( qedge, RTEMS_CAN_QUEUE_NOTIFY_EMPTY );
    rtems_can_queue_notify_input_ends( qedge, RTEMS_CAN_QUEUE_NOTIFY_SPACE );
    rtems_mutex_lock( &qedge->output_ends->ends_lock );
    rtems_mutex_lock( &qedge->fifo.fifo_lock );
    if ( !rtems_can_queue_fifo_out_is_ready_unprotected( &qedge->fifo ) &&
         ( qedge->output_ends != NULL ) ) {
      rtems_can_queue_fifo_set_flag( &qedge->fifo, RTEMS_CAN_FIFOF_INACTIVE );
      rtems_can_queue_edge_to_idle_unprotected(qedge->output_ends, qedge);
    }
    rtems_mutex_unlock( &qedge->fifo.fifo_lock );
    rtems_mutex_unlock( &qedge->output_ends->ends_lock );
  }
  RTEMS_DEBUG_PRINT( "For edge %d returned %d\n", qedge->edge_num, ret );
  return ret;
}

int rtems_can_queue_ends_init( struct rtems_can_queue_ends *qends )
{
  int i;

  for ( i = RTEMS_CAN_QUEUE_PRIO_NR; --i >= 0; ) {
    TAILQ_INIT( &qends->active[i] );
  }
  TAILQ_INIT( &qends->idle );
  TAILQ_INIT( &qends->inlist );
  TAILQ_INIT( &qends->outlist );
  rtems_mutex_init( &qends->ends_lock, "ends_lock" );
  return 0;
}

int rtems_can_queue_connect_edge(
  struct rtems_can_queue_edge *qedge,
  struct rtems_can_queue_ends *input_ends,
  struct rtems_can_queue_ends *output_ends
)
{
  if ( qedge == NULL ) {
    return -1;
  }
  RTEMS_DEBUG_PRINT( "Connecting edge %d\n", qedge->edge_num );
  rtems_can_queue_edge_incref( qedge );

  if ( input_ends < output_ends ) {
    rtems_mutex_lock( &input_ends->ends_lock );
    rtems_mutex_lock( &output_ends->ends_lock );
  } else {
    rtems_mutex_lock( &output_ends->ends_lock );
    if ( output_ends != input_ends ) {
      rtems_mutex_lock( &input_ends->ends_lock );
    }
  }

  rtems_mutex_lock( &qedge->fifo.fifo_lock );
  qedge->input_ends=input_ends;
  TAILQ_INSERT_TAIL( &input_ends->inlist, qedge, input_peers );
  qedge->output_ends=output_ends;
  TAILQ_INSERT_TAIL( &output_ends->outlist, qedge, output_peers );
  TAILQ_INSERT_TAIL( &output_ends->idle, qedge, activepeers );
  qedge->peershead = &output_ends->idle;
  rtems_mutex_unlock( &qedge->fifo.fifo_lock );

  if ( input_ends < output_ends ) {
    rtems_mutex_unlock( &output_ends->ends_lock );
    rtems_mutex_unlock( &input_ends->ends_lock );
  } else {
    if ( output_ends != input_ends ) {
      rtems_mutex_unlock( &input_ends->ends_lock );
    }
    rtems_mutex_unlock( &output_ends->ends_lock );
  }

  rtems_can_queue_notify_both_ends( qedge, RTEMS_CAN_QUEUE_NOTIFY_ATTACH );

  if ( rtems_can_queue_fifo_test_and_set_flag(
      &qedge->fifo, RTEMS_CAN_FIFOF_READY
    ) ) {
    rtems_can_queue_edge_decref( qedge );
  }
  return 0;
}

int rtems_can_queue_disconnect_edge( struct rtems_can_queue_edge *qedge )
{
  int ret;
  struct rtems_can_queue_ends *input_ends, *output_ends;

  input_ends=qedge->input_ends;
  output_ends=qedge->output_ends;

  if ( input_ends && output_ends ) {
    if ( input_ends < output_ends ) {
      rtems_mutex_lock( &input_ends->ends_lock );
      rtems_mutex_lock( &output_ends->ends_lock );
    } else {
      rtems_mutex_lock( &output_ends->ends_lock );
      if ( output_ends != input_ends ) {
        rtems_mutex_lock( &input_ends->ends_lock );
      }
    }
  } else {
    RTEMS_DEBUG_PRINT( "Called with not fully connected edge\n" );
    if ( input_ends ) {
      rtems_mutex_lock( &input_ends->ends_lock );
    } else if ( output_ends ) {
      rtems_mutex_lock( &output_ends->ends_lock );
    }
  }

  rtems_mutex_lock( &qedge->fifo.fifo_lock );
  if ( atomic_load( &qedge->edge_used ) == 0 ) {
    if ( qedge->output_ends ) {
      if ( qedge->peershead != NULL ) {
        TAILQ_REMOVE( qedge->peershead, qedge, activepeers );
        qedge->peershead = NULL;
      }
      TAILQ_REMOVE( &qedge->output_ends->outlist, qedge, output_peers );
      qedge->output_ends=NULL;
    }
    if ( qedge->input_ends ) {
      TAILQ_REMOVE( &qedge->input_ends->inlist, qedge, input_peers );
      qedge->input_ends=NULL;
    }
    ret = 1;
  } else {
    ret = -1;
  }

  rtems_mutex_unlock( &qedge->fifo.fifo_lock );

  if ( input_ends && output_ends ) {
    if ( input_ends < output_ends ) {
      rtems_mutex_unlock( &output_ends->ends_lock );
      rtems_mutex_unlock( &input_ends->ends_lock );
    } else {
      if ( output_ends != input_ends ) {
        rtems_mutex_unlock( &input_ends->ends_lock );
      }
      rtems_mutex_unlock( &output_ends->ends_lock );
    }
  } else {
    if ( input_ends ) {
      rtems_mutex_unlock( &input_ends->ends_lock );
    } else if ( output_ends ) {
      rtems_mutex_unlock( &output_ends->ends_lock );
    }
  }

  RTEMS_DEBUG_PRINT( "Edge %d returned %d\n", qedge->edge_num, ret );
  return ret;
}

void rtems_can_queue_block_inlist( struct rtems_can_queue_ends *qends )
{
  struct rtems_can_queue_edge *edge;

  rtems_can_queue_for_each_inedge( qends, edge ) {
    rtems_can_queue_fifo_set_flag( &edge->fifo, RTEMS_CAN_FIFOF_BLOCK );
  }
}

void rtems_can_queue_block_outlist( struct rtems_can_queue_ends *qends )
{
  struct rtems_can_queue_edge *edge;

  rtems_can_queue_for_each_outedge( qends, edge ) {
    rtems_can_queue_fifo_set_flag( &edge->fifo, RTEMS_CAN_FIFOF_BLOCK );
  }
}

int rtems_can_queue_ends_kill_inlist(
  struct rtems_can_queue_ends *qends,
  int send_rest
)
{
  struct rtems_can_queue_edge *edge;

  rtems_can_queue_for_each_inedge( qends, edge ) {
    rtems_can_queue_notify_both_ends(
      edge,
      RTEMS_CAN_QUEUE_NOTIFY_DEAD_WANTED
    );
    if ( send_rest ) {
      rtems_can_queue_edge_incref( edge );
      if ( !rtems_can_queue_fifo_test_and_set_flag(
          &edge->fifo,
          RTEMS_CAN_FIFOF_FREEONEMPTY
        ) ) {
        if ( !rtems_can_queue_fifo_test_flag( &edge->fifo, RTEMS_CAN_FIFOF_EMPTY ) ) {
          continue;
        }
        if ( !rtems_can_queue_fifo_test_and_clear_fl(
            &edge->fifo,
            RTEMS_CAN_FIFOF_FREEONEMPTY
          ) ) {
          continue;
        }
      }
      rtems_can_queue_edge_decref( edge );
    }
  }

  return TAILQ_EMPTY( &qends->inlist ) ? 0 : 1;
}

int rtems_can_queue_ends_kill_outlist( struct rtems_can_queue_ends *qends )
{
  struct rtems_can_queue_edge *edge;

  rtems_can_queue_for_each_outedge( qends, edge ) {
    rtems_can_queue_notify_both_ends(
      edge,
      RTEMS_CAN_QUEUE_NOTIFY_DEAD_WANTED
    );
  }

  return TAILQ_EMPTY( &qends->outlist ) ? 0 : 1;
}

int rtems_can_queue_ends_flush_inlist( struct rtems_can_queue_ends *qends )
{
  struct rtems_can_queue_edge *edge;

  rtems_can_queue_for_each_inedge( qends, edge ) {
    rtems_can_queue_flush( edge );
  }
  return 0;
}

int rtems_can_queue_ends_flush_outlist( struct rtems_can_queue_ends *qends )
{
  struct rtems_can_queue_edge *edge;

  rtems_can_queue_for_each_outedge( qends, edge ) {
    rtems_can_queue_flush( edge );
  }
  return 0;
}
