/*
 *  IMFS Node Removal Handler
 *
 *  This file contains the handler used to remove a node when a file type
 *  does not require special actions.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

int IMFS_rmnod(
  const rtems_filesystem_location_info_t *parentloc,
  const rtems_filesystem_location_info_t *loc
)
{
  int rv = 0;
  IMFS_jnode_t *node = loc->node_access;

  node = (*node->control->node_remove)(
    node,
    loc->mt_entry->mt_fs_root->location.node_access
  );
  if ( node != NULL ) {
    --node->reference_count;
    --node->st_nlink;
    if ( node->Parent != NULL ) {
      IMFS_remove_from_directory( node );
    }
  } else {
    rv = -1;
  }

  return rv;
}
