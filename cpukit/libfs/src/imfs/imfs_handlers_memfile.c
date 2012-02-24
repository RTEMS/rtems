/*
 *  Memfile Operations Tables for the IMFS
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

static const rtems_filesystem_file_handlers_r IMFS_memfile_handlers = {
  memfile_open,
  rtems_filesystem_default_close,
  memfile_read,
  memfile_write,
  memfile_ioctl,
  memfile_lseek,
  IMFS_stat,
  memfile_ftruncate,
  rtems_filesystem_default_fsync_or_fdatasync_success,
  rtems_filesystem_default_fsync_or_fdatasync_success,
  rtems_filesystem_default_fcntl
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
