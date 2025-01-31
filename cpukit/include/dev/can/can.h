/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup CANFDStack
 *
 * @brief This file is part of CAN/CAN FD bus common support
 *        and defines generic CAN structures used for common IO
 *        operations.
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

#ifndef _DEV_CAN_CAN_H
#define _DEV_CAN_CAN_H

#include <sys/queue.h>

#include <stdatomic.h>
#include <rtems.h>
#include <rtems/seterr.h>
#include <rtems/thread.h>
#include <sys/ioccom.h>

#include <dev/can/can-stats.h>
#include <dev/can/can-bittiming.h>
#include <dev/can/can-frame.h>
#include <dev/can/can-filter.h>

#define CAN_IOC_MAGIC 'd'

/**
 * @defgroup CANChip
 *
 * @ingroup CANFDStack
 *
 * @{
 */

/**
 * @name RTEMS CAN Chip Flags
 *
 * @{
 */

/**
 * @brief This define provides controller flags determining whether the chip
 * is configured and ready to be started.
 */
#define RTEMS_CAN_CHIP_CONFIGURED ( 0 )
/**
 * @brief This define provides controller flags determining whether the chip
 * is currently running or not.
 */
#define RTEMS_CAN_CHIP_RUNNING    ( 1 )

/**
 * @name RTEMS CAN Chip Capabilities
 *
 * @{
 */

/**
 * @brief This define provides controller flags determining whether the chip
 *  is CAN FD capable.
 */
#define RTEMS_CAN_CHIP_CAPABILITIES_FD             ( 1 << 0 )
/**
 * @brief This define provides controller flags determining whether the chip
 *  has TX timestamping support.
 */
#define RTEMS_CAN_CHIP_CAPABILITIES_TX_TIMESTAMP   ( 1 << 1 )
/**
 * @brief This define provides controller flags determining whether the chip
 *  has TX timestamping support.
 */
#define RTEMS_CAN_CHIP_CAPABILITIES_RX_TIMESTAMP   ( 1 << 2 )

/** @} */

/**
 * @name RTEMS CAN Chip Info.
 *
 * Arguments that can be used with @ref RTEMS_CAN_CHIP_GET_INFO IOCTL command.
 *
 * @{
 */

/**
 * @brief This define specifies user wants to obtain controller's nominal
 *  bitrate via @ref RTEMS_CAN_CHIP_GET_INFO ioctl.
 */
#define RTEMS_CAN_CHIP_BITRATE        ( 1 )
/**
 * @brief This define specifies user wants to obtain controller's data bitrate
 *  via @ref RTEMS_CAN_CHIP_GET_INFO ioctl.
 */
#define RTEMS_CAN_CHIP_DBITRATE       ( 2 )
/**
 * @brief This define specifies user wants to obtain number of users using
 *  the controller via @ref RTEMS_CAN_CHIP_GET_INFO ioctl.
 */
#define RTEMS_CAN_CHIP_NUSERS         ( 3 )
/**
 * @brief This define specifies user wants to obtain controller's flags
 *  via @ref RTEMS_CAN_CHIP_GET_INFO ioctl. Refer to @ref CANChip
 *  for flags definitions.
 */
#define RTEMS_CAN_CHIP_FLAGS          ( 4 )
/**
 * @brief This define specifies user wants to obtain currently set
 *  controller's modes via @ref RTEMS_CAN_CHIP_GET_INFO ioctl.
 */
#define RTEMS_CAN_CHIP_MODE           ( 5 )
/**
 * @brief This define specifies user wants to obtain modes supported
 *  by the controller via @ref RTEMS_CAN_CHIP_GET_INFO ioctl.
 */
#define RTEMS_CAN_CHIP_MODE_SUPPORTED ( 6 )

/** @} */
/** @} */

/**
 * @name RTEMS CAN Queue Direction Helpers
 *
 * @{
 */

/**
 * @brief This define can be used to select queue RX (from controller to
 *  application) direction.
 */
#define RTEMS_CAN_QUEUE_RX     ( 1 << 0 )
/**
 * @brief This define can be used to select queue TX (from application to
 *  controller) direction.
 */
#define RTEMS_CAN_QUEUE_TX     ( 1 << 1 )

/** @} */

/**
 * @name RTEMS CAN Modes
 *
 * These can be set with @ref RTEMS_CAN_CHIP_SET_MODE command.
 *
 * @{
 */

/**
 * @brief This define is used to set loopback mode via
 *  @ref RTEMS_CAN_CHIP_SET_MODE ioctl.
 */
