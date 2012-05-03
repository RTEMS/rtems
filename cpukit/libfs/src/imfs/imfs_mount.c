/*
 *  IMFS_mount
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

int IMFS_mount( rtems_filesystem_mount_table_entry_t *mt_entry )
{
  int rv = 0;
  IMFS_jnode_t *node = mt_entry->mt_point_node->location.node_access;

  if ( IMFS_is_directory( node ) ) {
    if ( node->info.directory.mt_fs == NULL ) {
      node->info.directory.mt_fs = mt_entry;
    } else {
      errno = EBUSY;
      rv = -1;
    }
  } else {
    errno = ENOTDIR;
    rv = -1;
  }

  return rv;
}
