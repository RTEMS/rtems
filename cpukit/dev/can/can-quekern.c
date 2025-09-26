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

#include <string.h>
#include <stdlib.h>

#include <rtems/malloc.h>
#include <rtems/timespec.h>

#include <dev/can/can.h>
#include <dev/can/can-helpers.h>
#include <dev/can/can-devcommon.h>
#include <dev/can/can-impl.h>

#define CAN_DEAD_FUNC_PRIORITY 120

#define CAN_KERN_INITIALIZED ( 1 << 0 )

static atomic_uint edge_num_cnt;

static rtems_mutex can_queue_dead_func_lock = RTEMS_MUTEX_INITIALIZER(
  "can_queue_dead_func_lock"
);
static rtems_binary_semaphore dead_func_sem = RTEMS_BINARY_SEMAPHORE_INITIALIZER(
  "can_queue_dead_func_sem"
);

static struct rtems_can_queue_ends_list can_queue_dead_ends;
static struct rtems_can_queue_edges_list can_queue_dead_edges;

static atomic_uint kern_flags;

static inline struct rtems_can_queue_edge *can_queue_dead_edges_cut_first( void )
{
  struct rtems_can_queue_edge *edge;
  rtems_mutex_lock( &can_queue_dead_func_lock );
  if ( TAILQ_EMPTY( &can_queue_dead_edges ) ) {
    edge = NULL;
  } else {
    edge = TAILQ_FIRST( &can_queue_dead_edges );
    TAILQ_REMOVE( &can_queue_dead_edges, edge, input_peers );
  }
  rtems_mutex_unlock( &can_queue_dead_func_lock );
  return edge;
}

static rtems_task can_queue_dead_func( rtems_task_argument arg )
{
  (void) arg;

  struct rtems_can_queue_edge *qedge;
  struct rtems_can_queue_ends *qends;
  struct rtems_can_queue_ends *entry;

  while ( 1 ) {
    while ( ( qedge = can_queue_dead_edges_cut_first() ) != NULL ) {
      RTEMS_DEBUG_PRINT( "Edge %d disposed\n", qedge->edge_num );

      rtems_can_queue_fifo_done_kern( &qedge->fifo );
      free( qedge );
    }

    rtems_mutex_lock( &can_queue_dead_func_lock );
    entry = TAILQ_FIRST( &can_queue_dead_ends );

    /* Lock can be released there, because only one instance of can_queue_dead_tl
    * can run at once and all other functions add ends only to head.
    */
    rtems_mutex_unlock( &can_queue_dead_func_lock );

    while ( entry != NULL ) {
      qends = entry;
      entry = TAILQ_NEXT( qends, dead_peers );
      if ( !TAILQ_EMPTY( &qends->inlist ) ) {
        continue;
      }
      if ( !TAILQ_EMPTY( &qends->outlist ) ) {
        continue;
      }

      rtems_mutex_lock( &can_queue_dead_func_lock );
      TAILQ_REMOVE( &can_queue_dead_ends, qends, dead_peers );
      rtems_mutex_unlock( &can_queue_dead_func_lock );

      RTEMS_DEBUG_PRINT( "Ends structure disposed\n" );

      free( qends );
    }

    rtems_binary_semaphore_wait( &dead_func_sem );
  }
}

static inline void can_queue_dead_func_release( void )
{
  rtems_binary_semaphore_post( &dead_func_sem );
}

void rtems_can_queue_edge_do_dead( struct rtems_can_queue_edge *qedge )
{
  rtems_can_queue_notify_both_ends( qedge, RTEMS_CAN_QUEUE_NOTIFY_NOUSR );

  if ( rtems_can_queue_disconnect_edge( qedge ) < 0 ) {
    RTEMS_DEBUG_PRINT( "rtems_can_queue_disconnect_edge failed.\n" );
    return;
  }

  rtems_mutex_lock( &can_queue_dead_func_lock );
  TAILQ_INSERT_TAIL( &can_queue_dead_edges, qedge, input_peers );
  rtems_mutex_unlock( &can_queue_dead_func_lock );
  can_queue_dead_func_release();
}

/**
 * @brief Notification callback handler for Linux userspace clients
 *
 * @param qends Pointer to the callback side ends structure.
 * @param qedge Edge which invoked notification.
 * @param what  Notification type.
 *
 * @return None
 *
 */
