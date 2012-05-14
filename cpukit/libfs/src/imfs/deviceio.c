/*
 *  IMFS Device Node Handlers
 *
 *  This file contains the set of handlers used to map operations on
 *  IMFS device nodes onto calls to the RTEMS Classic API IO Manager.
 *
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

#include "imfs.h"

#include <rtems/deviceio.h>

int device_open(
  rtems_libio_t *iop,
  const char    *pathname,
  int            oflag,
  mode_t         mode
)
{
  IMFS_jnode_t                  *the_jnode;

  the_jnode  = iop->pathinfo.node_access;

  return rtems_deviceio_open(
    iop,
    pathname,
    oflag,
    mode,
    the_jnode->info.device.major,
    the_jnode->info.device.minor
  );
}

int device_close(
  rtems_libio_t *iop
)
{
  IMFS_jnode_t                  *the_jnode;

  the_jnode = iop->pathinfo.node_access;

  return rtems_deviceio_close(
    iop,
    the_jnode->info.device.major,
    the_jnode->info.device.minor
  );
}

ssize_t device_read(
  rtems_libio_t *iop,
  void          *buffer,
  size_t         count
)
{
  IMFS_jnode_t           *the_jnode;

  the_jnode = iop->pathinfo.node_access;

  return rtems_deviceio_read(
    iop,
    buffer,
    count,
    the_jnode->info.device.major,
    the_jnode->info.device.minor
  );
}

ssize_t device_write(
  rtems_libio_t *iop,
  const void    *buffer,
  size_t         count
)
{
  IMFS_jnode_t           *the_jnode;

  the_jnode = iop->pathinfo.node_access;

  return rtems_deviceio_write(
    iop,
    buffer,
    count,
    the_jnode->info.device.major,
    the_jnode->info.device.minor
  );
}

int device_ioctl(
  rtems_libio_t   *iop,
  ioctl_command_t  command,
  void            *buffer
)
{
  IMFS_jnode_t             *the_jnode;

  the_jnode = iop->pathinfo.node_access;

  return rtems_deviceio_control(
    iop,
    command,
    buffer,
    the_jnode->info.device.major,
    the_jnode->info.device.minor
  );
}

int device_ftruncate(
  rtems_libio_t *iop,
  off_t          length
)
{
  return 0;
}
