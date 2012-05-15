/*
 *  IMFS Initialization
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

#include <stdlib.h>

/*
 *  IMFS_determine_bytes_per_block
 */
int imfs_memfile_bytes_per_block = 0;

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
  const rtems_filesystem_operations_table *op_table,
  const IMFS_node_control *const node_controls [IMFS_TYPE_COUNT]
)
{
  static int imfs_instance;

  int rv = 0;
  IMFS_fs_info_t *fs_info = calloc( 1, sizeof( *fs_info ) );

  if ( fs_info != NULL ) {
    IMFS_jnode_t *root_node;

    fs_info->instance = imfs_instance++;
    memcpy(
      fs_info->node_controls,
      node_controls,
      sizeof( fs_info->node_controls )
    );

    root_node = IMFS_allocate_node(
      fs_info,
      fs_info->node_controls [IMFS_DIRECTORY],
      "",
      0,
      (S_IFDIR | 0755),
      NULL
    );
    if ( root_node != NULL ) {
      mt_entry->fs_info = fs_info;
      mt_entry->ops = op_table;
      mt_entry->pathconf_limits_and_options = &IMFS_LIMITS_AND_OPTIONS;
      mt_entry->mt_fs_root->location.node_access = root_node;
      IMFS_Set_handlers( &mt_entry->mt_fs_root->location );
    } else {
      errno = ENOMEM;
      rv = -1;
    }
  } else {
    errno = ENOMEM;
    rv = -1;
  }

  if ( rv == 0 ) {
    IMFS_determine_bytes_per_block(
      &imfs_memfile_bytes_per_block,
      imfs_rq_memfile_bytes_per_block,
      IMFS_MEMFILE_DEFAULT_BYTES_PER_BLOCK
    );
  }

  return rv;
}

int IMFS_node_clone( rtems_filesystem_location_info_t *loc )
{
  IMFS_jnode_t *node = loc->node_access;

  ++node->reference_count;

  return 0;
}

void IMFS_node_destroy( IMFS_jnode_t *node )
{
  IMFS_assert( node->reference_count == 0 );

  node = (*node->control->node_destroy)( node );

  free( node );
}

void IMFS_node_free( const rtems_filesystem_location_info_t *loc )
{
  IMFS_jnode_t *node = loc->node_access;

  if ( node->reference_count == 1 ) {
    IMFS_node_destroy( node );
  } else {
    --node->reference_count;
  }
}

IMFS_jnode_t *IMFS_node_initialize_default(
  IMFS_jnode_t *node,
  const IMFS_types_union *info
)
{
  return node;
}

IMFS_jnode_t *IMFS_node_remove_default(
  IMFS_jnode_t *node,
  const IMFS_jnode_t *root_node
)
{
  return node;
}

IMFS_jnode_t *IMFS_node_destroy_default( IMFS_jnode_t *node )
{
  return node;
}

const IMFS_node_control IMFS_node_control_default = {
  .imfs_type = IMFS_INVALID_NODE,
  .handlers = &rtems_filesystem_handlers_default,
  .node_initialize = IMFS_node_initialize_default,
  .node_remove = IMFS_node_remove_default,
  .node_destroy = IMFS_node_destroy_default
};
