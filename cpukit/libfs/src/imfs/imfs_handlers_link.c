/*
 *  Link Operations Table for the IMFS
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

#include <errno.h>

#include "imfs.h"

/*
 *  Handler table for IMFS device nodes
 */

rtems_filesystem_file_handlers_r IMFS_link_handlers = {
  NULL,       /* open */
  NULL,       /* close */
  NULL,       /* read */
  NULL,       /* write */
  NULL,       /* ioctl */
  NULL,       /* lseek */
  IMFS_stat,  /* stat */
  NULL,       /* fchmod */
  NULL,       /* ftruncate */
  NULL,       /* fpathconf */
  NULL,       /* fsync */
  NULL,       /* fdatasync */
  NULL,       /* fcntl */
  IMFS_rmnod
};
