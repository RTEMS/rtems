/*
 *  Operations Tables for the IMFS
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <errno.h>

#include "imfs.h"

/*
 *  Set of operations handlers for operations on memfile entities.
 */

rtems_filesystem_file_handlers_r memfile_handlers = {
  memfile_open,
  memfile_close,
  memfile_read,
  memfile_write,
  memfile_ioctl,
  memfile_lseek,
  IMFS_stat,
  IMFS_fchmod,
  memfile_ftruncate,
  NULL,                /* fpathconf */
  NULL,                /* fsync */
  IMFS_fdatasync,
  IMFS_fcntl
};

/*
 *  Set of operations handlers for operations on directories.
 */

rtems_filesystem_file_handlers_r dir_handlers = {
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
  IMFS_fcntl
};

/*
 *  Handler table for IMFS device nodes
 */

rtems_filesystem_file_handlers_r device_handlers = {
  device_open,
  device_close,
  device_read,
  device_write,
  device_ioctl,
  device_lseek,
  IMFS_stat,
  IMFS_fchmod,
  NULL,   /* ftruncate */
  NULL,   /* fpathconf */
  NULL,   /* fsync */
  NULL,   /* fdatasync */
  NULL    /* fcntl */
};

/*
 *  Set of null operations handlers.
 */

rtems_filesystem_file_handlers_r null_handlers = {
  NULL,       /* open */
  NULL,       /* close */
  NULL,       /* read */
  NULL,       /* write */
  NULL,       /* ioctl */
  NULL,       /* lseek */
  NULL,       /* fstat */
  NULL,       /* fchmod */
  NULL,       /* ftruncate */
  NULL,       /* fpathconf */
  NULL,       /* fsync */
  NULL,       /* fdatasync */
  NULL        /* fcntl */
};
