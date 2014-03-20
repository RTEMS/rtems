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

#include "imfs.h"

#include <stdlib.h>
#include <string.h>

IMFS_jnode_t *IMFS_allocate_node(
  IMFS_fs_info_t *fs_info,
  const IMFS_node_control *node_control,
  const char *name,
  size_t namelen,
  mode_t mode,
  const IMFS_types_union *info
)
{
  IMFS_jnode_t        *node;
  IMFS_jnode_t        *initialized_node;
  struct timeval       tv;

  if ( namelen > IMFS_NAME_MAX ) {
    errno = ENAMETOOLONG;

    return NULL;
  }

  gettimeofday( &tv, 0 );

  /*
   *  Allocate an IMFS jnode
   */
  node = calloc( 1, sizeof( IMFS_jnode_t ) );
  if ( !node ) {
    errno = ENOMEM;

    return NULL;
  }

  /*
   *  Fill in the basic information
   */
  node->reference_count = 1;
  node->st_nlink = 1;
  memcpy( node->name, name, namelen );
  node->name [namelen] = '\0';
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
  node->st_ino = ++fs_info->ino_count;

  initialized_node = (*node->control->node_initialize)( node, info );
  if ( initialized_node == NULL ) {
    free( node );
  }

  return initialized_node;
}

IMFS_jnode_t *IMFS_create_node_with_control(
  const rtems_filesystem_location_info_t *parentloc,
  const IMFS_node_control *node_control,
  const char *name,
  size_t namelen,
  mode_t mode,
  const IMFS_types_union *info
)
{
  IMFS_fs_info_t *fs_info = parentloc->mt_entry->fs_info;
  IMFS_jnode_t *node = IMFS_allocate_node(
    fs_info,
    node_control,
    name,
    namelen,
    mode,
    info
  );

  if ( node != NULL ) {
    IMFS_jnode_t *parent = parentloc->node_access;

    /*
     *  This node MUST have a parent, so put it in that directory list.
     */
    IMFS_assert( parent != NULL );
    IMFS_add_to_directory( parent, node );
  }

  return node;
}

