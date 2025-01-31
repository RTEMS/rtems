/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup CANFDStack
 *
 * @brief This header file is part of CAN/CAN FD bus common support. It
 *        declares structures and functions used for controller's side of
 *        FIFO operations.
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

#ifndef _DEV_CAN_CAN_DEVCOMMON_H
#define _DEV_CAN_CAN_DEVCOMMON_H

#include <rtems.h>
#include <stdint.h>
#include <stdatomic.h>
#include <sys/ioccom.h>
#include <rtems/thread.h>

#include <dev/can/can.h>
#include <dev/can/can-bus.h>
#include <dev/can/can-impl.h>

/**
 * @brief This structure holds the controller's (application) side of
 *  queue's ends.
 */
struct rtems_can_queue_ends_dev {
  /**
   * @brief This member holds base @ref rtems_can_queue_ends structure.
   */
  struct rtems_can_queue_ends base;
  /**
   * @brief This member holds the chip's structure.
   */
  struct rtems_can_chip *chip;
  /**
   * @brief This member holds the worker semaphore used to trigger controller
   *  when there is a new message to be transmitted.
   */
  rtems_binary_semaphore worker_sem;
};

struct rtems_can_user;
TAILQ_HEAD( rtems_can_user_list_t, rtems_can_user );

struct rtems_can_chip;

/**
 * @brief This structure represents CAN controller operations. These
 *  provides interface from IOCTL calls to controller functions and should
 *  be registered during controller initialization.
 */
struct rtems_can_chip_ops {
  /**
   * @brief Starts the chip. Called with @ref RTEMS_CAN_CHIP_START ioctl.
   *
   *  The function should atomically set @ref RTEMS_CAN_CHIP_RUNNING
   *  flag in flags field of @ref rtems_can_chip structure and abort all
   *  currently filled HW buffers.
   *
   * @param  chip  Pointer to chip structure.
   *
   * @return 0 on success, negative value otherwise.
   */
  int ( *start_chip )( struct rtems_can_chip *chip );
  /**
   * @brief Stops the chip. Called with @ref RTEMS_CAN_CHIP_STOP ioctl.
   *
   *  The function should atomically clear @ref RTEMS_CAN_CHIP_RUNNING
   *  flag in flags field of @ref rtems_can_chip structure.
   *
   *  The function is blocking and passed with an option argument that
   *  provides the timeout (not blocking at all if argument is NULL). The
   *  function should abort all outgoing TX frames and clear the FIFO queues,
   *  returning the frames to the user as TX error frames. The flush takes
   *  places if those frames are not returned within the specified timeout.
   *
   * @param  chip  Pointer to @ref rtems_can_chip structure.
   * @param  ts    Pointer to the stop operation timeout.
   *
   * @return 0 on success, negative value otherwise.
   */
  int ( *stop_chip )( struct rtems_can_chip *chip, struct timespec *ts );
  /**
   * @brief Unrecognized ioctl calls are passed to chip specific
   *  function to handle them. This way chip specific ioctl calls can
   *  be implemented.
   *
   * @param  chip    Pointer to chip structure.
   * @param  command IOCTL command
   * @param  arg     Void pointer to IOCL argument
   *
   * @return 0 on success, negative value otherwise.
   */
  int ( *chip_ioctl )(
    struct rtems_can_chip *chip,
    ioctl_command_t command,
    void *arg
  );
  /**
   * @brief Obtains timestamp from the controller. Called with
   *  @ref RTEMS_CAN_CHIP_GET_TIMESTAMP ioctl.
   *
   * @param  chip      Pointer to chip structure.
   * @param  timestamp Pointer to uint64_t integer where timestamp is stored
   *
   * @return 0 on success, negative value otherwise.
   */
  int ( *get_chip_timestamp )(
    struct rtems_can_chip *chip,
    uint64_t *timestamp
  );
  /**
   * @brief Obtains controller's information specified by input integer
   *  argument. Called with @ref RTEMS_CAN_CHIP_GET_INFO ioctl.
   *
   * @param  chip  Pointer to chip structure.
   * @param  what  Integer specifying what info should be retrieved. Refer to
   *   @ref CANChip info defines.
   */
  int ( *get_chip_info )( struct rtems_can_chip *chip, int what );
  /**
   * @brief Calculates bit timing from given bit rate and saves the
   *  values to controller's registers. Called with
   *  @ref RTEMS_CAN_SET_BITRATE ioctl if @ref RTEMS_CAN_BITTIME_FROM_BITRATE
   *  is set.
   *
   * @param  chip  Pointer to chip structure.
   * @param  type  Bittiming type (nominal, FD)
   * @param  bt    Pointer to rtems_can_bittiming structure
   *
   * @return 0 on success, negated errno on error.
   * @retval -EPERM  Chip is already started, cannot change bittiming.
   * @retval -EINVAL Incorrect bit time type.
   */
  int ( *calc_bittiming )(
    struct rtems_can_chip *chip,
    int type,
    struct rtems_can_bittiming *bt
  );
  /**
   * @brief Checks bit timing given by user and saves it to controller's
   *  registers. Called with @ref RTEMS_CAN_SET_BITRATE ioctl if
   *  @ref RTEMS_CAN_BITTIME_FROM_PRECOMPUTED is set.
   *
   * @param  chip  Pointer to chip structure.
   * @param  type  Bittiming type (nominal, FD)
   * @param  bt    Pointer to rtems_can_bittiming structure
   *
   * @return 0 on success, negated errno on error.
   * @retval -EPERM  Chip is already started, cannot change bittiming.
   * @retval -EINVAL Incorrect bit time type.
   */
  int ( *check_and_set_bittiming )(
    struct rtems_can_chip *chip,
    int type,
    struct rtems_can_bittiming *bt
  );
  /**
   * @brief Sets controller mode defined in input argument. Called
   *  with @ref RTEMS_CAN_CHIP_SET_MODE.
   * @param  chip  Pointer to chip structure.
   * @param  mode  Modes to be set.
   *
   * @return 0 on success, negated errno on error.
   */
  int ( *set_chip_mode )( struct rtems_can_chip *chip, uint32_t mode );
};

