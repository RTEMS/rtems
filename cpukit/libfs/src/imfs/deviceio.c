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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
  return 0;
}
