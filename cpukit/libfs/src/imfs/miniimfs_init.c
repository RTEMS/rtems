/*
 *  Mini-IMFS Initialization
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

#if defined(IMFS_DEBUG)
#include <stdio.h>
#endif

/*
 *  miniIMFS file system operations table
 */

rtems_filesystem_operations_table  miniIMFS_ops = {
  IMFS_eval_path,
  IMFS_evaluate_for_make,
  NULL, /* XXX IMFS_link, */
  NULL, /* XXX IMFS_unlink, */
  IMFS_node_type,
  IMFS_mknod,
  NULL, /* XXX IMFS_chown, */
  NULL, /* XXX IMFS_freenodinfo, */
  NULL, /* XXX IMFS_mount, */
  miniIMFS_initialize,
  NULL, /* XXX IMFS_unmount, */
  NULL, /* XXX IMFS_fsunmount, */
  NULL, /* XXX IMFS_utime, */
  NULL, /* XXX IMFS_evaluate_link, */
  NULL, /* XXX IMFS_symlink, */
  NULL  /* XXX IMFS_readlink */
};

/*
 *  miniIMFS_initialize
 */

int miniIMFS_initialize(
  rtems_filesystem_mount_table_entry_t *temp_mt_entry
)
{
   IMFS_initialize_support(
      temp_mt_entry,
      &miniIMFS_ops,
      &rtems_filesystem_null_handlers,  /* for linearfiles */
      &rtems_filesystem_null_handlers,  /* for memfiles */
      &rtems_filesystem_null_handlers   /* for directories */
   );
   return 0;
}



