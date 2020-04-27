/**
 * @file
 *
 * @ingroup IMFS
 *
 * @brief IMFS Node Support
 */

/*
 *  COPYRIGHT (c) 1989-2010.
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

#include <errno.h>

int IMFS_initialize_support(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
)
{
  const IMFS_mount_data   *mount_data;
  IMFS_fs_info_t          *fs_info;
  const IMFS_node_control *node_control;
  IMFS_jnode_t            *root_node;

  mount_data = data;

  fs_info = mount_data->fs_info;
  fs_info->mknod_controls = mount_data->mknod_controls;
  node_control = &mount_data->mknod_controls->directory->node_control;
  root_node = &fs_info->Root_directory.Node;

  mt_entry->fs_info = fs_info;
  mt_entry->ops = mount_data->ops;
  mt_entry->pathconf_limits_and_options = &IMFS_LIMITS_AND_OPTIONS;
  mt_entry->mt_fs_root->location.node_access = root_node;
  mt_entry->mt_fs_root->location.handlers = node_control->handlers;

  root_node = IMFS_initialize_node(
    root_node,
    node_control,
    "",
    0,
    (S_IFDIR | 0755),
    NULL
  );
  IMFS_assert( root_node != NULL );

  return 0;
}

static IMFS_jnode_t *IMFS_node_initialize_enosys(
  IMFS_jnode_t *node,
  void *arg
)
{
  errno = ENOSYS;

  return NULL;
}

IMFS_jnode_t *IMFS_node_initialize_default(
  IMFS_jnode_t *node,
  void *arg
)
{
  return node;
}

const IMFS_mknod_control IMFS_mknod_control_enosys = {
  { .node_initialize = IMFS_node_initialize_enosys },
  .node_size = sizeof( IMFS_jnode_t )
};
