/*
 *  IMFS_unlink 
 *  
 *  Routine to remove a link node from the tree.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <stdlib.h>

#include "imfs.h"
#include <rtems/libio_.h>
#include <rtems/seterr.h>

int IMFS_unlink(
  rtems_filesystem_location_info_t  *loc       /* IN */
)
{
  IMFS_jnode_t                      *node;
  rtems_filesystem_location_info_t   the_link;
  int                                result = 0;
  
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
      rtems_set_errno_and_return_minus_one( EINVAL );

    the_link = *loc;
    the_link.node_access = node->info.hard_link.link_node;
    IMFS_Set_handlers( &the_link );

    /*
     *  If removing the last hard link to a node, then we need
     *  to remove the node that is a link and the node itself.
     */

    if ( node->info.hard_link.link_node->st_nlink == 1)
    {
        result = (*the_link.handlers->rmnod_h)( &the_link );
        if ( result != 0 )
            return -1;
    }
    else
    {
        node->info.hard_link.link_node->st_nlink --;
        IMFS_update_ctime( node->info.hard_link.link_node );
    }
  }

  /*
   *  Now actually free the node we were asked to free.
   */

  result = (*loc->handlers->rmnod_h)( loc );

  return result;
}

