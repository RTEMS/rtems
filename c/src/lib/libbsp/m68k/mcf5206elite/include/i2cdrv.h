/*
 * i2cdrv.h -- I2C bus driver prototype and definitions
 *
 * Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef __I2CDRV_H__
#define __I2CDRV_H__

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_DRIVER_TABLE_ENTRY \
  { i2cdrv_initialize, NULL, NULL, NULL, NULL, NULL }

/* i2cdrv_initialize --
 *     I2C driver initialization (rtems I/O driver primitive)
 */
rtems_device_driver
i2cdrv_initialize(rtems_device_major_number major,
                  rtems_device_minor_number minor,
                  void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __I2CDRV_H__ */
