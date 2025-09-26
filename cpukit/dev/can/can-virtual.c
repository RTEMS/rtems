/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup CANVirtual
 *
 * @brief This file implements virtual CAN chip.
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

/**************************************************************************/

#include <stdlib.h>
#include <rtems.h>
#include <rtems/malloc.h>
#include <rtems/score/basedefs.h>

#include <dev/can/can-virtual.h>
#include <dev/can/can-helpers.h>
#include <dev/can/can-devcommon.h>
#include <dev/can/can.h>

#define CAN_VIRTUAL_PRIORITY 120

static rtems_task virtual_worker( rtems_task_argument arg )
{
  struct rtems_can_chip *chip = (struct rtems_can_chip *)arg;
  struct rtems_can_queue_ends_dev *qends_dev = chip->qends_dev;
  struct rtems_can_queue_ends *qends = &qends_dev->base;
  struct rtems_can_queue_edge *qedge;
  struct rtems_can_queue_slot *slot;
  int cmd;
  while ( 1 ) {
    while ( ( cmd = rtems_can_queue_test_outslot( qends, &qedge, &slot ) ) >= 0 ) {
      if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) == 0 ) {
        if ( cmd >= 0 ) {
          rtems_can_queue_filter_frame_to_edges(
            qends,
            qedge,
            &slot->frame,
            CAN_FRAME_TXERR
          );
          rtems_can_stats_add_tx_error( &chip->chip_stats );
        } else {
          rtems_binary_semaphore_post( &chip->stop_sem );
        }
      } else  if ( cmd >= 0 ) {
        /* fill CAN message timestamp */
        slot->frame.header.timestamp = rtems_can_fill_timestamp();

        rtems_can_queue_filter_frame_to_edges(
          qends,
          qedge,
          &slot->frame,
          CAN_FRAME_LOCAL
        );
        rtems_can_stats_add_tx_done( &chip->chip_stats );
        rtems_can_stats_add_rx_done( &chip->chip_stats );
      }
      rtems_can_queue_free_outslot( qends, qedge, slot );
    }

    rtems_binary_semaphore_wait( &qends_dev->worker_sem );
  }
}

static int virtual_get_chip_info( struct rtems_can_chip *chip, int what )
{
  int ret;

  switch( what ) {
    case RTEMS_CAN_CHIP_BITRATE:
    case RTEMS_CAN_CHIP_DBITRATE:
      ret = -EINVAL;
      break;
    case RTEMS_CAN_CHIP_NUSERS:
      ret = atomic_load( &chip->used );
      break;
    case RTEMS_CAN_CHIP_FLAGS:
      ret = atomic_load( &chip->flags );
      break;
    default:
      ret = -EINVAL;
      break;
  }

  return ret;
}

/**
 * @brief  This function implements chip specific ioctl calls.
 *
 * @param  chip    Pointer to chip structure.
 * @param  command IOCTL command
 * @param  arg     Void pointer to IOCL argument
 *
 * @return Negative value on error.
 */
static int virtual_chip_ioctl(
  struct rtems_can_chip *chip,
  ioctl_command_t command,
  void *arg
)
{
  (void) chip;
  (void) arg;

  int ret;

  switch( command ) {
    default:
      ret = -EINVAL;
      break;
  }

  return ret;
}

static int virtual_stop_chip ( struct rtems_can_chip *chip, struct timespec *ts )
{
  rtems_interval timeout;
  struct timespec curr;
  struct timespec final;
  struct timespec towait;
  int ret;

  rtems_mutex_lock( &chip->lock );
  if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) == 0 ) {
    /* Already stopped, return. */
    rtems_mutex_unlock( &chip->lock );
    return 0;
  }

  rtems_can_clear_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags );
  rtems_binary_semaphore_post( &chip->qends_dev->worker_sem );

  if ( ts != NULL ) {
    /* Get absolute monotonic final time */
    clock_gettime( CLOCK_MONOTONIC, &final );
    rtems_timespec_add_to( &final, ts );

    clock_gettime( CLOCK_MONOTONIC, &curr );
    rtems_timespec_subtract( &curr, &final, &towait );
    if ( towait.tv_sec < 0 ) {
      ret = rtems_binary_semaphore_try_wait( &chip->stop_sem );
    } else {
      timeout = rtems_timespec_to_ticks( &towait );
      ret = rtems_binary_semaphore_wait_timed_ticks(
        &chip->stop_sem,
        timeout
      );
    }
  } else {
    ret = rtems_binary_semaphore_try_wait( &chip->stop_sem );
  }

  if (ret < 0) {
    rtems_can_queue_ends_flush_outlist( &chip->qends_dev->base );
  }

  rtems_binary_semaphore_post( &chip->qends_dev->worker_sem );
  rtems_can_stats_set_state( &chip->chip_stats, CAN_STATE_STOPPED );
  rtems_can_bus_notify_chip_stop( chip );
  rtems_mutex_unlock( &chip->lock );
  return 0;
}

static int virtual_start_chip ( struct rtems_can_chip *chip )
{
  rtems_mutex_lock( &chip->lock );
  if ( rtems_can_test_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags ) ) {
    /* Chip was already started, skip */
    rtems_mutex_unlock( &chip->lock );
    return 0;
  }

  rtems_can_set_bit( RTEMS_CAN_CHIP_RUNNING, &chip->flags );
  rtems_can_stats_reset( &chip->chip_stats );
  rtems_can_stats_set_state( &chip->chip_stats, CAN_STATE_ERROR_ACTIVE );
  rtems_mutex_unlock( &chip->lock );
  return 0;
}

static void virtual_register( struct rtems_can_chip_ops *chip_ops )
{
  chip_ops->start_chip = virtual_start_chip;
  chip_ops->stop_chip = virtual_stop_chip;
  chip_ops->chip_ioctl = virtual_chip_ioctl;
  chip_ops->get_chip_timestamp = NULL;
  chip_ops->get_chip_info = virtual_get_chip_info;
}

struct rtems_can_chip *rtems_can_virtual_initialize( void )
{
  rtems_id worker_task_id;
  rtems_status_code sc;
  struct rtems_can_chip *chip;

  chip = calloc( 1, sizeof( struct rtems_can_chip ) );
  if ( !chip ) {
    return NULL;
  }

  chip->type = CAN_VIRTUAL_TYPE;
  chip->capabilities = RTEMS_CAN_CHIP_CAPABILITIES_FD;
  chip->irq = 0;

  chip->qends_dev = calloc( 1, sizeof( struct rtems_can_queue_ends_dev ) );
  if ( !chip->qends_dev ) {
    free( chip );
    return NULL;
  }

  rtems_mutex_init( &chip->lock, "virtual_lock" );

  rtems_binary_semaphore_init(
    &chip->stop_sem,
    "can_virtual_stop_sem"
  );

  rtems_can_queue_ends_init_chip(
    chip,
    "can_virtual_worker"
  );

  sc = rtems_task_create(
    rtems_build_name( 'C', 'A', 'N', 'V' ),
    CAN_VIRTUAL_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE+0x1000,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &worker_task_id
  );
  if ( sc != RTEMS_SUCCESSFUL ) {
    free( chip->qends_dev );
    free( chip );
    return NULL;
  }

  rtems_task_start( worker_task_id, virtual_worker,
                    (rtems_task_argument)chip );

  virtual_register( &chip->chip_ops );

  rtems_can_set_bit( RTEMS_CAN_CHIP_CONFIGURED, &chip->flags );
  rtems_can_stats_set_state(&chip->chip_stats, CAN_STATE_STOPPED );

  return chip;
}
