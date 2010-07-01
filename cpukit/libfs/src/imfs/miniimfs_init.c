/**
 * @file
 *
 * @ingroup LibFSIMFS
 *
 * @brief Mini-IMFS initialization.
 */

/*
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

#include <rtems/libio_.h>

#include "imfs.h"

static const rtems_filesystem_operations_table miniIMFS_ops = {
  .evalpath_h = IMFS_eval_path,
  .evalformake_h = IMFS_evaluate_for_make,
  .link_h = rtems_filesystem_default_link,
  .unlink_h = rtems_filesystem_default_unlink,
  .node_type_h = IMFS_node_type,
  .mknod_h = IMFS_mknod,
  .chown_h = rtems_filesystem_default_chown,
  .freenod_h = rtems_filesystem_default_freenode,
  .mount_h = IMFS_mount,
  .fsmount_me_h = miniIMFS_initialize,
  .unmount_h = rtems_filesystem_default_unmount,
  .fsunmount_me_h = rtems_filesystem_default_unmount,
  .utime_h = rtems_filesystem_default_utime,
  .eval_link_h = rtems_filesystem_default_evaluate_link,
  .symlink_h = rtems_filesystem_default_symlink,
  .readlink_h = rtems_filesystem_default_readlink,
  .rename_h = rtems_filesystem_default_rename,
  .statvfs_h = rtems_filesystem_default_statvfs
};

int miniIMFS_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
)
{
  return IMFS_initialize_support(
    mt_entry,
    &miniIMFS_ops,
    &rtems_filesystem_null_handlers, /* for memfiles */
    &rtems_filesystem_null_handlers, /* for directories */
    &rtems_filesystem_null_handlers  /* for fifos */
  );
}
