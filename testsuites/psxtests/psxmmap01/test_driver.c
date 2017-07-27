/*
 * Copyright (c) 2017 Kevin Kirspel.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "test_driver.h"

#include <rtems/libio.h>

char test_data[PAGE_SIZE];

static int handler_fstat(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
)
{
  buf->st_mode = S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO;
  return 0;
}

static int handler_mmap(
  rtems_libio_t *iop,
  void **addr,
  size_t len,
  int prot,
  off_t off
)
{
  if ( len <= sizeof(test_data) ) {
    *addr = test_data;
  } else {
    *addr = NULL;
  }
  return 0;
}

static const rtems_filesystem_file_handlers_r node_handlers = {
  .open_h = rtems_filesystem_default_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = rtems_filesystem_default_read,
  .write_h = rtems_filesystem_default_write,
  .ioctl_h = rtems_filesystem_default_ioctl,
  .lseek_h = rtems_filesystem_default_lseek,
  .fstat_h = handler_fstat,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .poll_h = rtems_filesystem_default_poll,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .mmap_h = handler_mmap,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};

const IMFS_node_control node_control = IMFS_GENERIC_INITIALIZER(
  &node_handlers,
  IMFS_node_initialize_default,
  IMFS_node_destroy_default
);
