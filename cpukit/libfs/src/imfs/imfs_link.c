/**
 * @file
 *
 * @brief IMFS Create a New Link Node
 * @ingroup IMFS
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

int IMFS_link(
  const rtems_filesystem_location_info_t *parentloc,
  const rtems_filesystem_location_info_t *targetloc,
  const char *name,
  size_t namelen
)
{
  IMFS_jnode_t *new_node;
  IMFS_jnode_t *target;

  target = targetloc->node_access;

  /*
   *  Verify this node can be linked to.
   */
  if ( target->st_nlink >= LINK_MAX )
    rtems_set_errno_and_return_minus_one( EMLINK );

  /*
   *  Create a new link node.
   */
  new_node = IMFS_create_node(
    parentloc,
    IMFS_HARD_LINK,
    name,
    namelen,
    IMFS_STAT_FMT_HARD_LINK | ( S_IRWXU | S_IRWXG | S_IRWXO ),
    target
  );

  if ( !new_node )
    rtems_set_errno_and_return_minus_one( ENOMEM );

  /*
   * Increment the link count of the node being pointed to.
   */
  target->reference_count++;
  target->st_nlink++;
  IMFS_update_ctime( target );

  return 0;
}
