/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup CANFDStack
 *
 * @brief This file is part of CAN/CAN FD bus common support
 *        and implements common IO operations.
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/imfs.h>
#include <rtems/malloc.h>
#include <rtems/timespec.h>
#include <rtems/score/basedefs.h>

#include <dev/can/can-helpers.h>
#include <dev/can/can-devcommon.h>
#include <dev/can/can.h>

#include <stdlib.h>
#include <string.h>

static rtems_mutex canuser_manipulation_lock = RTEMS_MUTEX_INITIALIZER(
  "canuser_manipulation_lock"
);

static int can_bus_open(
  rtems_libio_t *iop,
  const char *path,
  int oflag,
  mode_t mode
);
static int can_bus_close( rtems_libio_t *iop );
static ssize_t can_bus_read( rtems_libio_t *iop, void *buffer, size_t count );
static ssize_t can_bus_write(
  rtems_libio_t *iop,
  const void *buffer,
  size_t count
);
static int can_bus_ioctl(
  rtems_libio_t *iop,
  ioctl_command_t request,
  void *buffer
);

static struct rtems_can_user *can_bus_get_user( rtems_libio_t *iop )
{
  struct rtems_can_user *canuser = iop->data1;
  if ( !canuser || ( canuser->magic != RTEMS_CAN_USER_MAGIC ) ) {
     rtems_set_errno_and_return_value( ENODEV, NULL );
  }

  if ( canuser->bus->chip == NULL ) {
     rtems_set_errno_and_return_value( EIO, NULL );
  }

  return canuser;
}

static int can_bus_ioctl_poll_tx_ready(
  struct rtems_can_queue_ends_user_t *qends_user,
  struct timespec *ts
)
{
  struct rtems_can_queue_ends *qends = &qends_user->base;
  rtems_interval timeout;
  bool nowait;
  struct timespec curr;
  struct timespec final;
  struct timespec towait;
  int ret;

  ret = rtems_can_queue_test_inslot( qends );
  if ( ret < 0 ) {
    /* No message available */
    timeout = RTEMS_NO_TIMEOUT;
    nowait = false;
    if ( ts != NULL ) {
      /* Get absolute monotonic final time */
      clock_gettime( CLOCK_MONOTONIC, &final );
      rtems_timespec_add_to( &final, ts );
    }

    do {
      if ( ts != NULL ) {
        /* Check current monotonic time and calculate new timeout based
         * on the difference between final time and current time.
         */
        clock_gettime( CLOCK_MONOTONIC, &curr );
        rtems_timespec_subtract( &curr, &final, &towait );
        if ( towait.tv_sec < 0 ) {
          nowait = true;
        } else {
          timeout = rtems_timespec_to_ticks( &towait );
        }
      }

      if ( nowait ) {
        ret = rtems_binary_semaphore_try_wait( &qends_user->sem_write );
      } else {
        ret = rtems_binary_semaphore_wait_timed_ticks(
          &qends_user->sem_write,
          timeout
        );
      }
      if ( ret != 0 ) {
        ret = -ETIME;
        break;
      }
      ret = rtems_can_queue_test_inslot( qends );
    } while ( ret < 0 );

    rtems_binary_semaphore_post( &qends_user->sem_write );
  }

  return ret;
}

static int can_bus_ioctl_poll_rx_avail(
  struct rtems_can_queue_ends_user_t *qends_user,
  struct timespec *ts
)
{
  struct rtems_can_queue_ends *qends = &qends_user->base;
  rtems_interval timeout;
  bool nowait;
  struct timespec curr;
  struct timespec final;
  struct timespec towait;
  int ret;

  ret = rtems_can_queue_pending_outslot_prio( qends, 0 );
  if ( ret < 0 ) {
    /* No message available */
    timeout = RTEMS_NO_TIMEOUT;
    nowait = false;
    if ( ts != NULL ) {
      /* Get absolute monotonic final time */
      clock_gettime( CLOCK_MONOTONIC, &final );
      rtems_timespec_add_to( &final, ts );
    }

    do {
      if ( ts != NULL ) {
        /* Check current monotonic time and calculate new timeout based
         * on the difference between final time and current time.
         */
        clock_gettime( CLOCK_MONOTONIC, &curr );
        rtems_timespec_subtract( &curr, &final, &towait );
        if ( towait.tv_sec < 0 ) {
          nowait = true;
        } else {
          timeout = rtems_timespec_to_ticks( &towait );
        }
      }

      if ( nowait ) {
        ret = rtems_binary_semaphore_try_wait( &qends_user->sem_read );
      } else {
        ret = rtems_binary_semaphore_wait_timed_ticks(
          &qends_user->sem_read,
          timeout
        );
      }
      if ( ret != 0 ) {
        ret = -ETIME;
        break;
      }
      ret = rtems_can_queue_pending_outslot_prio( qends, 0 );
    } while ( ret < 0 );

    rtems_binary_semaphore_post( &qends_user->sem_read );
  }

  return ret;
}

