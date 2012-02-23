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
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

static int IMFS_rmnod_directory(
  const rtems_filesystem_location_info_t *parentloc,
  const rtems_filesystem_location_info_t *loc
)
{
  int rv = 0;
  IMFS_jnode_t *node = loc->node_access;

  if ( !rtems_chain_is_empty( &node->info.directory.Entries ) ) {
    errno = ENOTEMPTY;
    rv = -1;
  } else if (
    rtems_filesystem_location_is_root( loc )
      || node->info.directory.mt_fs != NULL
  ) {
    errno = EBUSY;
    rv = -1;
  }

  return rv;
}

static int IMFS_rmnod_hard_link(
  const rtems_filesystem_location_info_t *parentloc,
  const rtems_filesystem_location_info_t *loc
)
{
  int rv = 0;
  IMFS_jnode_t *node = loc->node_access;
  IMFS_jnode_t *target = node->info.hard_link.link_node;

  if ( target->st_nlink == 1) {
    rtems_filesystem_location_info_t target_loc = *loc;

    target_loc.node_access = target;
    IMFS_Set_handlers( &target_loc );

    rv = (*target_loc.ops->rmnod_h)( parentloc, &target_loc );
  } else {
    --target->st_nlink;
    IMFS_update_ctime( target );
  }

  return rv;
}

int IMFS_rmnod(
  const rtems_filesystem_location_info_t *parentloc,
  const rtems_filesystem_location_info_t *loc
)
{
  int rv = 0;
  IMFS_jnode_t *node = loc->node_access;

  switch ( node->type ) {
    case IMFS_DIRECTORY:
      rv = IMFS_rmnod_directory( parentloc, loc );
      break;
    case IMFS_HARD_LINK:
      rv = IMFS_rmnod_hard_link( parentloc, loc );
      break;
    default:
      break;
  }

  if ( rv == 0 ) {
    --node->reference_count;
    --node->st_nlink;
    if ( node->Parent != NULL ) {
      IMFS_remove_from_directory( node );
    }
  }

  return rv;
}
