/*
 *  IMFS_symlink
 *
 *  The following rouine creates a new symbolic link node under parent
 *  with the name given in name.  The node is set to point to the node at
 *  to_loc.
 *
 *  COPYRIGHT (c) 1989-2009.
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

#include <stdlib.h>

int IMFS_symlink(
  const rtems_filesystem_location_info_t *parentloc,
  const char *name,
  size_t namelen,
  const char *target
)
{
  IMFS_types_union   info;
  IMFS_jnode_t      *new_node;

  /*
   * Duplicate link name
   */
  info.sym_link.name = strdup(target);
  if (info.sym_link.name == NULL) {
    rtems_set_errno_and_return_minus_one(ENOMEM);
  }

  /*
   *  Create a new link node.
   */
  new_node = IMFS_create_node(
    parentloc,
    IMFS_SYM_LINK,
    name,
    namelen,
    ( S_IFLNK | ( S_IRWXU | S_IRWXG | S_IRWXO )),
    &info
  );

  if (new_node == NULL) {
    free(info.sym_link.name);
    rtems_set_errno_and_return_minus_one(ENOMEM);
  }

  return 0;
}
