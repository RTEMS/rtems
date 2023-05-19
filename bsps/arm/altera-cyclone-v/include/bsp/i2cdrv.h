/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMCycVI2C
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

#ifndef I2CDRV_H
#define I2CDRV_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSBSPsARMCycVI2C I2C Driver
 *
 * @ingroup RTEMSBSPsARMCycV
 *
 * @brief I2C Driver.
 *
 * @{
 */

rtems_device_driver i2cdrv_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver i2cdrv_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver i2cdrv_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver i2cdrv_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver i2cdrv_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver i2cdrv_ioctl(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

#define I2C_DRIVER_TABLE_ENTRY \
  { \
    i2cdrv_initialize, \
    i2cdrv_open, \
    i2cdrv_close, \
    i2cdrv_read, \
    i2cdrv_write, \
    i2cdrv_ioctl \
  }

#define I2C_IOC_SET_SLAVE_ADDRESS 1

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* I2CDRV_H */
