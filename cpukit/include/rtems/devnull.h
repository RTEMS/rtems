/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RTEMS /dev/null Device Driver
 * 
 * This include file defines the interface to the RTEMS /dev/null
 * device driver.
 */

/*
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_DEVNULL_H
#define _RTEMS_DEVNULL_H

#include <rtems/io.h>

/**
 *  @defgroup libmisc_devnull Null Device Driver
 *
 *  @ingroup RTEMSDeviceDrivers
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#define DEVNULL_DRIVER_TABLE_ENTRY \
  { null_initialize, null_open, null_close, null_read, \
    null_write, null_control }

#define NULL_SUCCESSFUL RTEMS_SUCCESSFUL

rtems_device_driver null_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver null_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver null_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver null_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver null_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver null_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

#ifdef __cplusplus
}
#endif
/**@}*/
#endif
/* end of include file */
