/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Frame Buffer Device Driver for all Boards
 *
 * This file describes the Frame Buffer Device Driver for all boards.
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

#ifndef __RTEMS_FRAMEBUFFER_h__
#define __RTEMS_FRAMEBUFFER_h__

#include <rtems/io.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This macro defines the standard name for the frame buffer device
 *  that is available to applications.
 */
#define FRAMEBUFFER_DEVICE_NAME "/dev/fb"

/**
 * @brief Standard device file path of first frame buffer device.
 *
 * This device is the default frame buffer device for the Microwindows Screen
 * Driver.
 */
#define FRAMEBUFFER_DEVICE_0_NAME "/dev/fb0"

/**
 *  This macro defines the standard device driver table entry for
 *  a frame buffer device driver.
 */
#define FRAME_BUFFER_DRIVER_TABLE_ENTRY \
  { frame_buffer_initialize, frame_buffer_open, frame_buffer_close, \
    frame_buffer_read, frame_buffer_write, frame_buffer_control }

/**
 *  @brief Frame Buffer Initialization Entry Point
 *
 *  This method initializes the frame buffer device driver.
 *
 *  @param[in] major is the device driver major number
 *  @param[in] minor is the device driver minor number
 *  @param[in] arg is the parameters to this call
 *
 *  @return This method returns RTEMS_SUCCESSFUL when
 *          the device driver is successfully initialized.
 */
rtems_device_driver frame_buffer_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 *  @brief Frame Buffer Open Entry Point
 *
 *  This method opens a specific device supported by the
 *  frame buffer device driver.
 *
 *  @param[in] major is the device driver major number
 *  @param[in] minor is the device driver minor number
 *  @param[in] arg is the parameters to this call
 *
 *  @return This method returns RTEMS_SUCCESSFUL when
 *          the device driver is successfully opened.
 */
rtems_device_driver frame_buffer_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 *  @brief Frame Buffer Close Entry Point
 *
 *  This method closes a specific device supported by the
 *  frame buffer device driver.
 *
 *  @param[in] major is the device driver major number
 *  @param[in] minor is the device driver minor number
 *  @param[in] arg is the parameters to this call
 *
 *  @return This method returns RTEMS_SUCCESSFUL when
 *          the device is successfully closed.
 */
rtems_device_driver frame_buffer_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 *  @brief Frame Buffer Read Entry Point
 *
 *  This method reads from a specific device supported by the
 *  frame buffer device driver.
 *
 *  @param[in] major is the device driver major number
 *  @param[in] minor is the device driver minor number
 *  @param[in] arg is the parameters to this call
 *
 *  @return This method returns RTEMS_SUCCESSFUL when
 *          the device is successfully read from.
 */
rtems_device_driver frame_buffer_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 *  @brief Frame Buffer Write Entry Point
 *
 *  This method writes to a specific device supported by the
 *  frame buffer device driver.
 *
 *  @param[in] major is the device driver major number
 *  @param[in] minor is the device driver minor number
 *  @param[in] arg is the parameters to this call
 *
 *  @return This method returns RTEMS_SUCCESSFUL when
 *          the device is successfully written.
 */
rtems_device_driver frame_buffer_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 *  @brief Frame Buffer IO Control Entry Point
 *
 *  This method performs an IO Control operation on a
 *  specific device supported by the frame buffer device driver.
 *
 *  @param[in] major is the device driver major number
 *  @param[in] minor is the device driver minor number
 *  @param[in] arg is the parameters to this call
 *
 *  @return This method returns RTEMS_SUCCESSFUL when
 *          the device driver IO control operation is
 *          successfully performed.
 */
rtems_device_driver frame_buffer_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
