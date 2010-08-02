/*
 *  IMFS_create_node()
 *
 *  Routine to create a new in memory file system node.
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

#include <stdlib.h>
#include <string.h>
#include "imfs.h"
#include <rtems/libio_.h>

/*
 *  Create an IMFS filesystem node of an arbitrary type that is NOT
 *  the root directory node.
 */
IMFS_jnode_t *IMFS_create_node(
  rtems_filesystem_location_info_t *parent_loc,
  IMFS_jnode_types_t                type,
  const char                       *name,
  mode_t                            mode,
  const IMFS_types_union           *info
)
{
  IMFS_jnode_t        *node;
  IMFS_jnode_t        *parent;
  IMFS_fs_info_t      *fs_info;

  /*
   *  MUST have a parent node to call this routine.
   */
  if ( parent_loc == NULL )
    return NULL;

  parent = parent_loc->node_access;
  fs_info = parent_loc->mt_entry->fs_info;

  /*
   *  Reject creation of FIFOs if support is disabled.
   */
  if ( type == IMFS_FIFO &&
       fs_info->fifo_handlers == &rtems_filesystem_handlers_default )
    return NULL;

  /*
   *  Allocate filesystem node and fill in basic information
   */
  node  = IMFS_allocate_node( type, name, mode & ~rtems_filesystem_umask );
  if ( !node )
    return NULL;

  /*
   *  Set the type specific information
   */
  if ( type == IMFS_DIRECTORY ) {
    rtems_chain_initialize_empty(&node->info.directory.Entries);
  } else if ( type == IMFS_HARD_LINK ) {
    node->info.hard_link.link_node = info->hard_link.link_node;
  } else if ( type == IMFS_SYM_LINK ) {
    node->info.sym_link.name = info->sym_link.name;
  } else if ( type == IMFS_DEVICE ) {
    node->info.device.major = info->device.major;
    node->info.device.minor = info->device.minor;
  } else if ( type == IMFS_LINEAR_FILE ) {
    node->info.linearfile.size      = 0;
    node->info.linearfile.direct    = 0;
    if ( type == IMFS_MEMORY_FILE ) {
      node->info.file.size            = 0;
      node->info.file.indirect        = 0;
      node->info.file.doubly_indirect = 0;
      node->info.file.triply_indirect = 0;
    }
  } else if ( type == IMFS_FIFO ) {
    node->info.fifo.pipe = NULL;
  } else {
    IMFS_assert(0);
  }

  /*
   *  This node MUST have a parent, so put it in that directory list.
   */
  node->Parent = parent;
  node->st_ino = ++fs_info->ino_count;

  rtems_chain_append( &parent->info.directory.Entries, &node->Node );

  return node;
}

/*
 *  Allocate filesystem node and fill in basic information
 */
IMFS_jnode_t *IMFS_allocate_node(
  IMFS_jnode_types_t                type,
  const char                       *name,
  mode_t                            mode
)
{
  IMFS_jnode_t        *node;
  struct timeval       tv;

  /*
   *  Allocate an IMFS jnode
   */
  node = calloc( 1, sizeof( IMFS_jnode_t ) );
  if ( !node )
    return NULL;

  /*
   *  Fill in the basic information
   */
  node->st_nlink = 1;
  node->type     = type;
  strncpy( node->name, name, IMFS_NAME_MAX );

  /*
   *  Fill in the mode and permission information for the jnode structure.
   */
  node->st_mode = mode;
  #if defined(RTEMS_POSIX_API)
    node->st_uid = geteuid();
    node->st_gid = getegid();
  #else
    node->st_uid = 0;
    node->st_gid = 0;
  #endif

  /*
   *  Now set all the times.
   */
  gettimeofday( &tv, 0 );

  node->stat_atime  = (time_t) tv.tv_sec;
  node->stat_mtime  = (time_t) tv.tv_sec;
  node->stat_ctime  = (time_t) tv.tv_sec;

  return node;
}

IMFS_jnode_t *IMFS_create_root_node(void)
{
  IMFS_jnode_t        *node;

  /*
   *  Allocate filesystem node and fill in basic information
   */
  node = IMFS_allocate_node( IMFS_DIRECTORY, "", (S_IFDIR | 0755) );
  if ( !node )
    return NULL;

  /*
   *  Set the type specific information
   *
   *  NOTE: Root node is always a directory.
   */
  rtems_chain_initialize_empty(&node->info.directory.Entries);

  return node;
}
