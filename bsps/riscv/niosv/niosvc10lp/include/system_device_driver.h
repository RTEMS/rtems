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

#ifndef _SYSTEM_DRIVER_H
#define _SYSTEM_DRIVER_H

/* Definitions */
/* IOCTL Definitions */
#define IOCTL_SYSTEM_SOFTWARE_VERSION        0x00
#define IOCTL_SYSTEM_RESET                   0x01

#define VERSION_SIZE                         20

/* Global Structure definitions */
typedef struct SystemControlStruct
{
  char version[VERSION_SIZE];
}system_control_t;

#ifdef __cplusplus
extern "C" {
#endif

#define SYSTEM_DRIVER_TABLE_ENTRY \
  { system_initialize, system_open, system_close, \
    system_read, system_write, system_control }

rtems_device_driver system_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver system_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver system_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver system_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver system_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver system_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