static int can_bus_ioctl_create_queue(
  struct rtems_can_queue_param queue,
  struct rtems_can_chip *chip,
  struct rtems_can_queue_ends *qends
)
{
  struct rtems_can_queue_edge *new_edge = NULL;
  struct rtems_can_queue_ends *input_ends = NULL;
  struct rtems_can_queue_ends *output_ends = NULL;

  /* Set edge direction according to input parameter */
  if ( queue.direction == RTEMS_CAN_QUEUE_RX ) {
    input_ends = &chip->qends_dev->base;
    output_ends = qends;
  } else if ( queue.direction == RTEMS_CAN_QUEUE_TX ) {
    input_ends = qends;
    output_ends = &chip->qends_dev->base;
  } else {
    return -EINVAL;
  }

  /*
   * Check whether correct dlen_max and buffer_size is to be set.
   * Note that dlen_max and buffer_size are unsigned and do not need to
   * be checked for < 0.
   */
  if ( queue.dlen_max > CAN_FRAME_MAX_DLEN ) {
    return -EINVAL;
  }

  if ( queue.dlen_max == 0 ) {
    queue.dlen_max = chip->capabilities & RTEMS_CAN_CHIP_CAPABILITIES_FD ?
      CAN_FRAME_FD_DLEN : CAN_FRAME_STANDARD_DLEN;
  }

  if ( queue.buffer_size == 0 ) {
    queue.buffer_size = RTEMS_CAN_FIFO_SIZE;
  }

  /*
   * Check whether correct queue priority is to be set.
   * Note that priority is unsigned and does not need to be checked for < 0.
   */
  if ( queue.priority > RTEMS_CAN_QUEUE_PRIO_NR ) {
    return -EINVAL;
  }

  /* Create new edge */
  new_edge = rtems_can_queue_new_edge_kern( queue.buffer_size, queue.dlen_max );
  if ( new_edge == NULL ) {
    return -ENOMEM;
  }

  /* Copy filter and assign edge priority */
  new_edge->filter = queue.filter;
  new_edge->edge_prio = queue.priority;

  /* And connect edge. This should not fail. */
  rtems_can_queue_connect_edge( new_edge, input_ends, output_ends );
  rtems_can_queue_edge_decref( new_edge );
  return 0;  
}

