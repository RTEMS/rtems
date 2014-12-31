/**
 * @file
 *
 * @brief Memfile Operations Tables
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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

static int IMFS_stat_file(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
)
{
  const IMFS_file_t *file = loc->node_access;

  buf->st_size = file->File.size;
  buf->st_blksize = imfs_rq_memfile_bytes_per_block;

  return IMFS_stat( loc, buf );
}

static const rtems_filesystem_file_handlers_r IMFS_memfile_handlers = {
  .open_h = rtems_filesystem_default_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = memfile_read,
  .write_h = memfile_write,
  .ioctl_h = rtems_filesystem_default_ioctl,
  .lseek_h = rtems_filesystem_default_lseek_file,
  .fstat_h = IMFS_stat_file,
  .ftruncate_h = memfile_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync_success,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync_success,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};

static const rtems_filesystem_file_handlers_r IMFS_linfile_handlers = {
  .open_h = IMFS_linfile_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = IMFS_linfile_read,
  .write_h = rtems_filesystem_default_write,
  .ioctl_h = rtems_filesystem_default_ioctl,
  .lseek_h = rtems_filesystem_default_lseek_file,
  .fstat_h = IMFS_stat_file,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync_success,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync_success,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};

const IMFS_node_control IMFS_node_control_memfile = {
  .handlers = &IMFS_memfile_handlers,
  .node_size = sizeof(IMFS_file_t),
  .node_initialize = IMFS_node_initialize_default,
  .node_remove = IMFS_node_remove_default,
  .node_destroy = IMFS_memfile_remove
};

const IMFS_node_control IMFS_node_control_linfile = {
  .handlers = &IMFS_linfile_handlers,
  .node_size = sizeof(IMFS_file_t),
  .node_initialize = IMFS_node_initialize_default,
  .node_remove = IMFS_node_remove_default,
  .node_destroy = IMFS_node_destroy_default
};
