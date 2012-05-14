/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/deviceio.h>

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
  args.flags = iop->flags;
  args.mode = mode;

  status = rtems_io_open( major, minor, &args );

  return rtems_deviceio_errno( status );
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

  return rtems_deviceio_errno( status );
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
  args.flags = iop->flags;
  args.bytes_moved = 0;

  status = rtems_io_read( major, minor, &args );
  if ( status == RTEMS_SUCCESSFUL ) {
    iop->offset += args.bytes_moved;

    return (ssize_t) args.bytes_moved;
  } else {
    return rtems_deviceio_errno( status );
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
  args.buffer = buf;
  args.count = nbyte;
  args.flags = iop->flags;
  args.bytes_moved = 0;

  status = rtems_io_write( major, minor, &args );
  if ( status == RTEMS_SUCCESSFUL ) {
    iop->offset += args.bytes_moved;

    return (ssize_t) args.bytes_moved;
  } else {
    return rtems_deviceio_errno( status );
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
    return rtems_deviceio_errno(status);
  }
}