static int can_bus_open(
  rtems_libio_t *iop,
  const char *path,
  int oflag,
  mode_t mode
)
{
  (void) path;
  (void) oflag;
  (void) mode;

  struct rtems_can_queue_ends_user_t *qends_user;
  struct rtems_can_queue_ends *qends;
  struct rtems_can_queue_edge *edge4read = NULL;
  struct rtems_can_queue_edge *edge4write = NULL;
  struct rtems_can_chip *chip;
  struct rtems_can_user *canuser;
  int can_frame_dlen;
  int sc;

  struct rtems_can_bus *bus = IMFS_generic_get_context_by_iop( iop );
  if ( bus == NULL ) {
    rtems_set_errno_and_return_minus_one( ENODEV );
  }

  chip = bus->chip;

  atomic_fetch_add( &chip->used, 1 );

  canuser = ( struct rtems_can_user * )malloc( sizeof( struct rtems_can_user ) );
  if ( canuser == NULL ) {
    atomic_fetch_sub( &chip->used, 1 );
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }

  qends_user = ( struct rtems_can_queue_ends_user_t * )malloc( sizeof( struct rtems_can_queue_ends_user_t ) );
  if ( qends_user == NULL ) {
    free( canuser );
    atomic_fetch_sub( &chip->used, 1 );
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }

  canuser->flags = 0;
  canuser->bus = bus;
  canuser->magic = RTEMS_CAN_USER_MAGIC;

  iop->data1 = canuser;

  qends = &qends_user->base;
  rtems_can_queue_ends_init_user( qends_user );
  canuser->qends_user = qends_user;

  can_frame_dlen = chip->capabilities & RTEMS_CAN_CHIP_CAPABILITIES_FD ?
    CAN_FRAME_FD_DLEN : CAN_FRAME_STANDARD_DLEN;

  rtems_mutex_lock( &canuser_manipulation_lock );
  TAILQ_INSERT_TAIL( &chip->can_users, canuser, peers );
  rtems_mutex_unlock( &canuser_manipulation_lock );

  edge4write = rtems_can_queue_new_edge_kern( RTEMS_CAN_FIFO_SIZE, can_frame_dlen );
  sc = rtems_can_queue_connect_edge( edge4write, qends, &chip->qends_dev->base );
  if ( sc < 0 )
    goto no_qedge;

  edge4read = rtems_can_queue_new_edge_kern( RTEMS_CAN_FIFO_SIZE, can_frame_dlen );
  sc = rtems_can_queue_connect_edge( edge4read, &chip->qends_dev->base, qends );
  if ( sc < 0 )
    goto no_qedge;

  rtems_can_queue_edge_decref( edge4write );
  rtems_can_queue_edge_decref( edge4read );

  return 0;

no_qedge:
  if ( edge4write != NULL ) {
    rtems_can_queue_edge_decref( edge4write );
  }

  if ( edge4read != NULL ) {
    rtems_can_queue_edge_decref( edge4read );
  }
  rtems_mutex_lock( &canuser_manipulation_lock );
  TAILQ_REMOVE( &chip->can_users, canuser, peers );
  rtems_mutex_unlock( &canuser_manipulation_lock );
  canuser->qends_user = NULL;
  rtems_can_queue_ends_dispose_kern( qends, chip->close_nonblock );

  free( canuser );
  atomic_fetch_sub( &chip->used, 1 );
  rtems_set_errno_and_return_minus_one( ENOMEM );
}

static int can_bus_close( rtems_libio_t *iop )
{
  struct rtems_can_user *canuser;
  struct rtems_can_queue_ends_user_t *qends_user;
  struct rtems_can_queue_ends *qends;
  struct rtems_can_chip *chip;

  canuser = iop->data1;

  if ( !canuser || ( canuser->magic != RTEMS_CAN_USER_MAGIC ) ) {
    rtems_set_errno_and_return_minus_one( ENODEV );
  }

  chip = canuser->bus->chip;
  if ( chip == NULL ) {
    rtems_set_errno_and_return_minus_one( EIO );
  }

  qends_user = canuser->qends_user;
  qends = &qends_user->base;

  rtems_mutex_lock( &canuser_manipulation_lock );
  TAILQ_REMOVE( &chip->can_users, canuser, peers );
  rtems_mutex_unlock( &canuser_manipulation_lock );
  canuser->qends_user = NULL;
  rtems_can_queue_ends_dispose_kern( qends, chip->close_nonblock );

  free( canuser );

  atomic_fetch_sub( &chip->used, 1 );
  return 0;
}

static ssize_t can_bus_read( rtems_libio_t *iop, void *buffer, size_t count )
{
  struct rtems_can_queue_ends_user_t *qends_user;
  struct rtems_can_queue_ends *qends;
  struct rtems_can_queue_edge *qedge;
  struct rtems_can_queue_slot *slot;
  struct rtems_can_user *canuser;
  struct rtems_can_chip *chip;
  size_t frame_len;
  const size_t frame_header_len = sizeof( struct can_frame_header );
  int ret;

  canuser = can_bus_get_user( iop );
  if ( canuser == NULL ) {
    /* Correct errno is already set in can_bus_get_user function. */
    return -1;
  }

  chip = canuser->bus->chip;

  if ( count < frame_header_len ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  qends_user = canuser->qends_user;
  qends = &qends_user->base;

  ret = rtems_can_queue_test_outslot( qends, &qedge, &slot );
  if ( ret < 0 ) {
    if ( rtems_libio_iop_is_no_delay( iop ) ) {
      rtems_set_errno_and_return_minus_one( EAGAIN );
    }

    if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) == 0) {
      /* Chip is not running */
      rtems_set_errno_and_return_minus_one( EPERM );
    }

    do {
      rtems_binary_semaphore_wait( &qends_user->sem_read );
      ret = rtems_can_queue_test_outslot( qends, &qedge, &slot );
    } while ( ret < 0 );

    rtems_binary_semaphore_post( &qends_user->sem_read );
  }

  frame_len = can_framesize( ( struct can_frame *)&slot->frame );
  if ( count > frame_len ) {
    count = frame_len;
  }

  memcpy( buffer, &slot->frame, count );

  if ( count < frame_len ) {
    rtems_can_queue_push_back_outslot( qends, qedge, slot );
    rtems_set_errno_and_return_minus_one( EMSGSIZE );
  } else {
    rtems_can_queue_free_outslot( qends, qedge, slot );
  }

  return count;
}

