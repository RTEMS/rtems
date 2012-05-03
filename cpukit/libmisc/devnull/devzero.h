/**
 * @file rtems/devzero.h
 *
 * This include file defines the interface to the RTEMS /dev/zero
 * device driver.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_DEVZERO_H
#define _RTEMS_DEVZERO_H

#include <rtems/io.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DEVZERO_DEVICE_NAME "/dev/zero"

#define DEVZERO_DRIVER_TABLE_ENTRY \
  { \
    dev_zero_initialize, \
    dev_zero_open, \
    dev_zero_close, \
    dev_zero_read, \
    dev_zero_write, \
    dev_zero_control \
  }

rtems_device_driver dev_zero_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver dev_zero_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver dev_zero_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver dev_zero_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver dev_zero_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver dev_zero_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_DEVZERO_H */
