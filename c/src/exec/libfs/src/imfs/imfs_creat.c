/*
 *  IMFS_create_node()
 *
 *  Routine to create a new in memory file system node.
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

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "imfs.h"
#include <rtems/libio_.h>

IMFS_jnode_t *IMFS_create_node(
  rtems_filesystem_location_info_t  *parent_loc,
  IMFS_jnode_types_t                 type,
  char                              *name,
  mode_t                             mode,
  IMFS_types_union                  *info
)
{
  IMFS_jnode_t        *node;
  struct timeval       tv;
  IMFS_jnode_t        *parent = NULL;
  IMFS_fs_info_t      *fs_info;
  char                *sym_name;

  if ( parent_loc != NULL )
    parent = parent_loc->node_access;

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

  node->st_mode = mode & ~rtems_filesystem_umask;
  
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

  /*
   *  Set the type specific information
   */

  switch (type) {
    case IMFS_DIRECTORY:
      Chain_Initialize_empty(&node->info.directory.Entries);
      break;

    case IMFS_HARD_LINK:
      node->info.hard_link.link_node = info->hard_link.link_node;
      break;

    case IMFS_SYM_LINK:
      sym_name = calloc( 1, strlen( info->sym_link.name ) + 1 );
      strcpy( sym_name, info->sym_link.name );
      node->info.sym_link.name = sym_name;
      break;

    case IMFS_DEVICE:
      node->info.device.major = info->device.major;
      node->info.device.minor = info->device.minor;
      break;

    case IMFS_LINEAR_FILE:
      node->info.linearfile.size      = 0;
      node->info.linearfile.direct    = 0;

    case IMFS_MEMORY_FILE:
      node->info.file.size            = 0;
      node->info.file.indirect        = 0;
      node->info.file.doubly_indirect = 0;
      node->info.file.triply_indirect = 0;
      break;

    default:
      assert(0);
      break;
  }

  /*
   *  If this node has a parent, then put it in that directory list.
   */

  if ( parent ) {
    Chain_Append( &parent->info.directory.Entries, &node->Node );
    node->Parent = parent;

    fs_info = parent_loc->mt_entry->fs_info;
    node->st_ino = ++fs_info->ino_count;
  }


  return node;
}