static ssize_t can_bus_write(
  rtems_libio_t *iop,
  const void *buffer,
  size_t count
)
{
  struct rtems_can_queue_ends_user_t *qends_user;
  struct rtems_can_queue_ends *qends;
  struct rtems_can_queue_edge *qedge;
  struct rtems_can_queue_slot *slot;
  struct rtems_can_user *canuser;
  struct rtems_can_chip *chip;
  struct can_frame_header frame_header;
  const size_t frame_header_len = sizeof( struct can_frame_header );
  size_t frame_bytes;
  int ret;

  canuser = can_bus_get_user( iop );
  if ( canuser == NULL ) {
    /* Correct errno is already set in can_bus_get_user function. */
    return -1;
  }

  chip = canuser->bus->chip;
  if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) == 0 ) {
    /* Chip is not running */
    rtems_set_errno_and_return_minus_one( EPERM );
  }

  if ( count < frame_header_len ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  memcpy( &frame_header, buffer, frame_header_len );
  if ( frame_header.dlen > CAN_FRAME_MAX_DLEN ) {
    rtems_set_errno_and_return_minus_one( EMSGSIZE );
  }

  if ( count < frame_header_len + frame_header.dlen ) {
    rtems_set_errno_and_return_minus_one( EMSGSIZE );
  }

  frame_bytes = frame_header_len + frame_header.dlen;
  qends_user = canuser->qends_user;
  qends = &qends_user->base;

  if ( ( ret = rtems_can_queue_get_inslot_for_prio( qends, &qedge, &slot, &frame_header, 0, 2 ) ) < 0 ) {
    if ( ret < -1 ) {
      rtems_set_errno_and_return_minus_one( EIO );
    }

    if ( rtems_libio_iop_is_no_delay( iop ) ) {
      rtems_set_errno_and_return_minus_one( EAGAIN );
    }

    do {
      rtems_binary_semaphore_wait( &qends_user->sem_write );
      if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) == 0 ) {
        rtems_binary_semaphore_post( &qends_user->sem_write );
        rtems_set_errno_and_return_minus_one( EPERM );
      }
      ret = rtems_can_queue_get_inslot_for_prio( qends, &qedge, &slot, &frame_header, 0, 2 );
      if ( ret < -1 ) {
        rtems_binary_semaphore_post( &qends_user->sem_write );
        rtems_set_errno_and_return_minus_one( EIO );
      }
    } while ( ret < 0 );

    rtems_binary_semaphore_post( &qends_user->sem_write );
  }

  if ( frame_header.dlen > qedge->fifo.max_data_length ) {
    rtems_can_queue_abort_inslot( qends, qedge, slot );
    rtems_set_errno_and_return_minus_one( EMSGSIZE );
  }

  memcpy( &slot->frame, buffer, frame_bytes );

  /* Force extended frame format if id exceeds 11 bits */
  if ( slot->frame.header.can_id & ~CAN_FRAME_BFF_ID_MASK & CAN_FRAME_EFF_ID_MASK ) {
    slot->frame.header.flags |= CAN_FRAME_IDE;
  }

  rtems_can_queue_put_inslot( qends, qedge, slot );

  return count;
}

