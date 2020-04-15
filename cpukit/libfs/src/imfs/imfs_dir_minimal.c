/**
 * @file
 *
 * @ingroup IMFS
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

static const rtems_filesystem_file_handlers_r IMFS_dir_minimal_handlers = {
  .open_h = rtems_filesystem_default_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = rtems_filesystem_default_read,
  .write_h = rtems_filesystem_default_write,
  .ioctl_h = rtems_filesystem_default_ioctl,
  .lseek_h = rtems_filesystem_default_lseek_directory,
  .fstat_h = IMFS_stat,
  .ftruncate_h = rtems_filesystem_default_ftruncate_directory,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync_success,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync_success,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .mmap_h = rtems_filesystem_default_mmap,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};

const IMFS_mknod_control IMFS_mknod_control_dir_minimal = {
  {
    .handlers = &IMFS_dir_minimal_handlers,
    .node_initialize = IMFS_node_initialize_directory,
    .node_remove = IMFS_node_remove_directory,
    .node_destroy = IMFS_node_destroy_default
  },
  .node_size = sizeof( IMFS_directory_t )
};
