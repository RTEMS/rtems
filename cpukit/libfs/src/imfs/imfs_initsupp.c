/*
 *  IMFS Initialization
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

#include <sys/types.h>         /* for mkdir */
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <assert.h>

#include "imfs.h"
#include <rtems/libio_.h>
#include <rtems/seterr.h>

#if defined(IMFS_DEBUG)
#include <stdio.h>
#endif

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
  for (bit_mask = 16;
       !is_valid && (bit_mask <= 512); 
       bit_mask <<= 1) {
    if (bit_mask == requested_bytes_per_block) {
      is_valid = true;
    }
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
   const rtems_filesystem_file_handlers_r     *memfile_handlers,
   const rtems_filesystem_file_handlers_r     *directory_handlers
)
{
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

  temp_mt_entry->mt_fs_root.node_access = IMFS_create_node(
    NULL,
    IMFS_DIRECTORY,
    "",
    ( S_IFDIR | 0755 ),
    NULL
  );

  temp_mt_entry->mt_fs_root.handlers         = directory_handlers;
  temp_mt_entry->mt_fs_root.ops              = op_table;
  temp_mt_entry->pathconf_limits_and_options = IMFS_LIMITS_AND_OPTIONS;

  /*
   * Create custom file system data.
   */
  fs_info = calloc( 1, sizeof( IMFS_fs_info_t ) );
  if ( !fs_info ){
    free(temp_mt_entry->mt_fs_root.node_access);
    rtems_set_errno_and_return_minus_one(ENOMEM);
  }
  temp_mt_entry->fs_info = fs_info;

  /*
   * Set st_ino for the root to 1.
   */

  fs_info->ino_count             = 1;
  fs_info->memfile_handlers      = memfile_handlers;
  fs_info->directory_handlers    = directory_handlers;

  jnode = temp_mt_entry->mt_fs_root.node_access;
  jnode->st_ino = fs_info->ino_count;

  return 0;
}
