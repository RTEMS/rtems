/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Switch NXP PCA9548A Driver API
 *
 * @ingroup I2CSWITCHNXPPCA9548A
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

#ifndef _DEV_I2C_SWITCH_NXP_PCA9548A_H
#define _DEV_I2C_SWITCH_NXP_PCA9548A_H

#include <dev/i2c/i2c.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup I2CSWITCHNXPPCA9548A Switch NXP PCA9535 Driver
 *
 * @ingroup I2CDevice
 *
 * @brief Driver for NXP PCA9548A 8-channel switch device.
 *
 * @{
 */

int i2c_dev_register_switch_nxp_pca9548a(
  const char *bus_path,
  const char *dev_path,
  uint16_t address
);

#define SWITCH_NXP_PCA9548A_GET_CONTROL (I2C_DEV_IO_CONTROL + 0)

#define SWITCH_NXP_PCA9548A_SET_CONTROL (I2C_DEV_IO_CONTROL + 1)

static inline int switch_nxp_pca9548a_get_control(int fd, uint8_t *val)
{
  return ioctl(fd, SWITCH_NXP_PCA9548A_GET_CONTROL, val);
}

static inline int switch_nxp_pca9548a_set_control(int fd, uint8_t val)
{
  return ioctl(fd, SWITCH_NXP_PCA9548A_SET_CONTROL, (void *)(uintptr_t) val);
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DEV_I2C_SWITCH_NXP_PCA9548A_H */
