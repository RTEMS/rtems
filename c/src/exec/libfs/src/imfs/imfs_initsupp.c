/*
 *  IMFS Initialization
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <sys/types.h>         /* for mkdir */
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <assert.h>

#include "imfs.h"
#include "libio_.h"

#if defined(IMFS_DEBUG)
#include <stdio.h>
#endif

/*
 *  IMFS file system operations table
 */

rtems_filesystem_operations_table  IMFS_ops = {
  IMFS_eval_path,
  IMFS_evaluate_for_make,
  IMFS_link,
  IMFS_unlink,
  IMFS_node_type,
  IMFS_mknod,
  IMFS_rmnod,
  IMFS_chown,
  IMFS_freenodinfo,
  IMFS_mount,
  IMFS_initialize,
  IMFS_unmount,
  IMFS_fsunmount,
  IMFS_utime, 
  IMFS_evaluate_link,
  IMFS_symlink,
  IMFS_readlink
};

/*
 *  IMFS_initialize
 */

int IMFS_initialize(
  rtems_filesystem_mount_table_entry_t *temp_mt_entry
)
{
  IMFS_fs_info_t                        *fs_info;
  IMFS_jnode_t                          *jnode;

  /*
   *  Create the root node
   */

  temp_mt_entry->mt_fs_root.node_access = IMFS_create_node(
    NULL,
    IMFS_DIRECTORY,
    "",
    ( S_IRWXO | S_IRWXG| S_IRWXU ),
    NULL
  );

  temp_mt_entry->mt_fs_root.handlers    = &dir_handlers;
  temp_mt_entry->mt_fs_root.ops         = &IMFS_ops;
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

  fs_info->ino_count   = 1;

  jnode = temp_mt_entry->mt_fs_root.node_access;
  jnode->st_ino = fs_info->ino_count;

  return 0;
}

#define jnode_get_control( jnode ) \
  (&jnode->info.directory.Entries)

#define jnode_has_no_children( jnode )  \
  Chain_Is_empty( jnode_get_control( jnode ) )

#define jnode_has_children( jnode ) \
  ( ! jnode_has_no_children( jnode ) )

#define jnode_get_first_child( jnode ) \
    ((IMFS_jnode_t *)( Chain_Head( jnode_get_control( jnode ) )->next))


int IMFS_fsunmount(
  rtems_filesystem_mount_table_entry_t *temp_mt_entry
)
{
   IMFS_jnode_t                     *jnode;
   IMFS_jnode_t                     *next;
   rtems_filesystem_location_info_t loc;       
   int                              result = 0;

   /* 
    * Traverse tree that starts at the mt_fs_root and deallocate memory 
    * associated memory space
    */
    
   jnode = (IMFS_jnode_t *)temp_mt_entry->mt_fs_root.node_access;

   do {
     next = jnode->Parent;
     loc.node_access = (void *)jnode;

     if ( jnode->type != IMFS_DIRECTORY ) {
        result = IMFS_unlink( &loc );
        if (result != 0)
          return -1;
        jnode = next;
     } else if ( jnode_has_no_children( jnode ) ) {
        result = IMFS_unlink( &loc );
        if (result != 0)
          return -1;
        jnode = next;
     }
     if ( jnode != NULL ) {
       if ( jnode->type == IMFS_DIRECTORY ) {
         if ( jnode_has_children( jnode ) )
           jnode = jnode_get_first_child( jnode );
       }
     }
   } while (jnode != NULL);

   return 0;
}



