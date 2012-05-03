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
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

int IMFS_stat(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
)
{
  IMFS_fs_info_t *fs_info = loc->mt_entry->fs_info;
  IMFS_jnode_t *the_jnode = loc->node_access;

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

  return 0;
}
