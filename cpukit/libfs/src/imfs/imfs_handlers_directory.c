/**
 * @file
 *
 * @brief Operations Table for Directories
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

#include <dirent.h>

static size_t IMFS_directory_size( const IMFS_jnode_t *node )
{
  size_t size = 0;
  const rtems_chain_control *chain = &node->info.directory.Entries;
  const rtems_chain_node *current = rtems_chain_immutable_first( chain );
  const rtems_chain_node *tail = rtems_chain_immutable_tail( chain );

  while ( current != tail ) {
    size += sizeof( struct dirent );
    current = rtems_chain_immutable_next( current );
  }

  return size;
}

static int IMFS_stat_directory(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
)
{
  const IMFS_jnode_t *node = loc->node_access;

  buf->st_size = IMFS_directory_size( node );

  return IMFS_stat( loc, buf );
}

static const rtems_filesystem_file_handlers_r IMFS_directory_handlers = {
  .open_h = rtems_filesystem_default_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = imfs_dir_read,
  .write_h = rtems_filesystem_default_write,
  .ioctl_h = rtems_filesystem_default_ioctl,
  .lseek_h = rtems_filesystem_default_lseek_directory,
  .fstat_h = IMFS_stat_directory,
  .ftruncate_h = rtems_filesystem_default_ftruncate_directory,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync_success,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync_success,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
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
  IMFS_jnode_t *node
)
{
  if ( !rtems_chain_is_empty( &node->info.directory.Entries ) ) {
    errno = ENOTEMPTY;
    node = NULL;
  } else if ( IMFS_is_mount_point( node ) ) {
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
