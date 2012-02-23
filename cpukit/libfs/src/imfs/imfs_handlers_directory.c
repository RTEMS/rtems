/*
 *  Operations Table for Directories for the IMFS
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

/*
 *  Set of operations handlers for operations on directories.
 */

const rtems_filesystem_file_handlers_r IMFS_directory_handlers = {
  imfs_dir_open,
  rtems_filesystem_default_close,
  imfs_dir_read,
  rtems_filesystem_default_write,
  rtems_filesystem_default_ioctl,
  imfs_dir_lseek,
  IMFS_stat,
  rtems_filesystem_default_ftruncate_directory,
  rtems_filesystem_default_fsync_or_fdatasync_success,
  rtems_filesystem_default_fsync_or_fdatasync_success,
  rtems_filesystem_default_fcntl
};
