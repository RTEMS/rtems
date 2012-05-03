/**
 * @file rtems/watchdogdrv.h
 *
 *  This file describes the Watchdog Driver for all boards.
 *  A watchdog is a hardware device that will reset the board
 *  if not touched in a specific way at a regular interval.
 *  It is a simple, yet important, part of many embedded systems.
 */

/*
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_WATCHDOGDRV_H
#define _RTEMS_WATCHDOGDRV_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This macro defines the watchdog device driver entry points.
 */
#define WATCHDOG_DRIVER_TABLE_ENTRY \
  { Watchdog_initialize, NULL, NULL, NULL, NULL, Watchdog_control }

/**
 *  @brief Watchdog Driver Initialization
 *
 *  This method initializes the watchdog hardware device.  The device
 *  should be initialized as DISABLED since BSP initialization may
 *  take longer than the timeout period for the watchdog.
 *
 *  @param[in] major is the watchdog device major number
 *  @param[in] minor is the watchdog device minor number
 *  @param[in] arguments points to device driver arguments
 */
rtems_device_driver Watchdog_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arguments
);

/**
 *  @brief Watchdog Driver IO Control
 *
 *  This method implements the IO Control device driver entry
 *  point for the watchdog hardware device.
 *
 *  @param[in] major is the watchdog device major number
 *  @param[in] minor is the watchdog device minor number
 *  @param[in] arguments points to device driver arguments
 */
rtems_device_driver Watchdog_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arguments
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
