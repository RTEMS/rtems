/*
 *  IMFS Initialization
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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

#if defined(IMFS_DEBUG)
#include <stdio.h>
#endif

/*
 *  IMFS_initialize
 */

int IMFS_initialize_support(
  rtems_filesystem_mount_table_entry_t *temp_mt_entry,
   rtems_filesystem_operations_table    *op_table,
   rtems_filesystem_file_handlers_r     *linearfile_handlers,
   rtems_filesystem_file_handlers_r     *memfile_handlers,
   rtems_filesystem_file_handlers_r     *directory_handlers
)
{
  IMFS_fs_info_t                        *fs_info;
  IMFS_jnode_t                          *jnode;

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
    return 1;
  }
  temp_mt_entry->fs_info = fs_info;

  /*
   * Set st_ino for the root to 1.
   */

  fs_info->ino_count             = 1;
  fs_info->linearfile_handlers   = linearfile_handlers;
  fs_info->memfile_handlers      = memfile_handlers;
  fs_info->directory_handlers    = directory_handlers;

  jnode = temp_mt_entry->mt_fs_root.node_access;
  jnode->st_ino = fs_info->ino_count;

  return 0;
}
