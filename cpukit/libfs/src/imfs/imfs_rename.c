/*
 *  IMFS_rename
 *
 *  The following rouine creates a new link node under parent with the
 *  name given in name and removes the old.
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

int IMFS_rename(
  const rtems_filesystem_location_info_t *oldparentloc,
  const rtems_filesystem_location_info_t *oldloc,
  const rtems_filesystem_location_info_t *newparentloc,
  const char *name,
  size_t namelen
)
{
  int rv = 0;
  IMFS_jnode_t *node = oldloc->node_access;
  IMFS_jnode_t *new_parent = newparentloc->node_access;

  /*
   * FIXME: Due to insufficient checks we can create inaccessible nodes with
   * this operation.
   */

  if ( node->Parent != NULL ) {
    if ( namelen < IMFS_NAME_MAX ) {
      memcpy( node->name, name, namelen );
      node->name [namelen] = '\0';

      IMFS_remove_from_directory( node );
      IMFS_add_to_directory( new_parent, node );
      IMFS_update_ctime( node );
    } else {
      errno = ENAMETOOLONG;
      rv = -1;
    }
  } else {
    errno = EINVAL;
    rv = -1;
  }

  return rv;
}
