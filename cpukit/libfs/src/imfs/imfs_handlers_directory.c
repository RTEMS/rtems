/*
 *  Operations Table for Directories for the IMFS
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

static const rtems_filesystem_file_handlers_r IMFS_directory_handlers = {
  rtems_filesystem_default_open,
  rtems_filesystem_default_close,
  imfs_dir_read,
  rtems_filesystem_default_write,
  rtems_filesystem_default_ioctl,
  imfs_dir_lseek,
  IMFS_stat,
  rtems_filesystem_default_ftruncate_directory,
  rtems_filesystem_default_fsync_or_fdatasync_success,
  rtems_filesystem_default_fsync_or_fdatasync_success,
  rtems_filesystem_default_fcntl
};

static IMFS_jnode_t *IMFS_node_initialize_directory(
  IMFS_jnode_t *node,
  const IMFS_types_union *info
)
{
  rtems_chain_initialize_empty( &node->info.directory.Entries );

  return node;
}

static bool IMFS_is_mount_point( const IMFS_jnode_t *node )
{
  return node->info.directory.mt_fs != NULL;
}

static IMFS_jnode_t *IMFS_node_remove_directory(
  IMFS_jnode_t *node,
  const IMFS_jnode_t *root_node
)
{
  if ( !rtems_chain_is_empty( &node->info.directory.Entries ) ) {
    errno = ENOTEMPTY;
    node = NULL;
  } else if ( node == root_node || IMFS_is_mount_point( node ) ) {
    errno = EBUSY;
    node = NULL;
  }

  return node;
}

const IMFS_node_control IMFS_node_control_directory = {
  .imfs_type = IMFS_DIRECTORY,
  .handlers = &IMFS_directory_handlers,
  .node_initialize = IMFS_node_initialize_directory,
  .node_remove = IMFS_node_remove_directory,
  .node_destroy = IMFS_node_destroy_default
};
