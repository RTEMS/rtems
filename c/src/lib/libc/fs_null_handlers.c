/*
 *  Null Filesystem Operations Tables
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

#include <rtems/libio.h>

/*
 *  Set of null operations handlers.
 */

rtems_filesystem_file_handlers_r rtems_filesystem_null_handlers = {
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
  NULL,       /* fcntl */
  NULL        /* rmnod */
};