static void can_queue_notify_user(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge *qedge,
  int what
)
{
  RTEMS_DEBUG_PRINT( "For edge %d, use %d and event %d\n",
                     qedge->edge_num, (int)atomic_load( &qedge->edge_used ), what );

  struct rtems_can_queue_ends_user_t *qends_user = RTEMS_CONTAINER_OF(
    qends,
    struct rtems_can_queue_ends_user_t,
    base
  );

  switch ( what ) {
  case RTEMS_CAN_QUEUE_NOTIFY_EMPTY:
    rtems_binary_semaphore_post( &qends_user->sem_sync );
    if ( rtems_can_queue_fifo_test_and_clear_fl(
        &qedge->fifo,
        RTEMS_CAN_FIFOF_FREEONEMPTY
      ) ) {
      rtems_can_queue_edge_decref( qedge );
    }
    break;
  case RTEMS_CAN_QUEUE_NOTIFY_SPACE:
    rtems_binary_semaphore_post( &qends_user->sem_write );
    break;
  case RTEMS_CAN_QUEUE_NOTIFY_PROC:
    rtems_binary_semaphore_post( &qends_user->sem_read );
    break;
  case RTEMS_CAN_QUEUE_NOTIFY_NOUSR:
    rtems_binary_semaphore_post( &qends_user->sem_sync );
    break;
  case RTEMS_CAN_QUEUE_NOTIFY_DEAD_WANTED:
  case RTEMS_CAN_QUEUE_NOTIFY_DEAD:
    if ( rtems_can_queue_fifo_test_and_clear_fl(
        &qedge->fifo,
        RTEMS_CAN_FIFOF_READY
       ) ) {
      rtems_can_queue_edge_decref( qedge );
    }
    break;
  case RTEMS_CAN_QUEUE_NOTIFY_ATTACH:
    break;
  }
}

int rtems_can_queue_ends_init_user( struct rtems_can_queue_ends_user_t *qends_user )
{
  struct rtems_can_queue_ends *qends = &qends_user->base;

  rtems_can_queue_ends_init( qends );

  qends->notify = can_queue_notify_user;

  rtems_binary_semaphore_init(
    &qends_user->sem_read,
    "can_qends_user_read_sem"
  );
  rtems_binary_semaphore_init(
    &qends_user->sem_write,
    "can_qends_user_write_sem"
  );
  rtems_binary_semaphore_init(
    &qends_user->sem_sync,
    "can_qends_user_sync_sem"
  );

  RTEMS_DEBUG_PRINT( "Initialized\n" );
  return 0;
}

int rtems_can_queue_sync_wait_kern(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge *qedge,
  bool nowait,
  rtems_interval timeout
)
{
  int ret;
  struct rtems_can_queue_ends_user_t *qends_user = RTEMS_CONTAINER_OF(
    qends,
    struct rtems_can_queue_ends_user_t,
    base
  );

  /* Obtain semaphore only if FIFO is not empty. There may be cases where
   * qends_user is used only as RX and closed without sending a message.
   * In that case the semaphore would never be released.
   */

  if ( !rtems_can_queue_fifo_test_flag( &qedge->fifo, RTEMS_CAN_FIFOF_EMPTY ) ) {
    do {
      if ( nowait ) {
        ret = rtems_binary_semaphore_try_wait( &qends_user->sem_sync );
      } else {
        ret = rtems_binary_semaphore_wait_timed_ticks(
          &qends_user->sem_sync,
          timeout
        );
      }
      if ( ret != 0 ) {
        return -1;
      }
    } while (
        !rtems_can_queue_fifo_test_flag( &qedge->fifo, RTEMS_CAN_FIFOF_EMPTY )
      );

    rtems_binary_semaphore_post( &qends_user->sem_sync );
  }

  return 0;
}

int rtems_can_queue_fifo_init_kern(
  struct rtems_can_queue_fifo *fifo,
  int allocated_slot_count,
  int max_data_length
)
{
  int size;
  if ( allocated_slot_count == 0 ) {
    allocated_slot_count = RTEMS_CAN_FIFO_SIZE;
  }

  if ( max_data_length == 0 ) {
    max_data_length = CAN_FRAME_MAX_DLEN;
  }

  size = rtems_can_queue_fifo_slot_size( max_data_length ) * allocated_slot_count;
  fifo->entry = malloc( size );
  if ( fifo->entry == NULL ) {
    return -1;
  }

  fifo->allocated_slot_count = allocated_slot_count;
  fifo->max_data_length = max_data_length;
  return rtems_can_queue_fifo_init_slots( fifo );
}

int rtems_can_queue_fifo_done_kern( struct rtems_can_queue_fifo *fifo )
{
  if ( fifo->entry != NULL ) {
    free( fifo->entry );
  }

  fifo->entry = NULL;
  return 1;
}

