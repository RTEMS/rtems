/*
 *  IMFS_unlink 
 *  
 *  Routine to remove a link node from the tree.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <errno.h>
#include <stdlib.h>

#include "imfs.h"
#include "libio_.h"

int IMFS_unlink(
  rtems_filesystem_location_info_t  *loc       /* IN */
)
{
  IMFS_jnode_t                      *node;
  rtems_filesystem_location_info_t   the_link;
  int                                result;
  
  node = loc->node_access;

  /*
   * Decrement the link counter of node pointed to and free the
   * space.
   */

  /*
   * If this is the last last pointer to the node
   * free the node.
   */

  if ( node->type == IMFS_HARD_LINK ) {

    if ( !node->info.hard_link.link_node )
      set_errno_and_return_minus_one( EINVAL );

    the_link = *loc;
    the_link.node_access = node->info.hard_link.link_node;

    /*
     *  If removing the last hard link to a node, then we need
     *  to remove the node that is a link and the node itself.
     */

    node->info.hard_link.link_node->st_nlink --;
    IMFS_update_ctime( node->info.hard_link.link_node );
    if ( node->info.hard_link.link_node->st_nlink < 1) {
      result = (*loc->handlers->rmnod)( &the_link );
      if ( result != 0 )
        return -1;
    }
  }

  /*
   *  Now actually free the node we were asked to free.
   */

  result = (*loc->handlers->rmnod)( loc );

  return result;
}
