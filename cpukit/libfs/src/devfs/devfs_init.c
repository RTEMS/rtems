/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "devfs.h"

const rtems_filesystem_operations_table devFS_ops = {
  .lock_h = rtems_filesystem_default_lock,
  .unlock_h = rtems_filesystem_default_unlock,
  .eval_path_h = devFS_eval_path,
  .link_h = rtems_filesystem_default_link,
  .are_nodes_equal_h = rtems_filesystem_default_are_nodes_equal,
  .node_type_h = devFS_node_type,
  .mknod_h = devFS_mknod,
  .rmnod_h = rtems_filesystem_default_rmnod,
  .fchmod_h = rtems_filesystem_default_fchmod,
  .chown_h = rtems_filesystem_default_chown,
  .clonenod_h = rtems_filesystem_default_clonenode,
  .freenod_h = rtems_filesystem_default_freenode,
  .mount_h = rtems_filesystem_default_mount,
  .fsmount_me_h = devFS_initialize,
  .unmount_h = rtems_filesystem_default_unmount,
  .fsunmount_me_h = rtems_filesystem_default_fsunmount,
  .utime_h = rtems_filesystem_default_utime,
  .symlink_h = rtems_filesystem_default_symlink,
  .readlink_h = rtems_filesystem_default_readlink,
  .rename_h = rtems_filesystem_default_rename,
  .statvfs_h = rtems_filesystem_default_statvfs
};

const rtems_filesystem_file_handlers_r devFS_file_handlers = {
  .open_h = devFS_open,
  .close_h = devFS_close,
  .read_h = devFS_read,
  .write_h = devFS_write,
  .ioctl_h = devFS_ioctl,
  .lseek_h = rtems_filesystem_default_lseek_file,
  .fstat_h = devFS_stat,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl
};

int devFS_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void *data
)
{
  int rv = 0;

  if (data != NULL) {
    mt_entry->ops = &devFS_ops;
    mt_entry->immutable_fs_info = data;
    mt_entry->mt_fs_root->location.handlers = &devFS_file_handlers;
  } else {
    errno = EINVAL;
    rv = -1;
  }

  return rv;
}

