/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems-rfs
 *
 * RTEMS RFS Device Interface.
 *
 * This file contains the set of handlers used to map operations on RFS device
 * nodes onto calls to the RTEMS Classic API IO Manager.
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/devfs.h>
#include "rtems-rfs-rtems.h"

/**
 * This handler maps an open() operation onto rtems_io_open().
 *
 * @param iop
 * @param pathname
 * @param flag
 * @param mode
 * @return int
 */
static int
rtems_rfs_rtems_device_open ( rtems_libio_t *iop,
                              const char    *pathname,
                              int            oflag,
                              mode_t         mode)
{
  rtems_libio_open_close_args_t args;
  rtems_rfs_file_system*        fs = rtems_rfs_rtems_pathloc_dev (&iop->pathinfo);
  rtems_rfs_ino                 ino = rtems_rfs_rtems_get_iop_ino (iop);
  rtems_rfs_inode_handle        inode;
  int                           major;
  int                           minor;
  rtems_status_code             status;
  int                           rc;

  rtems_rfs_rtems_lock (fs);

  rc = rtems_rfs_inode_open (fs, ino, &inode, true);
  if (rc > 0)
  {
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("device_open: opening inode", rc);
  }

  major = rtems_rfs_inode_get_block (&inode, 0);
  minor = rtems_rfs_inode_get_block (&inode, 1);

  rc = rtems_rfs_inode_close (fs, &inode);
  if (rc > 0)
  {
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("device_open: closing inode", rc);
  }

  rtems_rfs_rtems_unlock (fs);

  iop->data0 = major;
  iop->data1 = (void*)((intptr_t) minor);

  args.iop   = iop;
  args.flags = iop->flags;
  args.mode  = mode;

  status = rtems_io_open (major, minor, (void *) &args);

  return rtems_deviceio_errno (status);
}

/**
 * This handler maps a close() operation onto rtems_io_close().
 *
 * @param iop
 * @return int
 */

static int
rtems_rfs_rtems_device_close (rtems_libio_t* iop)
{
  rtems_libio_open_close_args_t args;
  rtems_status_code             status;
  int                           major;
  int                           minor;

  major = (int) iop->data0;
  minor = (intptr_t) iop->data1;

  args.iop   = iop;
  args.flags = 0;
  args.mode  = 0;

  status = rtems_io_close (major, minor, (void *) &args);

  return rtems_deviceio_errno (status);
}

/**
 * This handler maps a read() operation onto rtems_io_read().
 *
 * @param iop
 * @param buffer
 * @param count
 * @return ssize_t
 */

static ssize_t
rtems_rfs_rtems_device_read (rtems_libio_t* iop, void* buffer, size_t count)
{
  rtems_libio_rw_args_t args;
  rtems_status_code     status;
  int                   major;
  int                   minor;

  major = (int) iop->data0;
  minor = (intptr_t) iop->data1;

  args.iop         = iop;
  args.offset      = iop->offset;
  args.buffer      = buffer;
  args.count       = count;
  args.flags       = iop->flags;
  args.bytes_moved = 0;

  status = rtems_io_read (major, minor, (void *) &args);
  if (status)
    return rtems_deviceio_errno (status);

  return (ssize_t) args.bytes_moved;
}

/*
 * This handler maps a write() operation onto rtems_io_write().
 *
 * @param iop
 * @param buffer
 * @param count
 * @return ssize_t
 */

static ssize_t
rtems_rfs_rtems_device_write (rtems_libio_t* iop,
                              const void*    buffer,
                              size_t         count)
{
  rtems_libio_rw_args_t args;
  rtems_status_code     status;
  int                   major;
  int                   minor;

  major = (int) iop->data0;
  minor = (intptr_t) iop->data1;

  args.iop         = iop;
  args.offset      = iop->offset;
  args.buffer      = (void *) buffer;
  args.count       = count;
  args.flags       = iop->flags;
  args.bytes_moved = 0;

  status = rtems_io_write (major, minor, (void *) &args);
  if (status)
    return rtems_deviceio_errno (status);

  return (ssize_t) args.bytes_moved;
}

/**
 * This handler maps an ioctl() operation onto rtems_io_ioctl().
 *
 * @param iop
 * @param command
 * @param buffer
 * @return int
 */

static int
rtems_rfs_rtems_device_ioctl (rtems_libio_t* iop,
                              uint32_t       command,
                              void*          buffer)
{
  rtems_libio_ioctl_args_t args;
  rtems_status_code        status;
  int                      major;
  int                      minor;

  major = (int) iop->data0;
  minor = (intptr_t) iop->data1;

  args.iop     = iop;
  args.command = command;
  args.buffer  = buffer;

  status = rtems_io_control (major, minor, (void *) &args);
  if (status)
    return rtems_deviceio_errno (status);

  return args.ioctl_return;
}

/**
 * The consumes the truncate call. You cannot truncate device files.
 *
 * @param iop
 * @param length
 * @return int
 */

static int
rtems_rfs_rtems_device_ftruncate (rtems_libio_t* iop, off_t length)
{
  return 0;
}

/*
 *  Handler table for RFS device nodes
 */

const rtems_filesystem_file_handlers_r rtems_rfs_rtems_device_handlers = {
  .open_h      = rtems_rfs_rtems_device_open,
  .close_h     = rtems_rfs_rtems_device_close,
  .read_h      = rtems_rfs_rtems_device_read,
  .write_h     = rtems_rfs_rtems_device_write,
  .ioctl_h     = rtems_rfs_rtems_device_ioctl,
  .lseek_h     = rtems_filesystem_default_lseek_file,
  .fstat_h     = rtems_rfs_rtems_fstat,
  .ftruncate_h = rtems_rfs_rtems_device_ftruncate,
  .fsync_h     = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h     = rtems_filesystem_default_fcntl
};