/**
 * @brief This structure represents one CAN controller.
 */
struct rtems_can_chip {
  /**
   * @brief This member holds controller's type/name.
   */
  const char *type;
  /**
   * @brief This member holds controller's interrupt number.
   */
  rtems_vector_number irq;
  /**
   * @brief This member holds controller's base clock frequency.
   */
  uint32_t freq;
  /**
   * @brief This member informs whether close operation is blocking or
   *  nonblocking. This can be set with @ref RTEMS_CAN_CLOSE_NONBLOCK ioctl.
   */
  bool close_nonblock;
  /**
   * @brief This member holds controller's flags.
   */
  atomic_uint flags;
  /**
   * @brief This member holds the number of users using the controller.
   */
  atomic_uint used;
  /**
   * @brief This member holds the currently set mode. Controller's device
   *  driver may set initial modes if needed.
   */
  uint32_t ctrlmode;
  /**
   * @brief This member holds modes supported by the controller. Controller's
   *  device driver should set this field. Writing
   *  unsuported mode via @ref RTEMS_CAN_CHIP_SET_MODE ioctl results in error.
   */
  uint32_t ctrlmode_supported;
  /**
   * @brief This member holds controller's capabilities.
   */
  uint32_t capabilities;
  /**
   * @brief This member holds the lock to ensure atomicity of chip operations.
   */
  rtems_mutex lock;
  /**
   * @brief This member is used by the worker to notify the closed operation
   *  is finished.
   */
  rtems_binary_semaphore stop_sem;
  /**
   * @brief This member holds nominal bit timing constants (max/min values)
   */
  const struct rtems_can_bittiming_const *bittiming_const;
  /**
   * @brief This member holds data bit timing constants (max/min values)
   */
  const struct rtems_can_bittiming_const *data_bittiming_const;
  /**
   * @brief This member holds currently set nominal btt timing values.
   */
  struct rtems_can_bittiming bittiming;
  /**
   * @brief This member holds currently set data btt timing values.
   */
  struct rtems_can_bittiming data_bittiming;
  /**
   * @brief This member holds chip operations.
   */
  struct rtems_can_chip_ops chip_ops;
  /**
   * @brief This member holds chip's side of queue ends.
   */
  struct rtems_can_queue_ends_dev *qends_dev;
  /**
   * @brief This member holds the list of chip's users.
   */
  struct rtems_can_user_list_t can_users;
  /**
   * @brief This member holds the chip's statistics.
   */
  struct rtems_can_stats chip_stats;
  /**
   * @brief This member holds chip's specific private structure. This
   *  structure defines non generic fields and setting.
   */
  void *internal;
};

