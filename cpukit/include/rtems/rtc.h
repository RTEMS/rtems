/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Real-Time Clock Driver Interface
 * 
 * Real-time clock driver interface.
 */

/*
 *  COPYRIGHT (c) 1989-2001.
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

#ifndef _RTEMS_RTC_H
#define _RTEMS_RTC_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup rtems_rtc Real-Time Clock Driver Interface
 *
 * @ingroup RTEMSDeviceDrivers
 *
 * This driver interface provides support to read and set the real-time clock
 * and to initialize the time of day for the system.
 */
/**@{**/

/**
 * Device file name path.
 */
#define RTC_DEVICE_NAME "/dev/rtc"

/**
 * Device driver table entry.
 */
#define RTC_DRIVER_TABLE_ENTRY \
  { rtc_initialize, rtc_open, rtc_close, \
    rtc_read, rtc_write, rtc_control }

/**
 * Initializes the real-time clock device and sets the time of day for the
 * system.
 *
 * If the real-time clock provides an invalid time of day value the system time
 * of day must remain untouched.
 */
rtems_device_driver rtc_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

/**
 * Opens the real-time clock device.
 */
rtems_device_driver rtc_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

/**
 * Closes the real-time clock device.
 */
rtems_device_driver rtc_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

/**
 * Reads the real-time clock value.
 *
 * The value will be returned in a @ref rtems_time_of_day structure.
 */
rtems_device_driver rtc_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

/**
 * Sets the real-time clock value.
 *
 * The value will be set from a @ref rtems_time_of_day structure.
 */
rtems_device_driver rtc_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

/**
 * Controls the real-time clock.
 */
rtems_device_driver rtc_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
