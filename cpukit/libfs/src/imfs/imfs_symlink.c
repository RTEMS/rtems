/*
 *  IMFS_symlink
 *
 *  The following rouine creates a new symbolic link node under parent 
 *  with the name given in name.  The node is set to point to the node at
 *  to_loc.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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

#include <errno.h>
#include "imfs.h"
#include <rtems/libio_.h>
#include <rtems/seterr.h>

int IMFS_symlink(
  rtems_filesystem_location_info_t  *parent_loc,
  const char                        *link_name,
  const char                        *node_name
)
{
  IMFS_types_union   info;
  IMFS_jnode_t      *new_node;
  char               new_name[ IMFS_NAME_MAX + 1 ];
  int                i;

  /*
   * Remove any separators at the end of the string.
   */

  IMFS_get_token( node_name, new_name, &i );

  info.sym_link.name = link_name;

  /*
   *  Create a new link node.
   */

  new_node = IMFS_create_node(
    parent_loc,
    IMFS_SYM_LINK,
    new_name,
    ( S_IFLNK | ( S_IRWXU | S_IRWXG | S_IRWXO )),
    &info
  );

  if ( !new_node )
    rtems_set_errno_and_return_minus_one( ENOMEM );

  return 0;
}


