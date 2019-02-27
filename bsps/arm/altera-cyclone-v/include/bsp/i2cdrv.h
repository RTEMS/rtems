/**
 * @file
 *
 * @ingroup RTEMSBSPsARMCycVI2C
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
