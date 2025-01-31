/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup CANFDStack
 *
 * @brief This header file is part of CAN/CAN FD bus common support. It
 *        declares structures and functions used for CAN bus registration.
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

#ifndef _DEV_CAN_CAN_BUS_H
#define _DEV_CAN_CAN_BUS_H

#include <rtems.h>
#include <stdint.h>
#include <stdatomic.h>
#include <sys/ioccom.h>
#include <rtems/thread.h>

struct rtems_can_chip;

/**
 * @brief This structure represents CAN bus device. This is the main
 *  structure passed throught file system during read/write/ioctl/close
 *  operations.
 *
 * The structure should be allocated by the user in the application.
 */
struct rtems_can_bus {
  /**
   * @brief This member holds the pointer to @ref rtems_can_chip structure.
   *  Controller specific initialization function should fill this
   *  structure properly.
   */
  struct rtems_can_chip *chip;
};

/**
 * @brief This function notifies all users the chip was stopped.
 *
 * This should be called from the chip's stop function once all
 * frames are either aborted or flushed from the FIFOs. It basically
 * unblocks users waiting on a blocking read.
 *
 * @param chip Pointer to @ref rtems_can_chip structure.
 *
 * @return Zero on success, negative value on error.
 *
 */
int rtems_can_bus_notify_chip_stop( struct rtems_can_chip *chip );

/**
 * @brief This function registers CAN device into /dev namespace.
 *
 * This should be called with already allocated @ref rtems_can_bus structure
 * and initialized controller to be registered.
 *
 * @param bus       Pointer to @ref rtems_can_bus structure.
 * @param bus_path  Entire path to which the device should be registered
 *                  (i.e /dev/can0, /dev/my_can etc.).
 *
 * @return Zero on success, negative value on error.
 *
 */
int rtems_can_bus_register( struct rtems_can_bus *bus, const char *bus_path );

#endif /* _DEV_CAN_CAN_DEVCOMMON_H */
