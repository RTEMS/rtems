/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RTEMS /dev/zero Device Driver
 *
 * This include file defines the interface to the RTEMS /dev/zero
 * device driver.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_DEVZERO_H
#define _RTEMS_DEVZERO_H

#include <rtems/io.h>

/**
 *  @defgroup libmisc_devzero Zero Device Driver
 *
 *  @ingroup RTEMSDeviceDrivers
 */
/**@{*/

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
/**@}*/
#endif /* _RTEMS_DEVZERO_H */
