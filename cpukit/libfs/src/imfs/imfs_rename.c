/**
 * @file
 *
 * @brief IMFS Rename
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

#include <rtems/imfs.h>

#include <string.h>
#include <stdlib.h>

int IMFS_rename(
  const rtems_filesystem_location_info_t *oldparentloc,
  const rtems_filesystem_location_info_t *oldloc,
  const rtems_filesystem_location_info_t *newparentloc,
  const char *name,
  size_t namelen
)
{
  IMFS_jnode_t *node = oldloc->node_access;
  IMFS_jnode_t *new_parent = newparentloc->node_access;
  char *allocated_name;

  /*
   * FIXME: Due to insufficient checks we can create inaccessible nodes with
   * this operation.
   */

  if ( node->Parent == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( namelen >= IMFS_NAME_MAX ) {
    rtems_set_errno_and_return_minus_one( ENAMETOOLONG );
  }

  allocated_name = malloc( namelen );
  if ( allocated_name == NULL ) {
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }

  memcpy( allocated_name, name, namelen );

  if ( ( node->flags & IMFS_NODE_FLAG_NAME_ALLOCATED ) != 0 ) {
    free( RTEMS_DECONST( char *, node->name ) );
  }

  node->name = allocated_name;
  node->namelen = namelen;
  node->flags |= IMFS_NODE_FLAG_NAME_ALLOCATED;

  IMFS_remove_from_directory( node );
  IMFS_add_to_directory( new_parent, node );
  IMFS_update_ctime( node );

  return 0;
}
