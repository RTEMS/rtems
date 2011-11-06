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

int IMFS_rename(
  rtems_filesystem_location_info_t  *old_parent_loc,  /* IN */
  rtems_filesystem_location_info_t  *old_loc,         /* IN */
  rtems_filesystem_location_info_t  *new_parent_loc,  /* IN */
  const char                        *new_name         /* IN */
)
{
  IMFS_jnode_t *the_jnode;
  IMFS_jnode_t *new_parent;

  the_jnode = old_loc->node_access;

  strncpy( the_jnode->name, new_name, IMFS_NAME_MAX );

  if ( the_jnode->Parent != NULL )
    rtems_chain_extract( (rtems_chain_node *) the_jnode );

  new_parent = new_parent_loc->node_access;
  the_jnode->Parent = new_parent;

  rtems_chain_append( &new_parent->info.directory.Entries, &the_jnode->Node );

  /*
   * Update the time.
   */
  IMFS_update_ctime( the_jnode );

  return 0;
}
