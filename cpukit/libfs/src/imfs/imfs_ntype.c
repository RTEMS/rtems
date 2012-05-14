/*
 *  IMFS_node_type
 *
 *  The following verifies that returns the type of node that the
 *  loc refers to.
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

rtems_filesystem_node_types_t IMFS_node_type(
  const rtems_filesystem_location_info_t *loc
)
{
  const IMFS_jnode_t *node = loc->node_access;
  IMFS_jnode_types_t imfs_type = IMFS_type( node );
  rtems_filesystem_node_types_t type;

  switch ( imfs_type ) {
    case IMFS_HARD_LINK:
      type = IMFS_type( node->info.hard_link.link_node );
      break;
    case IMFS_LINEAR_FILE:
      type = RTEMS_FILESYSTEM_MEMORY_FILE;
      break;
    default:
      type = imfs_type;
      break;
  }

  return type;
}
