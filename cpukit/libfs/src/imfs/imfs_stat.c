/**
 * @file
 *
 * @brief IMFS Stat
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

int IMFS_stat(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
)
{
  const IMFS_fs_info_t *fs_info = loc->mt_entry->fs_info;
  IMFS_jnode_t *the_jnode = loc->node_access;

  buf->st_dev   = rtems_filesystem_make_dev_t_from_pointer( fs_info );
  buf->st_mode  = the_jnode->st_mode;
  buf->st_nlink = the_jnode->st_nlink;
  buf->st_ino   = IMFS_node_to_ino( the_jnode );
  buf->st_uid   = the_jnode->st_uid;
  buf->st_gid   = the_jnode->st_gid;

  buf->st_atime = the_jnode->stat_atime;
  buf->st_mtime = the_jnode->stat_mtime;
  buf->st_ctime = the_jnode->stat_ctime;

  return 0;
}
