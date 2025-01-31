/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup CANFDStack
 *
 * @brief This file is part of CAN/CAN FD bus common support
 *        and implements controller's side of FIFO operations.
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
#include <rtems/score/basedefs.h>

#include <dev/can/can-devcommon.h>
#include <dev/can/can-stats.h>

#define SEC_TO_NSEC 1000000000

static void can_queue_notify_chip(
  struct rtems_can_queue_ends *qends,
  struct rtems_can_queue_edge *qedge,
  int what
)
{
  struct rtems_can_queue_ends_dev *qends_dev = RTEMS_CONTAINER_OF(
    qends,
    struct rtems_can_queue_ends_dev,
    base
  );

  switch ( what ) {
    case RTEMS_CAN_QUEUE_NOTIFY_PROC:
      rtems_binary_semaphore_post( &qends_dev->worker_sem );
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

void rtems_can_stats_reset( struct rtems_can_stats *stats )
{
  memset( stats, 0, sizeof( *stats ) );
}

int rtems_can_queue_ends_init_chip(
  struct rtems_can_chip *chip,
  const char *name
)
{
  int ret;

  ret = rtems_can_queue_ends_init( &chip->qends_dev->base );
  if ( ret < 0 ) {
    return ret;
  }

  chip->qends_dev->chip = chip;
  chip->qends_dev->base.notify = can_queue_notify_chip;
  rtems_binary_semaphore_init( &chip->qends_dev->worker_sem, name );

  return 0;
}

uint64_t rtems_can_fill_timestamp( void )
{
  struct timespec tp;
  clock_gettime( CLOCK_MONOTONIC, &tp );
  return ( tp.tv_sec * SEC_TO_NSEC + tp.tv_nsec );
}

int rtems_can_chip_start( struct rtems_can_chip *chip )
{
  if ( chip->chip_ops.start_chip != NULL ) {
    return chip->chip_ops.start_chip( chip );
  }

  return -ENODEV;
}