#define CAN_CTRLMODE_LOOPBACK       ( 1 << 0 )
/**
 * @brief This define is used to set listen only mode via
 *  @ref RTEMS_CAN_CHIP_SET_MODE ioctl.
 */
#define CAN_CTRLMODE_LISTENONLY     ( 1 << 1 )
/**
 * @brief This define is used to set triple sampling mode via
 *  @ref RTEMS_CAN_CHIP_SET_MODE ioctl.
 */
#define CAN_CTRLMODE_3_SAMPLES      ( 1 << 2 )
/**
 * @brief This define is used to set one shot mode via
 *  @ref RTEMS_CAN_CHIP_SET_MODE ioctl.
 */
#define CAN_CTRLMODE_ONE_SHOT       ( 1 << 3 )
/**
 * @brief This define is used to enable bus error reporting via
 *  @ref RTEMS_CAN_CHIP_SET_MODE ioctl.
 */
#define CAN_CTRLMODE_BERR_REPORTING ( 1 << 4 )
/**
 * @brief This define is used to set CAN FD mode via
 *  @ref RTEMS_CAN_CHIP_SET_MODE ioctl.
 */
#define CAN_CTRLMODE_FD             ( 1 << 5 )
/**
 * @brief This define is used to set to ignore missing CAN ack via
 *  @ref RTEMS_CAN_CHIP_SET_MODE ioctl.
 */
#define CAN_CTRLMODE_PRESUME_ACK    ( 1 << 6 )
/**
 * @brief This define is used to set CAN FD in non-ISO mode via
 *  @ref RTEMS_CAN_CHIP_SET_MODE ioctl.
 */
#define CAN_CTRLMODE_FD_NON_ISO     ( 1 << 7 )
/**
 * @brief This define is used to set classic CAN DLC option via
 *  @ref RTEMS_CAN_CHIP_SET_MODE ioctl.
 */
#define CAN_CTRLMODE_CC_LEN8_DLC    ( 1 << 8 )
/**
 * @brief This define is used to let CAN transiver automatically calculates TDCV
 *  via @ref RTEMS_CAN_CHIP_SET_MODE ioctl.
 */
#define CAN_CTRLMODE_TDC_AUTO       ( 1 << 9 )
/**
 * @brief This define is used to let TDCV calculate manually by user
 *  via @ref RTEMS_CAN_CHIP_SET_MODE ioctl.
 */
#define CAN_CTRLMODE_TDC_MANUAL     ( 1 << 10 )
#define CAN_CTRLMODE_MASK  ( CAN_CTRLMODE_LOOPBACK | CAN_CTRLMODE_LISTENONLY | \
                             CAN_CTRLMODE_3_SAMPLES | CAN_CTRLMODE_ONE_SHOT | \
                             CAN_CTRLMODE_BERR_REPORTING | CAN_CTRLMODE_FD | \
                             CAN_CTRLMODE_PRESUME_ACK | CAN_CTRLMODE_FD_NON_ISO | \
                             CAN_CTRLMODE_CC_LEN8_DLC | CAN_CTRLMODE_TDC_AUTO | \
                             CAN_CTRLMODE_TDC_MANUAL)

/** @} */

/**
 * @defgroup CANIoctl
 *
 * @ingroup CANFDStack
 *
 * @{
 */

/**
 * @name RTEMS CAN Stack Supported IOCTL calls.
 *
 * @{
 */

/**
 * @brief This ioctl call starts the controller. This is required in order
 *  to perform read/write operations. It has no effect if the chip is already
 *  started.
 */
#define RTEMS_CAN_CHIP_START         _IO( CAN_IOC_MAGIC, 1 )
/**
 * @brief This ioctl call stops the controller. It has no effect if the chip
 *  is already stopped.
 */
#define RTEMS_CAN_CHIP_STOP          _IOW( CAN_IOC_MAGIC, 2, struct timespec )
/**
 * @brief This ioctl call sets close operation to be blocking or nonblocking
 *  based on input parameter.
 */
#define RTEMS_CAN_CLOSE_NONBLOCK     _IO( CAN_IOC_MAGIC, 3 )
/**
 * @brief This ioctl call discards all RX/TX queues. Direction can be
 *  set with uint8_t parameter, see @ref RTEMS_CAN_QUEUE_RX and
 *  @ref RTEMS_CAN_QUEUE_TX.
 */
#define RTEMS_CAN_DISCARD_QUEUES     _IO( CAN_IOC_MAGIC, 4 )
/**
 * @brief This ioctl call flushes all RX/TX queues. Direction can be
 *  set with uint8_t parameter, see @ref RTEMS_CAN_QUEUE_RX and
 *  @ref RTEMS_CAN_QUEUE_TX.
 */
