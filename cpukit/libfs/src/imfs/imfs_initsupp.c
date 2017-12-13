/**
 * @file
 *
 * @brief IMFS Node Support
 * @ingroup IMFS
 */

/*
 *  COPYRIGHT (c) 1989-2010.
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

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int IMFS_determine_bytes_per_block(
  int *dest_bytes_per_block,
  int requested_bytes_per_block,
  int default_bytes_per_block
)
{
  bool is_valid = false;
  int bit_mask;

  /*
   * check, whether requested bytes per block is valid
   */
  for (bit_mask = 16; !is_valid && (bit_mask <= 512); bit_mask <<= 1) {
    if (bit_mask == requested_bytes_per_block) {
      is_valid = true;
      break;
    }
    if(bit_mask > requested_bytes_per_block)
      break;
  }
  *dest_bytes_per_block = ((is_valid)
			   ? requested_bytes_per_block
			   : default_bytes_per_block);
  return 0;
}

int IMFS_initialize_support(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
)
{
  const IMFS_mount_data *mount_data = data;
  IMFS_fs_info_t *fs_info = mount_data->fs_info;
  IMFS_jnode_t *root_node;

  fs_info->mknod_controls = mount_data->mknod_controls;

  root_node = IMFS_initialize_node(
    &fs_info->Root_directory.Node,
    &fs_info->mknod_controls->directory->node_control,
    "",
    0,
    (S_IFDIR | 0755),
    NULL
  );
  IMFS_assert( root_node != NULL );

  mt_entry->fs_info = fs_info;
  mt_entry->ops = mount_data->ops;
  mt_entry->pathconf_limits_and_options = &IMFS_LIMITS_AND_OPTIONS;
  mt_entry->mt_fs_root->location.node_access = root_node;
  IMFS_Set_handlers( &mt_entry->mt_fs_root->location );

  IMFS_determine_bytes_per_block(
    &imfs_memfile_bytes_per_block,
    imfs_rq_memfile_bytes_per_block,
    IMFS_MEMFILE_DEFAULT_BYTES_PER_BLOCK
  );

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
  {
    .handlers = &rtems_filesystem_handlers_default,
    .node_initialize = IMFS_node_initialize_enosys,
    .node_remove = IMFS_node_remove_default,
    .node_destroy = IMFS_node_destroy_default
  },
  .node_size = sizeof( IMFS_jnode_t )
};
