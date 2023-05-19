/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RTEMS Port of Linux I2C Device API
 *
 * @ingroup I2CLinux
 */

/*
 * Copyright (c) 2014 embedded brains GmbH & Co. KG
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

#ifndef _UAPI_LINUX_I2C_DEV_H
#define _UAPI_LINUX_I2C_DEV_H

#include <stdint.h>

/**
 * @addtogroup I2CLinux
 *
 * @{
 */

/**
 * @name I2C IO Control Commands
 *
 * @{
 */

/**
 * @brief Sets the count of transfer retries in case a slave
 * device does not acknowledge a transaction.
 *
 * The argument type is unsigned long.
 */
#define I2C_RETRIES 0x701

/**
 * @brief Sets the transfer timeout in 10ms units.
 *
 * The argument type is unsigned long.
 */
#define I2C_TIMEOUT 0x702

/**
 * @brief Sets the slave address.
 *
 * It is an error to set a slave address already used by another slave device.
 *
 * The argument type is unsigned long.
 */
#define I2C_SLAVE 0x703

/**
 * @brief Forces setting the slave address.
 *
 * The argument type is unsigned long.
 */
#define I2C_SLAVE_FORCE 0x706

/**
 * @brief Enables 10-bit addresses if argument is non-zero, otherwise
 * disables 10-bit addresses.
 *
 * The argument type is unsigned long.
 */
#define I2C_TENBIT 0x704

/**
 * @brief Gets the I2C controller functionality information.
 *
 * The argument type is a pointer to an unsigned long.
 */
#define I2C_FUNCS 0x705

/**
 * @brief Performs a combined read/write transfer.
 *
 * Only one stop condition is signalled.
 *
 * The argument type is a pointer to struct i2c_rdwr_ioctl_data.
 */
#define I2C_RDWR 0x707

/**
 * @brief Enables System Management Bus (SMBus) Packet Error Checking (PEC)
 * if argument is non-zero, otherwise disables PEC.
 *
 * The argument type is unsigned long.
 */
#define I2C_PEC 0x708

/**
 * @brief Performs an SMBus transfer.
 *
 * The argument type is a pointer to struct i2c_smbus_ioctl_data.
 */
#define I2C_SMBUS 0x720

/** @} */

/**
 * @brief Argument type for I2C_SMBUS IO control call.
 */
struct i2c_smbus_ioctl_data {
  uint8_t read_write;
  uint8_t command;
  uint32_t size;
  union i2c_smbus_data *data;
};

/**
 * @brief Argument type for I2C_RDWR IO control call.
 */
struct i2c_rdwr_ioctl_data {
  struct i2c_msg *msgs;
  uint32_t nmsgs;
};

/**
 * @brief Maximum count of messages for one IO control call.
 */
#define I2C_RDRW_IOCTL_MAX_MSGS 42

/** @} */

#endif /* _UAPI_LINUX_I2C_DEV_H */
