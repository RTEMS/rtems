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

#include <errno.h>

#include "imfs.h"

/*
 *  Set of operations handlers for operations on directories.
 */

rtems_filesystem_file_handlers_r IMFS_directory_handlers = {
  imfs_dir_open,
  imfs_dir_close,
  imfs_dir_read,
  NULL,             /* write */
  NULL,             /* ioctl */
  imfs_dir_lseek,
  imfs_dir_fstat,
  IMFS_fchmod,
  NULL,             /* ftruncate */
  NULL,             /* fpathconf */
  NULL,             /* fsync */
  IMFS_fdatasync,
  IMFS_fcntl,
  imfs_dir_rmnod
};