#define RTEMS_CAN_FLUSH_QUEUES       _IO( CAN_IOC_MAGIC, 5 )
/**
 * @brief This ioctl call sets controller's mode. Modes are
 *  passed uint32_t argument. Writing mode unsupported by the controller
 *  results in error as well as setting mode when the chip is already started.
 */
#define RTEMS_CAN_CHIP_SET_MODE      _IO( CAN_IOC_MAGIC, 6 )
/**
 * @brief This ioctl call obtains controller's info specified by input integer
 *  argument
 */
#define RTEMS_CAN_CHIP_GET_INFO      _IO( CAN_IOC_MAGIC, 7 )
/**
 * @brief This ioctl call waits until all TX messages from all FIFOs are
 *  sent to the network or until timeout defined in struct timespec. The
 *  timeout is specified as a relative timeout. Passing NULL indicates
 *  an infinite timeout.
 */
#define RTEMS_CAN_WAIT_TX_DONE       _IOW( CAN_IOC_MAGIC, 8, struct timespec )
/**
 * @brief This ioctl call waits until there is a free space in any TX
 *  FIFO queue or until timeout defined in struct timespec. The
 *  timeout is specified as a relative timeout. Passing NULL indicates
 *  an infinite timeout.
 */
#define RTEMS_CAN_POLL_TX_READY      _IOW( CAN_IOC_MAGIC, 9, struct timespec )
/**
 * @brief This ioctl call waits until there is an available frame in any RX
 *  FIFO queue or until timeout defined in struct timespec. The
 *  timeout is specified as a relative timeout. Passing NULL indicates
 *  an infinite timeout.
 */
#define RTEMS_CAN_POLL_RX_AVAIL      _IOW( CAN_IOC_MAGIC, 10, struct timespec )
/**
 * @brief This ioctl call creates new queue. Parameters are set with
 *  @ref rtems_can_queue_param structure.
 */
#define RTEMS_CAN_CREATE_QUEUE       _IOW( CAN_IOC_MAGIC, 11, struct rtems_can_queue_param )
/**
 * @brief This ioctl call sets controller's bitrate. See structure
 *  @ref rtems_can_set_bittiming.
 */
#define RTEMS_CAN_SET_BITRATE        _IOW( CAN_IOC_MAGIC, 12, struct rtems_can_set_bittiming )
/**
 * @brief This ioctl call obtains 64 unsigned timestamp from the controller.
 */
#define RTEMS_CAN_CHIP_GET_TIMESTAMP _IOR( CAN_IOC_MAGIC, 13, uint64_t )
/**
 * @brief This ioctl call obtains controller's statistics via
 *  @ref rtems_can_stats.
 */
#define RTEMS_CAN_CHIP_STATISTICS    _IOR( CAN_IOC_MAGIC, 14, struct rtems_can_stats )
/**
 * @brief This ioctl call gets controller's bitrate. See structure
 *  @ref rtems_can_set_bittiming.
 */
#define RTEMS_CAN_GET_BITTIMING      _IOWR( CAN_IOC_MAGIC, 15, struct rtems_can_get_bittiming )

/** @} */

/**
 * @brief This structure represents parameters of FIFO queue. It is used to
 *  setup new queues via @ref RTEMS_CAN_CREATE_QUEUE ioctl call.
 */
struct rtems_can_queue_param {
  /**
   * @brief This member specifies queue's direction. Use
   *  @ref RTEMS_CAN_QUEUE_RX or @ref RTEMS_CAN_QUEUE_TX
   */
  uint8_t direction;
  /**
   * @brief This member specifies queue's priority. Maximum priority value
   *  is available from @ref RTEMS_CAN_QUEUE_PRIO_NR define. Higher number
   *  means higher priority.
   */
  uint8_t priority;
  /**
   * @brief This member specifies queue's maximum data length. Passing 0
   *  applies default value: @ref CAN_FRAME_STANDARD_DLEN for standard
   *  frames only and @ref CAN_FRAME_FD_DLEN for CAN FD capable chips.
   */
  uint8_t dlen_max;
  /**
   * @brief This member specifies queue's buffer size. Passing 0 applies
   *  default @ref RTEMS_CAN_FIFO_SIZE value.
   */
  uint8_t buffer_size;
  /**
   * @brief This member holds a queue's filter. Refer to @ref rtems_can_filter
   *  for more information.
   */
  struct rtems_can_filter filter;
};

/** @} */

#endif