static int can_bus_ioctl(
  rtems_libio_t *iop,
  ioctl_command_t command,
  void *arg
)
{
  struct rtems_can_queue_ends_user_t *qends_user;
  struct rtems_can_queue_ends *qends;
  struct rtems_can_user *canuser;
  struct rtems_can_chip *chip;
  struct timespec ts;
  int direction;
  int ret = -EINVAL;

  canuser = iop->data1;
  if ( !canuser || ( canuser->magic != RTEMS_CAN_USER_MAGIC ) ) {
    rtems_set_errno_and_return_minus_one( ENODEV );
  }

  chip = canuser->bus->chip;
  if ( chip == NULL ) {
    rtems_set_errno_and_return_minus_one( EIO );
  }

  qends_user = canuser->qends_user;
  qends = &qends_user->base;

  switch ( command ) {
    case RTEMS_CAN_CHIP_START:
      if ( chip->chip_ops.start_chip != NULL ) {
        ret = chip->chip_ops.start_chip( chip );
      }
      break;

    case RTEMS_CAN_CHIP_STOP:
      if ( chip->chip_ops.stop_chip != NULL ) {
        if ( arg != 0 ) {
          ts = *( struct timespec * )arg;
          ret = chip->chip_ops.stop_chip( chip, &ts );
        } else {
          ret = chip->chip_ops.stop_chip( chip, NULL );
        }
      }
      break;

    case RTEMS_CAN_CLOSE_NONBLOCK:
      chip->close_nonblock = ( bool )arg;
      ret = 0;
      break;

    case RTEMS_CAN_WAIT_TX_DONE:
      if ( arg != 0 ) {
        ts = *( struct timespec * )arg;
        ret = rtems_can_queue_ends_sync_all_kern( qends, &ts );
      } else {
        ret = rtems_can_queue_ends_sync_all_kern( qends, NULL );
      }
      break;

    case RTEMS_CAN_POLL_TX_READY:
      if ( arg != 0) {
        ts = *( struct timespec * )arg;
        ret = can_bus_ioctl_poll_tx_ready( qends_user, &ts );
      } else {
        ret = can_bus_ioctl_poll_tx_ready( qends_user, NULL );
      }
      break;

    case RTEMS_CAN_POLL_RX_AVAIL:
      if ( arg != 0) {
        ts = *( struct timespec * )arg;
        ret = can_bus_ioctl_poll_rx_avail( qends_user, &ts );
      } else {
        ret = can_bus_ioctl_poll_rx_avail( qends_user, NULL );
      }
      break;

    case RTEMS_CAN_DISCARD_QUEUES:
      direction = ( intptr_t )arg;
      ret = 0;
      if ( ( direction & ( RTEMS_CAN_QUEUE_RX | RTEMS_CAN_QUEUE_TX ) ) == 0 ) {
        ret = -EINVAL;
      } else {
        if ( direction & RTEMS_CAN_QUEUE_RX ) {
          rtems_can_queue_ends_kill_outlist( qends );
        }

        if ( direction & RTEMS_CAN_QUEUE_TX ) {
          rtems_can_queue_ends_kill_inlist( qends, 0 );
        }
      }
      break;

    case RTEMS_CAN_FLUSH_QUEUES:
      direction = ( intptr_t )arg;
      ret = 0;

      if ( ( direction & ( RTEMS_CAN_QUEUE_RX | RTEMS_CAN_QUEUE_TX ) ) == 0 ) {
        ret = -EINVAL;
      } else {
        if ( direction & RTEMS_CAN_QUEUE_RX ) {
          rtems_can_queue_ends_flush_outlist( qends );
        }

        if ( direction & RTEMS_CAN_QUEUE_TX ) {
          rtems_can_queue_ends_flush_inlist( qends );
        }
      }
      break;

    case RTEMS_CAN_CREATE_QUEUE:
      ret = can_bus_ioctl_create_queue(
        *( struct rtems_can_queue_param *)arg,
        chip,
        qends
      );
      break;

    case RTEMS_CAN_SET_BITRATE:
      struct rtems_can_set_bittiming bittime =
        *( struct rtems_can_set_bittiming *)arg;
      if ( bittime.from == RTEMS_CAN_BITTIME_FROM_BITRATE ) {
        ret = chip->chip_ops.calc_bittiming(
          chip,
          bittime.type,
          &bittime.bittiming
        );
      } else if ( bittime.from == RTEMS_CAN_BITTIME_FROM_PRECOMPUTED ) {
        ret = chip->chip_ops.check_and_set_bittiming(
          chip,
          bittime.type,
          &bittime.bittiming
        );
      }
      break;

    case RTEMS_CAN_CHIP_SET_MODE:
      uint32_t ctrlmode = ( uintptr_t )arg;
      if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) == 1 ) {
        /* Cannot change the mode if chip has already started */
        ret = -EPERM;
        break;
      }

      if ( ( ctrlmode & ~chip->ctrlmode_supported ) & CAN_CTRLMODE_MASK ) {
        /* Not supported mode, return error */
        ret = -EINVAL;
      } else {
        chip->ctrlmode = ctrlmode;
        ret = RTEMS_SUCCESSFUL;
      }
      break;

    case RTEMS_CAN_GET_BITTIMING:
      struct rtems_can_get_bittiming *bittiming =
        ( struct rtems_can_get_bittiming *)arg;
      ret = RTEMS_SUCCESSFUL;
      if ( bittiming->type == RTEMS_CAN_BITTIME_NOMINAL ) {
        memcpy(
          &bittiming->bittiming,
          &chip->bittiming,
          sizeof( struct rtems_can_bittiming )
        );
        memcpy(
          &bittiming->bittiming_const,
          chip->bittiming_const,
          sizeof( struct rtems_can_bittiming_const )
        );
      } else if ( bittiming->type == RTEMS_CAN_BITTIME_DATA ) {
        memcpy(
          &bittiming->bittiming,
          &chip->data_bittiming,
          sizeof( struct rtems_can_bittiming )
        );
        memcpy(
          &bittiming->bittiming_const,
          chip->data_bittiming_const,
          sizeof( struct rtems_can_bittiming_const )
        );
      } else {
        ret = -EINVAL;
      }

      break;

    case RTEMS_CAN_CHIP_GET_TIMESTAMP:
      uint64_t timestamp;
      if ( chip->chip_ops.get_chip_timestamp != NULL ) {
        ret = chip->chip_ops.get_chip_timestamp( chip, &timestamp );
        *( uint64_t * )arg = timestamp;
      }
      break;

    case RTEMS_CAN_CHIP_STATISTICS:
      *( struct rtems_can_stats* )arg = chip->chip_stats;
      ret = RTEMS_SUCCESSFUL;
      break;

    case RTEMS_CAN_CHIP_GET_INFO:
      if ( chip->chip_ops.get_chip_info != NULL ) {
        ret = chip->chip_ops.get_chip_info( chip, ( intptr_t )arg );
      }
      break;

    default:
      if ( chip->chip_ops.chip_ioctl != NULL ) {
        ret = chip->chip_ops.chip_ioctl( chip, command, arg );
      }
      break;
  }

  if ( ret < 0 ) {
    rtems_set_errno_and_return_minus_one( -ret );
  }

  return ret;
}

