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
#include <bsp/bootcard.h>
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
#define FILE_HEADER_EPCQ_OFFSET                     0x100000
#define FILE_HEADER_VER_SIZE_VERSION                11
/* Local Structures */
typedef struct
{
  uint32_t offset;
  uint32_t size;
  uint32_t crc;
  char version[FILE_HEADER_VER_SIZE_VERSION];
}file_header_t;
/* Local Functions */
/* Local Variables */
static char software_version[VERSION_SIZE];

/* system_initialize --
 *     This routine registers the system device
 *
 * PARAMETERS:
 *     major - major led device number
 *     minor - miled led device number (not used)
 *     arg - device initialize argument
 *
 * RETURNS:
 *     RTEMS error code (RTEMS_SUCCESSFUL if device initialized successfuly)
 */
rtems_device_driver system_initialize (
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  (void) arg;

  /* Local Variables */
  rtems_status_code status;
  file_header_t fhead;

  /*
   * Register the devices
   */
  status = rtems_io_register_name ("/dev/system", major, minor );
  assert( status == RTEMS_SUCCESSFUL );

  /* read the file header information */
  epcq_read_buffer(
    FILE_HEADER_EPCQ_OFFSET,
    ( uint8_t * )&fhead,
    sizeof( fhead )
  );

  memset( &software_version[0], 0, sizeof( software_version ));
  strncpy( software_version, fhead.version, FILE_HEADER_VER_SIZE_VERSION );

  return RTEMS_SUCCESSFUL;
}

/* system_open --
 *     Open system device.
 *
 * PARAMETERS:
 *     major - major device number for led devices
 *     minor - miled device number for led
 *     arg - device opening argument
 *
 * RETURNS:
 *     RTEMS error code
 */
rtems_device_driver system_open (
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

/* system_close --
 *     Close system device.
 *
 * PARAMETERS:
 *     major - major device number for led devices
 *     minor - miled device number for led
 *     arg - device close argument
 *
 * RETURNS:
 *     RTEMS error code
 */
rtems_device_driver system_close (
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

/* system_read --
 *     Read from the system device
 *
 * PARAMETERS:
 *     major - major device number for led devices
 *     minor - miled device number for led
 *     arg - device read argument
 *
 * RETURNS:
 *     RTEMS error code
 */
rtems_device_driver system_read (
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

/* system_write --
 *     Write to the system device
 *
 * PARAMETERS:
 *     major - major device number for led devices
 *     minor - miled device number for led
 *     arg - device write argument
 *
 * RETURNS:
 *     RTEMS error code
 */
rtems_device_driver system_write (
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

/* system_control --
 *     Handle system I/O control (IOCTL)
 *
 * PARAMETERS:
 *     major - major device number for system devices
 *     minor - miled device number for system
 *     arg - device ioctl argument
 *
 * RETURNS:
 *     RTEMS error code
 */
rtems_device_driver system_control (
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  (void) major;
  (void) minor;

  /* Local Varaibles */
  rtems_libio_ioctl_args_t *args = arg;
  system_control_t *ctrl;

  /* Get Data */
  ctrl = ( system_control_t * )args->buffer;
  args->ioctl_return = 0;

  /* Parse Command */
  switch( args->command ) {
    case IOCTL_SYSTEM_SOFTWARE_VERSION :
      strcpy( ctrl->version, software_version );
      break;
    case IOCTL_SYSTEM_RESET :
      bsp_reset( ( rtems_fatal_source )0, ( rtems_fatal_code )0 );
      break;
    default :
      args->ioctl_return = -1;
      break;
  }
  /* OK */
  return RTEMS_SUCCESSFUL;
}
