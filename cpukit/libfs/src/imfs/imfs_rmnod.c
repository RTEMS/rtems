/*
 *  IMFS_rmnod
 *
 *  This routine is available from the optable to remove a node 
 *  from the IMFS file system.
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

#include <errno.h>
#include "libio_.h"
#include "imfs.h"


int IMFS_rmnod(
  rtems_filesystem_location_info_t      *pathloc       /* IN */
)
{
  IMFS_jnode_t *the_jnode;  

  the_jnode = (IMFS_jnode_t *) pathloc->node_access;

  if ( the_jnode->type == IMFS_DIRECTORY ) {

    /* 
     * You cannot remove a node that still has children 
     */

    if ( ! Chain_Is_empty( &the_jnode->info.directory.Entries ) )
       set_errno_and_return_minus_one( ENOTEMPTY );

    /* 
     * You cannot remove the file system root node.
     */

    if ( pathloc->mt_entry->mt_fs_root.node_access == pathloc->node_access )
       set_errno_and_return_minus_one( EBUSY );

    /* 
     * You cannot remove a mountpoint.
     */

     if ( the_jnode->info.directory.mt_fs != NULL )
       set_errno_and_return_minus_one( EBUSY );          
  } 
    
  /* 
   * Take the node out of the parent's chain that contains this node 
   */

  if ( the_jnode->Parent != NULL ) {
    Chain_Extract( (Chain_Node *) the_jnode );
    the_jnode->Parent = NULL;
  }

  /*
   * Decrement the link counter and see if we can free the space.
   */

  the_jnode->st_nlink--;
  IMFS_update_ctime( the_jnode );

  /*
   * The file cannot be open and the link must be less than 1 to free.
   */

  if ( !rtems_libio_is_file_open( the_jnode ) && (the_jnode->st_nlink < 1) ) {

    /* 
     * Is the rtems_filesystem_current is this node?
     */

    if ( rtems_filesystem_current.node_access == pathloc->node_access )
       rtems_filesystem_current.node_access = NULL;

    /*
     * Free memory associated with a memory file.
     */

    if ( the_jnode->type == IMFS_MEMORY_FILE ) 
      IMFS_memfile_remove( the_jnode );

    free( the_jnode );
  }

  return 0;

}