/**
 * @brief This structure holds the user's (application) side of queue's ends.
 */
struct rtems_can_queue_ends_user_t {
  /**
   * @brief This member holds base @ref rtems_can_queue_ends structure.
   */
  struct rtems_can_queue_ends base;
  /**
   * @brief This member holds semaphore informing the user's side there
   *  is a new message to be read.
   */
  rtems_binary_semaphore sem_read;
  /**
   * @brief This member holds semaphore informing the user's side there
   *  is a free space to write message.
   */
  rtems_binary_semaphore sem_write;
  /**
   * @brief This member holds semaphore synchronizing queues during close
   *  operation. It informs all messages were sent from the queue.
   */
  rtems_binary_semaphore sem_sync;
};

/**
 * @brief This structure represents one CAN user (application).
 */
struct rtems_can_user {
  /**
   * @brief This member holds user's flags.
   */
  unsigned long flags;
  /**
   * @brief This member holds TAILQ entry.
   */
  TAILQ_ENTRY( rtems_can_user ) peers;
  /**
   * @brief This member holds user's side of queue's ends.
   */
  struct rtems_can_queue_ends_user_t *qends_user;
  /**
   * @brief This member holds pointer to @ref rtems_can_bus structure.
   */
  struct rtems_can_bus *bus;
  /**
   * @brief This member holds user magic value. It should be set
   *  to @ref RTEMS_CAN_USER_MAGIC value.
   */
  int magic;
};

/**
 * @brief This function starts the controller
 *
 * Provides common interface to start controller based
 * on \ref rtems_can_chip structure passed as input parameter.
 *
 * @param chip   Pointer to \ref rtems_can_chip structure
 *
 * @return Zero on success, negative value on error.
 *
 */
int rtems_can_chip_start( struct rtems_can_chip *chip );

/**
 * @brief This function calculates CAN bit timing for given bit rate.
 *
 * This calculates bit timing values for given bit rate (provided in bt
 * input argument) and controller's maximum/minimal values given by btc
 * argument.
 *
 * @param chip  Pointer to @ref rtems_can_chip structure.
 * @param bt    Pointer to @ref rtems_can_bittiming structure. Calculated
 *              values are stored here.
 * @param btc   Pointer to @ref rtems_can_bittiming_const structure. This
 *              provides maximum and minimal bit timing values of the
 *              controller.
 *
 * @return Zero on success (and filled bt argument), negative value on error.
 *
 */
int rtems_can_bitrate2bittiming(
  struct rtems_can_chip *chip,
  struct rtems_can_bittiming *bt,
  const struct rtems_can_bittiming_const *btc
);

/**
 * @brief This function fills timestamping with current monotonic time
 *
 * @return 64-bit timestamp value.
 *
 */
uint64_t rtems_can_fill_timestamp( void );

/**
 * @brief Userspace clients specific ends initialization.
 *
 * @param qends_user Pointer to @ref rtems_can_queue_ends_user_t structure.
 *
 * @return Zero on success; -1 otherwise.
 *
 */
int rtems_can_queue_ends_init_user(
  struct rtems_can_queue_ends_user_t *qends
);

/**
 * @brief This function initializes ends from chip's side
 *
 * It should be called by CAN controller to initialize its ends.
 *
 * @param chip Pointer to @ref rtems_can_chip structure
 * @param name Name of the controller's worker semaphore
 *
 * @return Zero on success, negative value on error.
 *
 */
int rtems_can_queue_ends_init_chip(
  struct rtems_can_chip *chip,
  const char *name
);

#endif /* _DEV_CAN_CAN_DEVCOMMON_H */
