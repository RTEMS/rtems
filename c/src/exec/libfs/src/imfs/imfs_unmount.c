/*
 *  IMFS_unmount
 *
 *  This routine will look at a mount table entry that we are going to
 *  add to the mount table. If the mount point
 *  rtems_filesystem_location_info_t struct refers to a node that is a
 *  directory that has a file system mounted on it, the node will be
 *  marked as a mount point by * setting its directory.mt_fs pointer
 *  to NULL. This indicates that a directory is no longer mounted on
 *  this node.
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

#include <errno.h>

#include "imfs.h"
#include <rtems/libio_.h>
#include <rtems/seterr.h>

int IMFS_unmount(
  rtems_filesystem_mount_table_entry_t *mt_entry
)
{
  IMFS_jnode_t  *node;

  node = mt_entry->mt_point_node.node_access;

  /* 
   * Is the node that we are mounting onto a directory node ? 
   */

  if ( node->type != IMFS_DIRECTORY )
    rtems_set_errno_and_return_minus_one( ENOTDIR );

  /* 
   * Did the node indicate that there was a directory mounted here? 
   */

  if ( node->info.directory.mt_fs == NULL )
    rtems_set_errno_and_return_minus_one( EINVAL );  /* XXX */

  /* 
   * Set the mt_fs pointer to indicate that there is no longer 
   * a file system mounted to this point.
   */
  
  node->info.directory.mt_fs = NULL;

  return 0; 
}
