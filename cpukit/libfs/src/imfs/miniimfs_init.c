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
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

#include <stdlib.h>

#include <rtems/seterr.h>

const rtems_filesystem_operations_table miniIMFS_ops = {
  .lock_h = rtems_filesystem_default_lock,
  .unlock_h = rtems_filesystem_default_unlock,
  .eval_path_h = IMFS_eval_path,
  .link_h = rtems_filesystem_default_link,
  .are_nodes_equal_h = rtems_filesystem_default_are_nodes_equal,
  .mknod_h = IMFS_mknod,
  .rmnod_h = IMFS_rmnod,
  .fchmod_h = rtems_filesystem_default_fchmod,
  .chown_h = rtems_filesystem_default_chown,
  .clonenod_h = IMFS_node_clone,
  .freenod_h = IMFS_node_free,
  .mount_h = IMFS_mount,
  .unmount_h = rtems_filesystem_default_unmount,
  .fsunmount_me_h = rtems_filesystem_default_fsunmount,
  .utime_h = rtems_filesystem_default_utime,
  .symlink_h = rtems_filesystem_default_symlink,
  .readlink_h = rtems_filesystem_default_readlink,
  .rename_h = rtems_filesystem_default_rename,
  .statvfs_h = rtems_filesystem_default_statvfs
};

static const IMFS_mknod_controls IMFS_mini_mknod_controls = {
  .directory = &IMFS_mknod_control_directory,
  .device = &IMFS_mknod_control_device,
  .file = &IMFS_mknod_control_memfile,
  .fifo = &IMFS_mknod_control_enosys
};

int miniIMFS_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
)
{
  IMFS_fs_info_t *fs_info = calloc( 1, sizeof( *fs_info ) );
  IMFS_mount_data mount_data = {
    .fs_info = fs_info,
    .ops = &miniIMFS_ops,
    .mknod_controls = &IMFS_mini_mknod_controls
  };

  if ( fs_info == NULL ) {
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }

  return IMFS_initialize_support( mt_entry, &mount_data );
}
