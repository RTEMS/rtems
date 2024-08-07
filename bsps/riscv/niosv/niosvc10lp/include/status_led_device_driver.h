/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *
 * Copyright (C) 2024 Kevin Kirspel
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _STATUS_LED_DRIVER_H
#define _STATUS_LED_DRIVER_H

/* Definitions */
/* IOCTL Definitions */
#define IOCTL_STATUS_LED_TURN_ON                 0x00
#define IOCTL_STATUS_LED_TURN_OFF                0x01

#define STATUS_LED_1                             0x01
#define STATUS_LED_2                             0x02
#define STATUS_LED_3                             0x04
#define STATUS_LED_4                             0x08

/* Global Structure definitions */
typedef struct StatusLedControlStruct
{
  uint32_t led_mask;
}status_led_control_t;

#ifdef __cplusplus
extern "C" {
#endif

#define STATUS_LED_DRIVER_TABLE_ENTRY \
  { status_led_initialize, status_led_open, status_led_close, \
    status_led_read, status_led_write, status_led_control }

rtems_device_driver status_led_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver status_led_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver status_led_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver status_led_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver status_led_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver status_led_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

