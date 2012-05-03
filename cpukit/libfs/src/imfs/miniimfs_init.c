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
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

const rtems_filesystem_operations_table miniIMFS_ops = {
  .lock_h = rtems_filesystem_default_lock,
  .unlock_h = rtems_filesystem_default_unlock,
  .eval_path_h = IMFS_eval_path,
  .link_h = rtems_filesystem_default_link,
  .are_nodes_equal_h = rtems_filesystem_default_are_nodes_equal,
  .node_type_h = IMFS_node_type,
  .mknod_h = IMFS_mknod,
  .rmnod_h = IMFS_rmnod,
  .fchmod_h = rtems_filesystem_default_fchmod,
  .chown_h = rtems_filesystem_default_chown,
  .clonenod_h = IMFS_node_clone,
  .freenod_h = IMFS_node_free,
  .mount_h = IMFS_mount,
  .fsmount_me_h = miniIMFS_initialize,
  .unmount_h = rtems_filesystem_default_unmount,
  .fsunmount_me_h = rtems_filesystem_default_fsunmount,
  .utime_h = rtems_filesystem_default_utime,
  .symlink_h = rtems_filesystem_default_symlink,
  .readlink_h = rtems_filesystem_default_readlink,
  .rename_h = rtems_filesystem_default_rename,
  .statvfs_h = rtems_filesystem_default_statvfs
};

static const IMFS_node_control *const
  IMFS_mini_node_controls [IMFS_TYPE_COUNT] = {
  [IMFS_DIRECTORY] = &IMFS_node_control_directory,
  [IMFS_DEVICE] = &IMFS_node_control_device,
  [IMFS_HARD_LINK] = &IMFS_node_control_default,
  [IMFS_SYM_LINK] = &IMFS_node_control_default,
  [IMFS_MEMORY_FILE] = &IMFS_node_control_memfile,
  [IMFS_LINEAR_FILE] = &IMFS_node_control_linfile,
  [IMFS_FIFO] = &IMFS_node_control_default
};

int miniIMFS_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
)
{
  return IMFS_initialize_support(
    mt_entry,
    &miniIMFS_ops,
    IMFS_mini_node_controls
  );
}
