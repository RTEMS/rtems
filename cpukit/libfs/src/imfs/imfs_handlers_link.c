/*
 *  Link Operations Table for the IMFS
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
 *  Handler table for IMFS device nodes
 */

const rtems_filesystem_file_handlers_r IMFS_link_handlers = {
  rtems_filesystem_default_open,
  rtems_filesystem_default_close,
  rtems_filesystem_default_read,
  rtems_filesystem_default_write,
  rtems_filesystem_default_ioctl,
  rtems_filesystem_default_lseek,
  IMFS_stat,  /* stat */
  rtems_filesystem_default_ftruncate,
  rtems_filesystem_default_fsync_or_fdatasync,
  rtems_filesystem_default_fsync_or_fdatasync,
  rtems_filesystem_default_fcntl
};
