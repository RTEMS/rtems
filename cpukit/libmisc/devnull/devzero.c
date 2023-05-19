/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libmisc_devnull Device Driver
 *
 * @brief Devzero
 */

/*
 * Copyright (c) 2011 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/devzero.h>

#include <string.h>

#include <rtems/libio.h>

rtems_device_driver dev_zero_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor RTEMS_UNUSED,
  void *arg RTEMS_UNUSED
)
{
  return rtems_io_register_name(DEVZERO_DEVICE_NAME, major, 0);
}

rtems_device_driver dev_zero_open(
  rtems_device_major_number major RTEMS_UNUSED,
  rtems_device_minor_number minor RTEMS_UNUSED,
  void *arg RTEMS_UNUSED
)
{
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver dev_zero_close(
  rtems_device_major_number major RTEMS_UNUSED,
  rtems_device_minor_number minor RTEMS_UNUSED,
  void *arg RTEMS_UNUSED
)
{
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver dev_zero_read(
  rtems_device_major_number major RTEMS_UNUSED,
  rtems_device_minor_number minor RTEMS_UNUSED,
  void *arg RTEMS_UNUSED
)
{
  rtems_libio_rw_args_t *rw = arg;

  rw->bytes_moved = rw->count;
  memset(rw->buffer, 0, rw->count);

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver dev_zero_write(
  rtems_device_major_number major RTEMS_UNUSED,
  rtems_device_minor_number minor RTEMS_UNUSED,
  void *arg
)
{
  rtems_libio_rw_args_t *rw = arg;

  rw->bytes_moved = rw->count;

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver dev_zero_control(
  rtems_device_major_number major RTEMS_UNUSED,
  rtems_device_minor_number minor RTEMS_UNUSED,
  void *arg RTEMS_UNUSED
)
{
  return RTEMS_IO_ERROR;
}
