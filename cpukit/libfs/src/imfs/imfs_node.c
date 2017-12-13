/**
 * @file
 *
 * @brief IMFS Node Support
 * @ingroup IMFS
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
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

IMFS_jnode_t *IMFS_initialize_node(
  IMFS_jnode_t *node,
  const IMFS_node_control *node_control,
  const char *name,
  size_t namelen,
  mode_t mode,
  void *arg
)
{
  struct timeval tv;

  if ( namelen > IMFS_NAME_MAX ) {
    errno = ENAMETOOLONG;

    return NULL;
  }

  gettimeofday( &tv, 0 );

  /*
   *  Fill in the basic information
   */
  node->name = name;
  node->namelen = namelen;
  node->reference_count = 1;
  node->st_nlink = 1;
  node->control = node_control;

  /*
   *  Fill in the mode and permission information for the jnode structure.
   */
  node->st_mode = mode;
  node->st_uid = geteuid();
  node->st_gid = getegid();

  /*
   *  Now set all the times.
   */

  node->stat_atime  = (time_t) tv.tv_sec;
  node->stat_mtime  = (time_t) tv.tv_sec;
  node->stat_ctime  = (time_t) tv.tv_sec;

  return (*node_control->node_initialize)( node, arg );
}

int IMFS_node_clone( rtems_filesystem_location_info_t *loc )
{
  IMFS_jnode_t *node = loc->node_access;

  ++node->reference_count;

  return 0;
}

void IMFS_node_destroy( IMFS_jnode_t *node )
{
  IMFS_assert( node->reference_count == 0 );

  (*node->control->node_destroy)( node );
}

void IMFS_node_free( const rtems_filesystem_location_info_t *loc )
{
  IMFS_jnode_t *node = loc->node_access;

  --node->reference_count;

  if ( node->reference_count == 0 ) {
    IMFS_node_destroy( node );
  }
}

IMFS_jnode_t *IMFS_node_remove_default(
  IMFS_jnode_t *node
)
{
  return node;
}

void IMFS_node_destroy_default( IMFS_jnode_t *node )
{
  if ( ( node->flags & IMFS_NODE_FLAG_NAME_ALLOCATED ) != 0 ) {
    free( RTEMS_DECONST( char *, node->name ) );
  }

  free( node );
}
