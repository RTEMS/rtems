/*
 *  IMFS Stub for UNIX configuration
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

#include <sys/types.h>         /* for mkdir */
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <assert.h>

#include "imfs.h"
#include <rtems/libio_.h>

#include <stdio.h>

/*  
 *  IMFS file system operations table
 */

rtems_filesystem_operations_table  IMFS_ops = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

/*  
 *  IMFS file system operations table
 */

rtems_filesystem_operations_table  miniIMFS_ops = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};
