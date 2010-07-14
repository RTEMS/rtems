/**
 * @file
 *
 * @ingroup LibFSIMFS
 *
 * @brief IMFS without fifo support initialization.
 */

/*
 * Copyright (c) 2010
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "imfs.h"

const rtems_filesystem_operations_table fifoIMFS_ops = {
  .evalpath_h = IMFS_eval_path,
  .evalformake_h = IMFS_evaluate_for_make,
  .link_h = IMFS_link,
  .unlink_h = IMFS_unlink,
  .node_type_h = IMFS_node_type,
  .mknod_h = IMFS_mknod,
  .chown_h = IMFS_chown,
  .freenod_h = rtems_filesystem_default_freenode,
  .mount_h = IMFS_mount,
  .fsmount_me_h = fifoIMFS_initialize,
  .unmount_h = IMFS_unmount,
  .fsunmount_me_h = IMFS_fsunmount,
  .utime_h = IMFS_utime,
  .eval_link_h = IMFS_evaluate_link,
  .symlink_h = IMFS_symlink,
  .readlink_h = IMFS_readlink,
  .rename_h = IMFS_rename,
  .statvfs_h = rtems_filesystem_default_statvfs
};

int fifoIMFS_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void *data
)
{
  return IMFS_initialize_support(
    mt_entry,
    &fifoIMFS_ops,
    &IMFS_memfile_handlers,
    &IMFS_directory_handlers,
    &IMFS_fifo_handlers
  );
}
