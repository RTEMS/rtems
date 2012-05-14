/**
 * @file
 *
 * Real-time clock driver interface.
 */

/*
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
 * This driver interface provides support to read and set the real-time clock
 * and to initialize the time of day for the system.
 *
 * @{
 */

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