struct rtems_can_queue_edge *rtems_can_queue_new_edge_kern(
  int allocated_slot_count,
  int max_data_length
)
{
  struct rtems_can_queue_edge *qedge;
  qedge = (struct rtems_can_queue_edge *)calloc( 1, sizeof( struct rtems_can_queue_edge ) );
  if ( qedge == NULL ) {
    return NULL;
  }

  rtems_mutex_init( &qedge->fifo.fifo_lock, "fifo_lock" );
  if ( rtems_can_queue_fifo_init_kern(
    &qedge->fifo,
    allocated_slot_count,
    max_data_length
  ) < 0 ) {
    free( qedge );
    RTEMS_DEBUG_PRINT( "Failed\n" );
    return NULL;
  }

  atomic_store( &qedge->edge_used, 1 );
  qedge->filter.id = 0;
  qedge->filter.id_mask = 0;
  qedge->filter.flags = 0;
  qedge->filter.flags_mask = CAN_FRAME_ECHO | CAN_FRAME_TXERR | CAN_FRAME_ERR;
  qedge->edge_prio = 0;
  /* not exactly clean, but enough for debugging */
  atomic_fetch_add( &edge_num_cnt, 1 );
  qedge->edge_num = atomic_load( &edge_num_cnt );
  RTEMS_DEBUG_PRINT( "New edge %d\n", qedge->edge_num );
  return qedge;
}

int rtems_can_queue_ends_sync_all_kern(
  struct rtems_can_queue_ends *qends,
  struct timespec *ts
)
{
  struct rtems_can_queue_edge *qedge;
  struct timespec curr;
  struct timespec final;
  struct timespec towait;
  rtems_interval timeout;
  bool nowait;
  int ret;

  timeout = RTEMS_NO_TIMEOUT;
  nowait = false;
  if ( ts != NULL ) {
    clock_gettime( CLOCK_MONOTONIC, &final );
    rtems_timespec_add_to( &final, ts );
  }

  rtems_can_queue_for_each_inedge( qends, qedge ) {
    RTEMS_DEBUG_PRINT( "Called for edge %d\n", qedge->edge_num );
    if ( ts != NULL ) {
      clock_gettime( CLOCK_MONOTONIC, &curr );
      rtems_timespec_subtract( &curr, &final, &towait );
      if ( towait.tv_sec < 0 ) {
        nowait = true;
      } else {
        timeout = rtems_timespec_to_ticks( &towait );
      }
    }

    ret = rtems_can_queue_sync_wait_kern( qends, qedge, nowait, timeout );
    if ( ret == -1 ) {
      rtems_can_queue_edge_decref( qedge );
      return -ETIME;
    }
  }

  return 0;
}

static void can_queue_ends_dispose_postpone(
  struct rtems_can_queue_ends *qends
)
{
  rtems_mutex_lock( &can_queue_dead_func_lock );
  TAILQ_INSERT_TAIL( &can_queue_dead_ends, qends, dead_peers );
  rtems_mutex_unlock( &can_queue_dead_func_lock );
  can_queue_dead_func_release();
}

int rtems_can_queue_ends_dispose_kern(
  struct rtems_can_queue_ends *qends,
  bool nonblock
)
{
  int delayed;

  rtems_can_queue_block_inlist( qends );
  rtems_can_queue_block_outlist( qends );

  /* Wait until all pending messages in the output FIFOs are sent */
  if ( nonblock == false ) {
    rtems_can_queue_ends_sync_all_kern( qends, NULL );
  }

  /* Finish or kill all outgoing edges listed in input_ends */
  delayed = rtems_can_queue_ends_kill_inlist( qends, 1 );
  /* Kill all incoming edges listed in output_ends */
  delayed |= rtems_can_queue_ends_kill_outlist( qends );

  if ( delayed ) {
    can_queue_ends_dispose_postpone( qends );

    RTEMS_DEBUG_PRINT( "Delayed\n" );
    return 1;
  }

  free( qends );
  RTEMS_DEBUG_PRINT( "Finished\n" );
  return 0;
}

int rtems_can_queue_kern_initialize( void )
{
  rtems_id dead_func_id;
  rtems_status_code sc;

  if ( rtems_can_test_and_set_bit( CAN_KERN_INITIALIZED, &kern_flags ) == 1 ) {
    /* Already initialized */
    return 0;
  }

  TAILQ_INIT( &can_queue_dead_ends );
  TAILQ_INIT( &can_queue_dead_edges );

  sc = rtems_task_create(
    rtems_build_name( 'C', 'A', 'N', 'D' ),
    CAN_DEAD_FUNC_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE+0x1000,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &dead_func_id
  );
  if ( sc != RTEMS_SUCCESSFUL )
    return -1;

  rtems_task_start( dead_func_id, can_queue_dead_func, 0 );

  return 0;
}
