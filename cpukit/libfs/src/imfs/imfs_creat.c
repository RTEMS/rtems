/**
 * @file
 *
 * @brief Create an IMFS Node
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

#include <stdlib.h>
#include <string.h>

IMFS_jnode_t *IMFS_create_node(
  const rtems_filesystem_location_info_t *parentloc,
  const IMFS_node_control *node_control,
  size_t node_size,
  const char *name,
  size_t namelen,
  mode_t mode,
  void *arg
)
{
  IMFS_jnode_t *allocated_node;
  IMFS_jnode_t *node;

  allocated_node = calloc( 1, node_size + namelen );
  if ( allocated_node == NULL ) {
    errno = ENOMEM;

    return NULL;
  }

  node = IMFS_initialize_node(
    allocated_node,
    node_control,
    (char *) allocated_node + node_size,
    namelen,
    mode,
    arg
  );
  if ( node != NULL ) {
    IMFS_jnode_t *parent = parentloc->node_access;

    memcpy( RTEMS_DECONST( char *, node->name ), name, namelen );

    /*
     *  This node MUST have a parent, so put it in that directory list.
     */
    IMFS_assert( parent != NULL );
    IMFS_add_to_directory( parent, node );
  } else {
    free( allocated_node );
  }

  return node;
}

