/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief RTEMS DeviceIO Support
 *  @ingroup Device
 */

/*
 *  COPYRIGHT (c) 1989-2013.
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

#include <rtems/deviceio.h>
#include <rtems/rtems/status.h>

int rtems_deviceio_open(
  rtems_libio_t *iop,
  const char *path,
  int oflag,
  mode_t mode,
  rtems_device_major_number major,
  rtems_device_minor_number minor
)
{
  rtems_status_code status;
  rtems_libio_open_close_args_t args;

  args.iop = iop;
  args.flags = rtems_libio_iop_flags( iop );
  args.mode = mode;

  status = rtems_io_open( major, minor, &args );

  return rtems_status_code_to_errno( status );
}

int rtems_deviceio_close(
  rtems_libio_t *iop,
  rtems_device_major_number major,
  rtems_device_minor_number minor
)
{
  rtems_status_code status;
  rtems_libio_open_close_args_t args;

  args.iop = iop;
  args.flags = 0;
  args.mode = 0;

  status = rtems_io_close( major, minor, &args );

  return rtems_status_code_to_errno( status );
}

ssize_t rtems_deviceio_read(
  rtems_libio_t *iop,
  void *buf,
  size_t nbyte,
  rtems_device_major_number major,
  rtems_device_minor_number minor
)
{
  rtems_status_code status;
  rtems_libio_rw_args_t args;

  args.iop = iop;
  args.offset = iop->offset;
  args.buffer = buf;
  args.count = nbyte;
  args.flags = rtems_libio_iop_flags( iop );
  args.bytes_moved = 0;

  status = rtems_io_read( major, minor, &args );
  if ( status == RTEMS_SUCCESSFUL ) {
    iop->offset += args.bytes_moved;

    return (ssize_t) args.bytes_moved;
  } else {
    return rtems_status_code_to_errno( status );
  }
}

ssize_t rtems_deviceio_write(
  rtems_libio_t *iop,
  const void *buf,
  size_t nbyte,
  rtems_device_major_number major,
  rtems_device_minor_number minor
)
{
  rtems_status_code status;
  rtems_libio_rw_args_t args;

  args.iop = iop;
  args.offset = iop->offset;
  args.buffer = RTEMS_DECONST( void *, buf );
  args.count = nbyte;
  args.flags = rtems_libio_iop_flags( iop );
  args.bytes_moved = 0;

  status = rtems_io_write( major, minor, &args );
  if ( status == RTEMS_SUCCESSFUL ) {
    iop->offset += args.bytes_moved;

    return (ssize_t) args.bytes_moved;
  } else {
    return rtems_status_code_to_errno( status );
  }
}

int rtems_deviceio_control(
  rtems_libio_t *iop,
  ioctl_command_t command,
  void *buffer,
  rtems_device_major_number major,
  rtems_device_minor_number minor
)
{
  rtems_status_code status;
  rtems_libio_ioctl_args_t args;

  args.iop = iop;
  args.command = command;
  args.buffer = buffer;

  status = rtems_io_control( major, minor, &args );
  if ( status == RTEMS_SUCCESSFUL ) {
    return args.ioctl_return;
  } else {
    return rtems_status_code_to_errno(status);
  }
}
