/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup CANFDStack
 *
 * @brief This header file is part of CAN/CAN FD bus common support. It
 *        implements controller's statistics.
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

#ifndef _DEV_CAN_CAN_STATS_H
#define _DEV_CAN_CAN_STATS_H

/**
 * @brief This enum represents the current state of CAN controller
 */
enum can_state {
  /**
   * @brief This member indicates the controller is in error active state (
   *  RX/TX error count < 96)
   */
	CAN_STATE_ERROR_ACTIVE = 0,
  /**
   * @brief This member indicates the controller is in error warning state (
   *  RX/TX error count < 128)
   */
	CAN_STATE_ERROR_WARNING,
  /**
   * @brief This member indicates the controller is in error passive state (
   *  RX/TX error count < 256)
   */
	CAN_STATE_ERROR_PASSIVE,
  /**
   * @brief This member indicates the controller is in bus off state (
   *  RX/TX error count >= 256)
   */
	CAN_STATE_BUS_OFF,
  /**
   * @brief This member indicates the the controller is stopped.
   */
	CAN_STATE_STOPPED,
  /**
   * @brief This member indicates the the controller is in sleep.
   */
	CAN_STATE_SLEEPING,
  /**
   * @brief This member indicates the the controller is in stopping process.
   */
  CAN_STATE_STOPPING,
  /**
   * @brief This member holds the maximum number of controller's states.
   */
	CAN_STATE_MAX
};

/**
 * @brief This structure is used to represent CAN statistics
 */
struct rtems_can_stats {
  /**
   * @brief This member holds number of succesful TX frames.
   */
  unsigned long tx_done;
  /**
   * @brief This member holds number of succesful RX frames.
   */
  unsigned long rx_done;
  /**
   * @brief This member holds number of bytes succesfully send.
   */
  unsigned long tx_bytes;
  /**
   * @brief This member holds number of bytes succesfully received.
   */
  unsigned long rx_bytes;
  /**
   * @brief This member holds number of TX errors.
   */
  unsigned long tx_error;
  /**
   * @brief This member holds number of RX errors.
   */
  unsigned long rx_error;
  /**
   * @brief This member holds number of overflows on RX side.
   */
  unsigned long rx_overflows;
  /**
   * @brief This member holds controller's state (error active, passive,
   * bus off)
   */
  int chip_state;
};

static inline void rtems_can_stats_add_tx_done(
  struct rtems_can_stats *stats
)
{
  stats->tx_done += 1;
}

static inline void rtems_can_stats_add_rx_done(
  struct rtems_can_stats *stats
)
{
  stats->rx_done += 1;
}

static inline void rtems_can_stats_add_tx_bytes(
  struct rtems_can_stats *stats,
  uint16_t nbytes
)
{
  stats->tx_bytes += nbytes;
}

static inline void rtems_can_stats_add_rx_bytes(
  struct rtems_can_stats *stats,
  uint16_t nbytes
)
{
  stats->rx_bytes += nbytes;
}

static inline void rtems_can_stats_add_tx_error(
  struct rtems_can_stats *stats
)
{
  stats->tx_error += 1;
}

static inline void rtems_can_stats_add_rx_error(
  struct rtems_can_stats *stats
)
{
  stats->rx_error += 1;
}

static inline void rtems_can_stats_add_rx_overflows(
  struct rtems_can_stats *stats
)
{
  stats->rx_overflows += 1;
}

static inline void rtems_can_stats_set_state(
  struct rtems_can_stats *stats,
  enum can_state state
)
{
  stats->chip_state = state;
}

/**
 * @brief This function resets the controller's statistics.
 *
 *  This sets all fields of @ref rtems_can_stats structure to zero. It
 *  should be called by the controller after it is started.
 *
 * @param stats   Pointer to @ref rtems_can_stats structure.
 */
void rtems_can_stats_reset( struct rtems_can_stats *stats );

#endif /* _DEV_CAN_CAN_STATS_H */
