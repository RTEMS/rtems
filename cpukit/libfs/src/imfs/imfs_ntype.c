/*
 *  IMFS_node_type
 *
 *  The following verifies that returns the type of node that the 
 *  loc refers to.
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

rtems_filesystem_node_types_t IMFS_node_type(
   rtems_filesystem_location_info_t    *pathloc         /* IN */
)
{
  IMFS_jnode_t *node;

  node = pathloc->node_access;
  return node->type;
}
