/*
 *  IMFS_mount
 *  
 *  This routine will look at a mount table entry that we are going to
 *  add to the mount table. If the mount point rtems_filesystem
 *  location_info_t struct refers to a node that is a directory,
 *  the node will be marked as a mount point by setting its directory.mt_fs
 *  pointer to point to the mount table entry that we are about to add
 *  to the mount table chain.
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

int IMFS_mount(
  rtems_filesystem_mount_table_entry_t *mt_entry
)
{
  IMFS_jnode_t  *node;

  node = mt_entry->mt_point_node.node_access;

  /*
   *  Is the node that we are mounting onto a directory node ?
   */
  
  if ( node->type != IMFS_DIRECTORY )
    rtems_set_errno_and_return_minus_one( ENOTDIR );

  /*
   *  Set mt_fs pointer to point to the mount table entry for
   *  the mounted file system.
   */

  node->info.directory.mt_fs = mt_entry;
  return 0;
}
