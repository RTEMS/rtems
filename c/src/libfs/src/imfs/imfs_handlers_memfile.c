/*
 *  Memfile Operations Tables for the IMFS
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

#include <errno.h>

#include "imfs.h"

/*
 *  Set of operations handlers for operations on memfile entities.
 */

rtems_filesystem_file_handlers_r IMFS_linearfile_handlers = {
  memfile_open,
  memfile_close,
  linearfile_read,
  NULL,                /* write */
  memfile_ioctl,
  linearfile_lseek,
  IMFS_stat,
  NULL,                /* chmod */
  NULL,                /* ftruncate */
  NULL,                /* fpathconf */
  IMFS_fdatasync,      /* fsync */
  IMFS_fdatasync,
  IMFS_fcntl,
  NULL                 /* rmnod */
};

rtems_filesystem_file_handlers_r IMFS_memfile_handlers = {
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
  IMFS_fdatasync,      /* fsync */
  IMFS_fdatasync,
  IMFS_fcntl,
  memfile_rmnod
};
