/*
 *  IMFS_stat
 *
 *  This routine provides a stat for the IMFS file system.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
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

#include "imfs.h"

#include <dirent.h>
#include <string.h>

static size_t IMFS_directory_size( const IMFS_jnode_t *the_jnode )
{
  size_t size = 0;
  const rtems_chain_control *chain = &the_jnode->info.directory.Entries;
  const rtems_chain_node *current = rtems_chain_immutable_first( chain );
  const rtems_chain_node *tail = rtems_chain_immutable_tail( chain );

  while ( current != tail ) {
    size += sizeof(struct dirent);
    current = rtems_chain_immutable_next( current );
  }

  return size;
}

int IMFS_stat(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
)
{
  IMFS_fs_info_t *fs_info = loc->mt_entry->fs_info;
  IMFS_jnode_t *the_jnode = loc->node_access;
  IMFS_device_t *io = &the_jnode->info.device;

  if ( the_jnode->type == IMFS_HARD_LINK ) {
    the_jnode = the_jnode->info.hard_link.link_node;
  }

  switch ( the_jnode->type ) {

    case IMFS_DEVICE:
      buf->st_rdev = rtems_filesystem_make_dev_t( io->major, io->minor );
      break;

    case IMFS_LINEAR_FILE:
    case IMFS_MEMORY_FILE:
      buf->st_size = the_jnode->info.file.size;
      break;

    case IMFS_DIRECTORY:
      buf->st_size = IMFS_directory_size( the_jnode );
      break;

    case IMFS_SYM_LINK:
      buf->st_size = strlen( the_jnode->info.sym_link.name );
      break;

    case IMFS_FIFO:
      break;

    default:
      rtems_set_errno_and_return_minus_one( ENOTSUP );
      break;
  }

  /*
   * The device number of the IMFS is the major number and the minor is the
   * instance.
   */
  buf->st_dev =
    rtems_filesystem_make_dev_t( IMFS_DEVICE_MAJOR_NUMBER, fs_info->instance );

  buf->st_mode  = the_jnode->st_mode;
  buf->st_nlink = the_jnode->st_nlink;
  buf->st_ino   = the_jnode->st_ino;
  buf->st_uid   = the_jnode->st_uid;
  buf->st_gid   = the_jnode->st_gid;

  buf->st_atime = the_jnode->stat_atime;
  buf->st_mtime = the_jnode->stat_mtime;
  buf->st_ctime = the_jnode->stat_ctime;

  if ( the_jnode->type != IMFS_DIRECTORY ) {
    buf->st_blksize = imfs_rq_memfile_bytes_per_block;
  }

  return 0;
}
