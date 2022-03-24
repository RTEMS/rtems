/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @brief Console Driver for all Boards
 *
 * This file describes the Console Device Driver for all boards.
 * This driver provides support for the standard C Library.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
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

#ifndef _RTEMS_CONSOLE_H
#define _RTEMS_CONSOLE_H

#include <rtems/io.h> /* rtems_device_driver */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This macro defines the standard name for the console device
 * that is available to applications.
 */
#define CONSOLE_DEVICE_NAME "/dev/console"

/**
 * This macro defines the standard device driver table entry for
 * a console device driver.
 */
#define CONSOLE_DRIVER_TABLE_ENTRY \
  { console_initialize, console_open, console_close, \
    console_read, console_write, console_control }

/**
 * @brief Console initialization entry point.
 *
 * This method initializes the console device driver.
 *
 * @param[in] major is the device driver major number.
 * @param[in] minor is the device driver minor number.
 * @param[in] arg is the parameters to this call.
 *
 * @retval RTEMS_SUCCESSFUL The device driver is successfully initialized.
 */
rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 * @brief Console open entry point.
 *
 * This method opens a specific device supported by the
 * console device driver.
 *
 * @param[in] major is the device driver major number
 * @param[in] minor is the device driver minor number
 * @param[in] arg is the parameters to this call
 *
 * @retval RTEMS_SUCCESSFUL The device driver is successfully opened.
 */
rtems_device_driver console_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 * @brief Console close entry point.
 *
 * This method closes a specific device supported by the
 * console device driver.
 *
 * @param[in] major is the device driver major number
 * @param[in] minor is the device driver minor number
 * @param[in] arg is the parameters to this call
 *
 * @retval RTEMS_SUCCESSFUL The device driver is successfully closed.
 */
rtems_device_driver console_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 * @brief Console read entry point.
 *
 * This method reads from a specific device supported by the
 * console device driver.
 *
 * @param[in] major is the device driver major number
 * @param[in] minor is the device driver minor number
 * @param[in] arg is the parameters to this call
 *
 * @retval RTEMS_SUCCESSFUL The device is successfully read from.
 */
rtems_device_driver console_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 * @brief Console write entry point.
 *
 * This method writes to a specific device supported by the
 * console device driver.
 *
 * @param[in] major is the device driver major number
 * @param[in] minor is the device driver minor number
 * @param[in] arg is the parameters to this call
 *
 * @retval RTEMS_SUCCESSFUL The device is successfully written.
 */
rtems_device_driver console_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 * @brief Console IO control entry point.
 *
 * This method performs an IO Control operation on a
 * specific device supported by the console device driver.
 *
 * @param[in] major is the device driver major number
 * @param[in] minor is the device driver minor number
 * @param[in] arg is the parameters to this call
 * 
 * @retval RTEMS_SUCCESSFUL the device driver IO control operation is
 *         successfully performed.
 */
rtems_device_driver console_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 * @brief Initializes a simple console device.
 *
 * See CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER documentation in the
 * "RTEMS Classic API Guide".
 */
void _Console_simple_Initialize( void );

/**
 * @brief Initializes a simple task console device.
 *
 * See CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER documentation in
 * the "RTEMS Classic API Guide".
 */
void _Console_simple_task_Initialize( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
