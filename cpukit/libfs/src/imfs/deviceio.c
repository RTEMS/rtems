/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup IMFS
 *
 * @brief IMFS Device Node Handlers
 */

/*
 *  COPYRIGHT (c) 1989-2012.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/imfs.h>

#include <rtems/deviceio.h>

int device_open(
  rtems_libio_t *iop,
  const char    *pathname,
  int            oflag,
  mode_t         mode
)
{
  const IMFS_device_t *device = IMFS_iop_to_device( iop );

  return rtems_deviceio_open(
    iop,
    pathname,
    oflag,
    mode,
    device->major,
    device->minor
  );
}

int device_close(
  rtems_libio_t *iop
)
{
  const IMFS_device_t *device = IMFS_iop_to_device( iop );

  return rtems_deviceio_close(
    iop,
    device->major,
    device->minor
  );
}

ssize_t device_read(
  rtems_libio_t *iop,
  void          *buffer,
  size_t         count
)
{
  const IMFS_device_t *device = IMFS_iop_to_device( iop );

  return rtems_deviceio_read(
    iop,
    buffer,
    count,
    device->major,
    device->minor
  );
}

ssize_t device_write(
  rtems_libio_t *iop,
  const void    *buffer,
  size_t         count
)
{
  const IMFS_device_t *device = IMFS_iop_to_device( iop );

  return rtems_deviceio_write(
    iop,
    buffer,
    count,
    device->major,
    device->minor
  );
}

int device_ioctl(
  rtems_libio_t   *iop,
  ioctl_command_t  command,
  void            *buffer
)
{
  const IMFS_device_t *device = IMFS_iop_to_device( iop );

  return rtems_deviceio_control(
    iop,
    command,
    buffer,
    device->major,
    device->minor
  );
}

int device_ftruncate(
  rtems_libio_t *iop,
  off_t          length
)
{
  (void) iop;
  (void) length;

  return 0;
}
