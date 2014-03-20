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
  const IMFS_jnode_t *node = loc->node_access;

  buf->st_size = node->info.file.size;
  buf->st_blksize = imfs_rq_memfile_bytes_per_block;

  return IMFS_stat( loc, buf );
}

static const rtems_filesystem_file_handlers_r IMFS_memfile_handlers = {
  .open_h = memfile_open,
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

const IMFS_node_control IMFS_node_control_memfile = {
  .imfs_type = IMFS_MEMORY_FILE,
  .handlers = &IMFS_memfile_handlers,
  .node_initialize = IMFS_node_initialize_default,
  .node_remove = IMFS_node_remove_default,
  .node_destroy = IMFS_memfile_remove
};

const IMFS_node_control IMFS_node_control_linfile = {
  .imfs_type = IMFS_LINEAR_FILE,
  .handlers = &IMFS_memfile_handlers,
  .node_initialize = IMFS_node_initialize_default,
  .node_remove = IMFS_node_remove_default,
  .node_destroy = IMFS_node_destroy_default
};
