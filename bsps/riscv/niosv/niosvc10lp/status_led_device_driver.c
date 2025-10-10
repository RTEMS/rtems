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

#include <bsp.h>
#include <stdio.h>
#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/error.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <bsp/fatal.h>
#include <bsp/irq.h>
#include <bsp/niosv.h>

#include <rtems/score/riscv-utility.h>

/* Local Definitaions */
/* Local Structures */
/* Local Functions */
static void status_led_turn_on( uint32_t index );
static void status_led_turn_off( uint32_t index );
/* Local Variables */

/* status_led_initialize --
 *     This routine registers the status led device
 *
 * PARAMETERS:
 *     major - major led device number
 *     minor - minor led device number (not used)
 *     arg - device initialize argument
 *
 * RETURNS:
 *     RTEMS error code (RTEMS_SUCCESSFUL if device initialized successfuly)
 */
rtems_device_driver status_led_initialize (
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  (void) arg;

  /* Local Variables */
  rtems_status_code status;

  /*
   * Register the devices
   */
  status = rtems_io_register_name ("/dev/status_led", major, minor );
  assert( status == RTEMS_SUCCESSFUL );

  return RTEMS_SUCCESSFUL;
}

/* status_led_open --
 *     Open status led device.
 *
 * PARAMETERS:
 *     major - major device number for led devices
 *     minor - minor device number for led
 *     arg - device opening argument
 *
 * RETURNS:
 *     RTEMS error code
 */
rtems_device_driver status_led_open (
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  return RTEMS_SUCCESSFUL;
}

/* status_led_close --
 *     Close status led device.
 *
 * PARAMETERS:
 *     major - major device number for led devices
 *     minor - minor device number for led
 *     arg - device close argument
 *
 * RETURNS:
 *     RTEMS error code
 */
rtems_device_driver status_led_close (
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  return RTEMS_SUCCESSFUL;
}

/* status_led_read --
 *     Read from the status led device
 *
 * PARAMETERS:
 *     major - major device number for led devices
 *     minor - minor device number for led
 *     arg - device read argument
 *
 * RETURNS:
 *     RTEMS error code
 */
rtems_device_driver status_led_read (
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  return RTEMS_SUCCESSFUL;
}

/* status_led_write --
 *     Write to the status led device
 *
 * PARAMETERS:
 *     major - major device number for led devices
 *     minor - minor device number for led
 *     arg - device write argument
 *
 * RETURNS:
 *     RTEMS error code
 */
rtems_device_driver status_led_write (
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  return RTEMS_SUCCESSFUL;
}

/* status_led_control --
 *     Handle status led device I/O control (IOCTL)
 *
 * PARAMETERS:
 *     major - major device number for led devices
 *     minor - minor device number for led
 *     arg - device ioctl argument
 *
 * RETURNS:
 *     RTEMS error code
 */
rtems_device_driver status_led_control (
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  (void) major;
  (void) minor;

  /* Local Varaibles */
  rtems_libio_ioctl_args_t *args = arg;
  status_led_control_t *ctrl;

  /* Get Data */
  ctrl = ( status_led_control_t * )args->buffer;
  args->ioctl_return = 0;

  /* Parse Command */
  switch( args->command )
  {
    case IOCTL_STATUS_LED_TURN_ON :
      status_led_turn_on(ctrl->led_mask);
      break;
    case IOCTL_STATUS_LED_TURN_OFF :
      status_led_turn_off(ctrl->led_mask);
      break;
    default :
      args->ioctl_return = -1;
      break;
  }
  /* OK */
  return RTEMS_SUCCESSFUL;
}

/* status_led_turn_on --
 *     Turns the LED on
 *
 * PARAMETERS:
 *
 * RETURNS:
 *
 */
void status_led_turn_on( uint32_t mask )
{
  LED_PIO_REGS->outclear = mask;
}

/* status_led_turn_off --
 *     Turns the LED on
 *
 * PARAMETERS:
 *
 * RETURNS:
 *
 */
void status_led_turn_off( uint32_t mask )
{
  LED_PIO_REGS->outset = mask;
}
