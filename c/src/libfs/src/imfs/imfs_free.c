/*
 *  Free IMFS Node Support Routines
 *
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

/*
 *  IMFS_freenodinfo
 *
 *  This routine is the IMFS free node handler for the file system
 *  operations table.  
 * 
 *  The In Memory File System keeps its nodes in memory.  This routine
 *  is for file sytems that do not.
 */

int IMFS_freenodinfo(
 rtems_filesystem_location_info_t      *pathloc       /* IN */
)
{
  return 0;
}


/*
 *  IMFS_freenod
 *
 *  The following routine frees a node if possible.
 *
 *  The routine returns 0 if the node was not freed and 1 if it was.
 *
 *  NOTE:  This routine is for INTERNAL IMFS use only.
 */

int IMFS_freenod(
  rtems_filesystem_location_info_t  *pathloc
)
{
  IMFS_jnode_t       *the_jnode;

  the_jnode = pathloc->node_access;

  if ( the_jnode->type == IMFS_DIRECTORY ) {

    /* 
     * You cannot remove a node that still has children 
     */

    if ( ! Chain_Is_empty( &the_jnode->info.directory.Entries ) )
       return ENOTEMPTY;

    /* 
     * You cannot remove the file system root node.
     */
    if ( pathloc->mt_entry->mt_fs_root.node_access == pathloc->node_access )
       return EBUSY;

    /* 
     * You cannot remove a mountpoint.
     */
     if ( the_jnode->info.directory.mt_fs != NULL )
       return EBUSY;       
  } 

  if ( !rtems_libio_is_file_open( the_jnode ) && 
       (the_jnode->st_nlink < 1) ) {

    /* 
     * Is the rtems_filesystem_current is this node?
     */
    if ( rtems_filesystem_current.node_access == pathloc->node_access ) {
      rtems_filesystem_current.node_access = NULL;
    }

    /*
     * Free memory associated with a memory file.
     */
    if ( the_jnode->type == IMFS_MEMORY_FILE ) 
      IMFS_memfile_remove( the_jnode );

    free( the_jnode );
  } 

  return 0;
}
