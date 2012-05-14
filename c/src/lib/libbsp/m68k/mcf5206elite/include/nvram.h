/*
 * nvram.h -- DS1307-based non-volatile memory device driver.
 *
 * This driver support file-like operations to 56-bytes long non-volatile
 * memory of DS1307 I2C real-time clock chip.
 *
 * Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef __DRIVER__NVRAM_H__
#define __DRIVER__NVRAM_H__

#ifdef __cplusplus
extern "C" {
#endif

#define NVRAM_DRIVER_TABLE_ENTRY \
  { nvram_driver_initialize, nvram_driver_open, nvram_driver_close, \
    nvram_driver_read, nvram_driver_write, NULL }

/* nvram_driver_initialize --
 *     Non-volatile memory device driver initialization.
 */
rtems_device_driver
nvram_driver_initialize(rtems_device_major_number major,
                        rtems_device_minor_number minor,
                        void *arg);

/* nvram_driver_open --
 *     Non-volatile memory device driver open primitive.
 */
rtems_device_driver
nvram_driver_open(rtems_device_major_number major,
                  rtems_device_minor_number minor,
                  void *arg);

/* nvram_driver_close --
 *     Non-volatile memory device driver close primitive.
 */
rtems_device_driver
nvram_driver_close(rtems_device_major_number major,
                   rtems_device_minor_number minor,
                   void *arg);

/* nvram_driver_read --
 *     Non-volatile memory device driver read primitive.
 */
rtems_device_driver
nvram_driver_read(rtems_device_major_number major,
                  rtems_device_minor_number minor,
                  void *arg);

/* nvram_driver_write --
 *     Non-volatile memory device driver write primitive.
 */
rtems_device_driver
nvram_driver_write(rtems_device_major_number major,
                   rtems_device_minor_number minor,
                   void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __VFDDRV_H__ */
