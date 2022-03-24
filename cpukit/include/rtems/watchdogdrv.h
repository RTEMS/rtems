/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
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
