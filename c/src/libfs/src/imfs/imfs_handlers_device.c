/*
 *  Device Operations Table for the IMFS
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
 *  Handler table for IMFS device nodes
 */

rtems_filesystem_file_handlers_r IMFS_device_handlers = {
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
