/*
 *  IMFS Device Node Handlers
 *
 *  This file contains the set of handlers used to map operations on
 *  IMFS device nodes onto calls to the RTEMS Classic API IO Manager.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/assoc.h>                /* assoc.h not included by rtems.h */
#include <errno.h>

#include "imfs.h"

/*
 * Convert RTEMS status to a UNIX errno
 */

rtems_assoc_t errno_assoc[] = {
    { "OK",                 RTEMS_SUCCESSFUL,                0 },
    { "BUSY",               RTEMS_RESOURCE_IN_USE,           EBUSY },
    { "INVALID NAME",       RTEMS_INVALID_NAME,              EINVAL },
    { "NOT IMPLEMENTED",    RTEMS_NOT_IMPLEMENTED,           ENOSYS },
    { "TIMEOUT",            RTEMS_TIMEOUT,                   ETIMEDOUT },
    { "NO MEMORY",          RTEMS_NO_MEMORY,                 ENOMEM },
    { "NO DEVICE",          RTEMS_UNSATISFIED,               ENODEV },
    { "INVALID NUMBER",     RTEMS_INVALID_NUMBER,            EBADF},
    { "NOT RESOURCE OWNER", RTEMS_NOT_OWNER_OF_RESOURCE,     EPERM},
    { "IO ERROR",           RTEMS_IO_ERROR,                  EIO},
    { 0, 0, 0 },
};

static unsigned32
rtems_deviceio_errno(rtems_status_code code)
{
    int rc;
    
    if ((rc = rtems_assoc_remote_by_local(errno_assoc, (unsigned32) code)))
    {
        errno = rc;
        return -1;
    }
    return -1;
}

/*
 *  device_open
 *
 *  This handler maps an open() operation onto rtems_io_open().
 */

int device_open(
  rtems_libio_t *iop,
  const char    *pathname,
  unsigned32     flag,
  unsigned32     mode
)
{
  rtems_libio_open_close_args_t  args;
  rtems_status_code              status;
  IMFS_jnode_t                  *the_jnode;

  the_jnode  = iop->file_info;

  args.iop   = iop;
  args.flags = iop->flags;
  args.mode  = mode;

  status = rtems_io_open(
    the_jnode->info.device.major,
    the_jnode->info.device.minor,
    (void *) &args
  );
  if ( status ) {
    rtems_deviceio_errno(status);
    return RTEMS_UNSATISFIED;
  }

  return 0;
}

/*
 *  device_close
 *
 *  This handler maps a close() operation onto rtems_io_close().
 */

int device_close(
  rtems_libio_t *iop
)
{
  rtems_libio_open_close_args_t  args;
  rtems_status_code              status;
  IMFS_jnode_t                  *the_jnode;

  the_jnode = iop->file_info;

  args.iop   = iop;
  args.flags = 0;
  args.mode  = 0;

  status = rtems_io_close(
    the_jnode->info.device.major,
    the_jnode->info.device.minor,
    (void *) &args
  );
  if ( status ) {
    rtems_deviceio_errno(status);
    return RTEMS_UNSATISFIED;
  }
  return 0;
}

/*
 *  device_read
 *
 *  This handler maps a read() operation onto rtems_io_read().
 */

ssize_t device_read(
  rtems_libio_t *iop,
  void          *buffer,
  unsigned32     count
)
{
  rtems_libio_rw_args_t   args;
  rtems_status_code       status;
  IMFS_jnode_t           *the_jnode;

  the_jnode = iop->file_info;

  args.iop         = iop;
  args.offset      = iop->offset;
  args.buffer      = buffer;
  args.count       = count;
  args.flags       = iop->flags;
  args.bytes_moved = 0;

  status = rtems_io_read(
    the_jnode->info.device.major,
    the_jnode->info.device.minor,
    (void *) &args
  );

  if ( status )
    return rtems_deviceio_errno(status);

  return (ssize_t) args.bytes_moved;
}

/*
 *  device_write
 *
 *  This handler maps a write() operation onto rtems_io_write().
 */

ssize_t device_write(
  rtems_libio_t *iop,
  const void    *buffer,
  unsigned32     count
)
{
  rtems_libio_rw_args_t   args;
  rtems_status_code       status;
  IMFS_jnode_t           *the_jnode;

  the_jnode = iop->file_info;

  args.iop         = iop;
  args.offset      = iop->offset;
  args.buffer      = (void *) buffer;
  args.count       = count;
  args.flags       = iop->flags;
  args.bytes_moved = 0;

  status = rtems_io_write(
    the_jnode->info.device.major,
    the_jnode->info.device.minor,
    (void *) &args
  );

  if ( status )
    return rtems_deviceio_errno(status);

  return (ssize_t) args.bytes_moved;
}

/*
 *  device_ioctl
 *
 *  This handler maps an ioctl() operation onto rtems_io_ioctl().
 */

int device_ioctl(
  rtems_libio_t *iop,
  unsigned32     command,
  void          *buffer
)
{
  rtems_libio_ioctl_args_t  args;
  rtems_status_code         status;
  IMFS_jnode_t             *the_jnode;

  args.iop     = iop;
  args.command = command;
  args.buffer  = buffer;

  the_jnode = iop->file_info;

  status = rtems_io_control(
    the_jnode->info.device.major,
    the_jnode->info.device.minor,
    (void *) &args
  );

  if ( status )
    return rtems_deviceio_errno(status);

  return args.ioctl_return;
}

/*
 *  device_lseek
 *
 *  This handler eats all lseek() operations.
 */

int device_lseek(
  rtems_libio_t *iop,
  off_t          offset,
  int            whence
)
{
  return 0;
}

/*
 *  device_stat
 *
 *  The IMFS_stat() is used.
 */

/*
 *  device_rmnod
 *
 *  The IMFS_rmnod() is used.
 */
