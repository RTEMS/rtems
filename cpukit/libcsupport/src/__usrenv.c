/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <sys/stat.h>

#include <rtems/libio_.h>

static int null_handler_open(
  rtems_libio_t *iop,
  const char *path,
  int oflag,
  mode_t mode
)
{
  return -1;
}

static int null_handler_fstat(
  const rtems_filesystem_location_info_t *pathloc,
  struct stat *buf
)
{
  return -1;
}

const rtems_filesystem_file_handlers_r rtems_filesystem_null_handlers = {
  .open_h = null_handler_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = rtems_filesystem_default_read,
  .write_h = rtems_filesystem_default_write,
  .ioctl_h = rtems_filesystem_default_ioctl,
  .lseek_h = rtems_filesystem_default_lseek,
  .fstat_h = null_handler_fstat,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl
};

static void null_op_lock_or_unlock(
  const rtems_filesystem_mount_table_entry_t *mt_entry
)
{
  /* Do nothing */
}

static int null_op_mknod(
  const rtems_filesystem_location_info_t *parentloc,
  const char *name,
  size_t namelen,
  mode_t mode,
  dev_t dev
)
{
  return -1;
}

static int null_op_rmnod(
  const rtems_filesystem_location_info_t *parentloc,
  const rtems_filesystem_location_info_t *loc
)
{
  return -1;
}

static int null_op_link(
  const rtems_filesystem_location_info_t *parentloc,
  const rtems_filesystem_location_info_t *targetloc,
  const char *name,
  size_t namelen
)
{
  return -1;
}

static int null_op_fchmod(
  const rtems_filesystem_location_info_t *pathloc,
  mode_t mode
)
{
  return -1;
}

static int null_op_chown(
  const rtems_filesystem_location_info_t *loc,
  uid_t owner,
  gid_t group
)
{
  return -1;
}

static int null_op_clonenode(
  rtems_filesystem_location_info_t *loc
)
{
  return -1;
}

static int null_op_mount(
  rtems_filesystem_mount_table_entry_t *mt_entry
)
{
  return -1;
}

static int null_op_fsmount_me(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void *data
)
{
  return -1;
}

static int null_op_unmount(
  rtems_filesystem_mount_table_entry_t *mt_entry
)
{
  return -1;
}

static void null_op_fsunmount_me(
  rtems_filesystem_mount_table_entry_t *mt_entry
)
{
  /* Do nothing */
}

static int null_op_utime(
  const rtems_filesystem_location_info_t *loc,
  time_t actime,
  time_t modtime
)
{
  return -1;
}

static int null_op_symlink(
  const rtems_filesystem_location_info_t *parentloc,
  const char *name,
  size_t namelen,
  const char *target
)
{
  return -1;
}

static ssize_t null_op_readlink(
  const rtems_filesystem_location_info_t *loc,
  char *buf,
  size_t bufsize
)
{
  return -1;
}

static int null_op_rename(
  const rtems_filesystem_location_info_t *oldparentloc,
  const rtems_filesystem_location_info_t *oldloc,
  const rtems_filesystem_location_info_t *newparentloc,
  const char *name,
  size_t namelen
)
{
  return -1;
}

static int null_op_statvfs(
  const rtems_filesystem_location_info_t *loc,
  struct statvfs *buf
)
{
  return -1;
}

static const rtems_filesystem_operations_table null_ops = {
  .lock_h = null_op_lock_or_unlock,
  .unlock_h = null_op_lock_or_unlock,
  .eval_path_h = rtems_filesystem_default_eval_path,
  .link_h = null_op_link,
  .are_nodes_equal_h = rtems_filesystem_default_are_nodes_equal,
  .node_type_h = rtems_filesystem_default_node_type,
  .mknod_h = null_op_mknod,
  .rmnod_h = null_op_rmnod,
  .fchmod_h = null_op_fchmod,
  .chown_h = null_op_chown,
  .clonenod_h = null_op_clonenode,
  .freenod_h = rtems_filesystem_default_freenode,
  .mount_h = null_op_mount,
  .fsmount_me_h = null_op_fsmount_me,
  .unmount_h = null_op_unmount,
  .fsunmount_me_h = null_op_fsunmount_me,
  .utime_h = null_op_utime,
  .symlink_h = null_op_symlink,
  .readlink_h = null_op_readlink,
  .rename_h = null_op_rename,
  .statvfs_h = null_op_statvfs
};

rtems_filesystem_mount_table_entry_t rtems_filesystem_null_mt_entry = {
  .location_chain = {
    .Head = {
      .Node = {
        .next = &rtems_filesystem_global_location_null.location.mt_entry_node,
        .previous = NULL
      },
      .fill = &rtems_filesystem_global_location_null.location.mt_entry_node,
    }
  },
  .ops = &null_ops,
  .mt_point_node = &rtems_filesystem_global_location_null,
  .mt_fs_root = &rtems_filesystem_global_location_null,
  .mounted = false,
  .writeable = false
};

rtems_filesystem_global_location_t rtems_filesystem_global_location_null = {
  .location = {
    .mt_entry_node = {
      .next = &rtems_filesystem_null_mt_entry.location_chain.Tail.Node,
      .previous = &rtems_filesystem_null_mt_entry.location_chain.Head.Node
    },
    .handlers = &rtems_filesystem_null_handlers,
    .mt_entry = &rtems_filesystem_null_mt_entry
  },

  /*
   * The initial reference count accounts for the following references
   *  o the root directory of the user environment,
   *  o the current directory of the user environment,
   *  o the root node of the null file system instance, and
   *  o the mount point node of the null file system instance.
   */
  .reference_count = 4
};

rtems_user_env_t rtems_global_user_env = {
  .current_directory = &rtems_filesystem_global_location_null,
  .root_directory = &rtems_filesystem_global_location_null,
  .umask = S_IWGRP | S_IWOTH
};

rtems_user_env_t *rtems_current_user_env = &rtems_global_user_env;
