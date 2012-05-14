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
 * RTEMS RFS Directory Access Routines
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <rtems/rfs/rtems-rfs-dir.h>
#include <rtems/rfs/rtems-rfs-link.h>
#include "rtems-rfs-rtems.h"

/**
 * This rountine will verify that the node being opened as a directory is in
 * fact a directory node. If it is then the offset into the directory will be
 * set to 0 to position to the first directory entry.
 */
static int
rtems_rfs_rtems_dir_open (rtems_libio_t* iop,
                          const char*    pathname,
                          int            oflag,
                          mode_t         mode)
{
  rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (&iop->pathinfo);
  rtems_rfs_ino          ino = rtems_rfs_rtems_get_iop_ino (iop);
  rtems_rfs_inode_handle inode;
  int                    rc;

  rtems_rfs_rtems_lock (fs);

  rc = rtems_rfs_inode_open (fs, ino, &inode, true);
  if (rc)
  {
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("dir_open: opening inode", rc);
  }

  if (!RTEMS_RFS_S_ISDIR (rtems_rfs_inode_get_mode (&inode)))
  {
    rtems_rfs_inode_close (fs, &inode);
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("dir_open: not dir", ENOTDIR);
  }

  iop->offset = 0;

  rtems_rfs_inode_close (fs, &inode);
  rtems_rfs_rtems_unlock (fs);
  return 0;
}

/**
 * This routine will be called by the generic close routine to cleanup any
 * resources that have been allocated for the management of the file
 *
 * @param iop
 * @retval 0 Always no error.
 */
static int
rtems_rfs_rtems_dir_close (rtems_libio_t* iop)
{
  /*
   * The RFS does not hold any resources. Nothing to do.
   */
  return 0;
}

/**
 * This routine will read the next directory entry based on the directory
 * offset. The offset should be equal to -n- time the size of an individual
 * dirent structure. If n is not an integer multiple of the sizeof a dirent
 * structure, an integer division will be performed to determine directory
 * entry that will be returned in the buffer. Count should reflect -m- times
 * the sizeof dirent bytes to be placed in the buffer.  If there are not -m-
 * dirent elements from the current directory position to the end of the
 * exisiting file, the remaining entries will be placed in the buffer and the
 * returned value will be equal to -m actual- times the size of a directory
 * entry.
 */
static ssize_t
rtems_rfs_rtems_dir_read (rtems_libio_t* iop,
                          void*          buffer,
                          size_t         count)
{
  rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (&iop->pathinfo);
  rtems_rfs_ino          ino = rtems_rfs_rtems_get_iop_ino (iop);
  rtems_rfs_inode_handle inode;
  struct dirent*         dirent;
  ssize_t                bytes_transferred;
  int                    d;
  int                    rc;

  count  = count / sizeof (struct dirent);
  dirent = buffer;

  rtems_rfs_rtems_lock (fs);

  rc = rtems_rfs_inode_open (fs, ino, &inode, true);
  if (rc)
  {
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("dir_read: read inode", rc);
  }

  bytes_transferred = 0;

  for (d = 0; d < count; d++, dirent++)
  {
    size_t size;
    rc = rtems_rfs_dir_read (fs, &inode, iop->offset, dirent, &size);
    if (rc == ENOENT)
    {
      rc = 0;
      break;
    }
    if (rc > 0)
    {
      bytes_transferred = rtems_rfs_rtems_error ("dir_read: dir read", rc);
      break;
    }
    iop->offset += size;
    bytes_transferred += sizeof (struct dirent);
  }

  rtems_rfs_inode_close (fs, &inode);
  rtems_rfs_rtems_unlock (fs);

  return bytes_transferred;
}

/*
 *  Set of operations handlers for operations on directories.
 */

const rtems_filesystem_file_handlers_r rtems_rfs_rtems_dir_handlers = {
  .open_h      = rtems_rfs_rtems_dir_open,
  .close_h     = rtems_rfs_rtems_dir_close,
  .read_h      = rtems_rfs_rtems_dir_read,
  .write_h     = rtems_filesystem_default_write,
  .ioctl_h     = rtems_filesystem_default_ioctl,
  .lseek_h     = rtems_filesystem_default_lseek_directory,
  .fstat_h     = rtems_rfs_rtems_fstat,
  .ftruncate_h = rtems_filesystem_default_ftruncate_directory,
  .fsync_h     = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_rfs_rtems_fdatasync,
  .fcntl_h     = rtems_filesystem_default_fcntl
};
