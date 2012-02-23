/*
 *  IMFS Initialization
 *
 *  COPYRIGHT (c) 1989-2010.
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


/*
 *  IMFS_initialize
 */
int IMFS_initialize_support(
  rtems_filesystem_mount_table_entry_t        *temp_mt_entry,
   const rtems_filesystem_operations_table    *op_table,
   const rtems_filesystem_file_handlers_r     *link_handlers,
   const rtems_filesystem_file_handlers_r     *fifo_handlers
)
{
  static int                             imfs_instance;
  IMFS_fs_info_t                        *fs_info;
  IMFS_jnode_t                          *jnode;

  /*
   * determine/check value for imfs_memfile_bytes_per_block
   */
  IMFS_determine_bytes_per_block(&imfs_memfile_bytes_per_block,
				 imfs_rq_memfile_bytes_per_block,
				 IMFS_MEMFILE_DEFAULT_BYTES_PER_BLOCK);

  /*
   *  Create the root node
   *
   *  NOTE: UNIX root is 755 and owned by root/root (0/0).
   */
  temp_mt_entry->mt_fs_root->location.node_access = IMFS_create_root_node();
  temp_mt_entry->mt_fs_root->location.handlers = &IMFS_directory_handlers;
  temp_mt_entry->mt_fs_root->location.ops = op_table;
  temp_mt_entry->pathconf_limits_and_options = IMFS_LIMITS_AND_OPTIONS;

  /*
   * Create custom file system data.
   */
  fs_info = calloc( 1, sizeof( IMFS_fs_info_t ) );
  if ( !fs_info ) {
    free(temp_mt_entry->mt_fs_root->location.node_access);
    rtems_set_errno_and_return_minus_one(ENOMEM);
  }
  temp_mt_entry->fs_info = fs_info;

  /*
   * Set st_ino for the root to 1.
   */

  fs_info->instance              = imfs_instance++;
  fs_info->ino_count             = 1;
  fs_info->link_handlers         = link_handlers;
  fs_info->fifo_handlers         = fifo_handlers;

  jnode = temp_mt_entry->mt_fs_root->location.node_access;
  jnode->st_ino = fs_info->ino_count;

  return 0;
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

  switch ( node->type ) {
    case IMFS_MEMORY_FILE:
      IMFS_memfile_remove( node );
      break;
    case IMFS_SYM_LINK:
      free( node->info.sym_link.name );
      break;
    default:
      break;
  }

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
