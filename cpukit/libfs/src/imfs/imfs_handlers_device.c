/**
 * @file
 *
 * @brief Device Operations Table
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

#include <rtems/imfs.h>

static int IMFS_stat_device(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
)
{
  const IMFS_device_t *device = loc->node_access;

  buf->st_rdev = rtems_filesystem_make_dev_t( device->major, device->minor );

  return IMFS_stat( loc, buf );
}

static const rtems_filesystem_file_handlers_r IMFS_device_handlers = {
  .open_h = device_open,
  .close_h = device_close,
  .read_h = device_read,
  .write_h = device_write,
  .ioctl_h = device_ioctl,
  .lseek_h = rtems_filesystem_default_lseek_file,
  .fstat_h = IMFS_stat_device,
  .ftruncate_h = device_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .mmap_h = rtems_filesystem_default_mmap,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};

static IMFS_jnode_t *IMFS_node_initialize_device(
  IMFS_jnode_t *node,
  void *arg
)
{
  IMFS_device_t *device = (IMFS_device_t *) node;
  dev_t *dev = arg;

  rtems_filesystem_split_dev_t( *dev, device->major, device->minor );

  return node;
}

const IMFS_mknod_control IMFS_mknod_control_device = {
  {
    .handlers = &IMFS_device_handlers,
    .node_initialize = IMFS_node_initialize_device,
    .node_remove = IMFS_node_remove_default,
    .node_destroy = IMFS_node_destroy_default
  },
  .node_size = sizeof( IMFS_device_t )
};
