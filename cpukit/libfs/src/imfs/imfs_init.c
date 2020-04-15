/**
 * @file
 *
 * @ingroup LibFSIMFS
 *
 * @brief IMFS initialization.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/imfs.h>

#include <stdlib.h>

#include <rtems/seterr.h>

static const rtems_filesystem_operations_table IMFS_ops = {
  .lock_h = rtems_filesystem_default_lock,
  .unlock_h = rtems_filesystem_default_unlock,
  .eval_path_h = IMFS_eval_path,
  .link_h = IMFS_link,
  .are_nodes_equal_h = rtems_filesystem_default_are_nodes_equal,
  .mknod_h = IMFS_mknod,
  .rmnod_h = IMFS_rmnod,
  .fchmod_h = IMFS_fchmod,
  .chown_h = IMFS_chown,
  .clonenod_h = IMFS_node_clone,
  .freenod_h = IMFS_node_free,
  .mount_h = IMFS_mount,
  .unmount_h = IMFS_unmount,
  .fsunmount_me_h = IMFS_fsunmount,
  .utime_h = IMFS_utime,
  .symlink_h = IMFS_symlink,
  .readlink_h = IMFS_readlink,
  .rename_h = IMFS_rename,
  .statvfs_h = rtems_filesystem_default_statvfs
};

static const IMFS_mknod_controls IMFS_default_mknod_controls = {
  .directory = &IMFS_mknod_control_dir_default,
  .device = &IMFS_mknod_control_device,
  .file = &IMFS_mknod_control_memfile,
  .fifo = &IMFS_mknod_control_enosys
};

int IMFS_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
)
{
  IMFS_fs_info_t *fs_info = calloc( 1, sizeof( *fs_info ) );
  IMFS_mount_data mount_data = {
    .fs_info = fs_info,
    .ops = &IMFS_ops,
    .mknod_controls = &IMFS_default_mknod_controls
  };

  if ( fs_info == NULL ) {
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }

  return IMFS_initialize_support( mt_entry, &mount_data );
}