static const rtems_filesystem_file_handlers_r can_bus_handler = {
  .open_h = can_bus_open,
  .close_h = can_bus_close,
  .read_h = can_bus_read,
  .write_h = can_bus_write,
  .ioctl_h = can_bus_ioctl,
  .lseek_h = rtems_filesystem_default_lseek,
  .fstat_h = IMFS_stat,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .mmap_h = rtems_filesystem_default_mmap,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};

static void can_bus_node_destroy( IMFS_jnode_t *node )
{
  IMFS_node_destroy_default( node );
}

static const IMFS_node_control can_bus_node_control = IMFS_GENERIC_INITIALIZER(
  &can_bus_handler,
  IMFS_node_initialize_generic,
  can_bus_node_destroy
);

int rtems_can_bus_notify_chip_stop( struct rtems_can_chip *chip )
{
  struct rtems_can_user *user;

  rtems_mutex_lock( &canuser_manipulation_lock );
  user = TAILQ_FIRST( &chip->can_users );

  while ( user != NULL ) {
    rtems_binary_semaphore_post( &user->qends_user->sem_read );
    user = TAILQ_NEXT( user, peers );
  }

  rtems_mutex_unlock( &canuser_manipulation_lock );
  return 0;
}

int rtems_can_bus_register( struct rtems_can_bus *bus, const char *bus_path )
{
  int ret;

  if ( rtems_can_queue_kern_initialize() < 0 ) {
    return -1;
  }

  TAILQ_INIT( &bus->chip->can_users );

  ret = IMFS_make_generic_node(
    bus_path,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &can_bus_node_control,
    bus
  );

  return ret;
}
