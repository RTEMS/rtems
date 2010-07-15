/**
 * @file
 *
 * @ingroup LibIO
 *
 * @brief rtems_filesystem_operations_default definition.
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

#include <rtems/libio.h>

const rtems_filesystem_operations_table rtems_filesystem_operations_default = {
  .evalpath_h = rtems_filesystem_default_evalpath,
  .evalformake_h = rtems_filesystem_default_evalformake,
  .link_h = rtems_filesystem_default_link,
  .unlink_h = rtems_filesystem_default_unlink,
  .node_type_h = rtems_filesystem_default_node_type,
  .mknod_h = rtems_filesystem_default_mknod,
  .chown_h = rtems_filesystem_default_chown,
  .freenod_h = rtems_filesystem_default_freenode,
  .mount_h = rtems_filesystem_default_mount,
  .fsmount_me_h = rtems_filesystem_default_fsmount,
  .unmount_h = rtems_filesystem_default_unmount,
  .fsunmount_me_h = rtems_filesystem_default_fsunmount,
  .utime_h = rtems_filesystem_default_utime,
  .eval_link_h = rtems_filesystem_default_evaluate_link,
  .symlink_h = rtems_filesystem_default_symlink,
  .readlink_h = rtems_filesystem_default_readlink,
  .rename_h = rtems_filesystem_default_rename,
  .statvfs_h = rtems_filesystem_default_statvfs
};
